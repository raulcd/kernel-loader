#include "vendored/vector_rank.h"
#include <arrow/compute/function.h>
#include <arrow/compute/function_options.h>
#include <vendored/function_internal.h>
#include <vendored/reflection_internal.h>
#include <vendored/vector_sort_internal.h>
#include <vendored/codegen_internal.h>
#include "arrow/util/logging.h"
#include <iostream>
namespace arrow::compute::internal {


using NullPartitionResult = GenericNullPartitionResult<uint64_t>;

    static auto kRankQuantileOptionsType = GetFunctionOptionsType<RankQuantileOptions>(
      arrow::internal::DataMember("sort_keys", &RankQuantileOptions::sort_keys),
      /*

       TODO: Requires fixing. It fails to compile with:

      no matching function for call to
      ‘GenericToScalar(const arrow::internal::DataMemberProperty<arrow::compute::internal::RankQuantileOptions,
                       arrow::compute::NullPlacement>::Type&)’
      */
      // arrow::internal::DataMember("null_placement", &RankQuantileOptions::null_placement),
      arrow::internal::DataMember("factor", &RankQuantileOptions::factor));

    RankQuantileOptions::RankQuantileOptions(std::vector<SortKey> sort_keys,
                                             NullPlacement null_placement, double factor)
    : FunctionOptions(internal::kRankQuantileOptionsType),
        sort_keys(std::move(sort_keys)),
        null_placement(null_placement),
        factor(factor) {}

constexpr char RankQuantileOptions::kTypeName[];
namespace
{

    const FunctionDoc rank_quantile_doc(
    "Compute quantile ranks of an array",
    ("This function computes a quantile rank of the input array.\n"
     "By default, null values are considered greater than any other value and\n"
     "are therefore sorted at the end of the input. For floating-point types,\n"
     "NaNs are considered greater than any other non-null value, but smaller\n"
     "than null values.\n"
     "Results are computed as in https://en.wikipedia.org/wiki/Percentile_rank\n"
     "\n"
     "The handling of nulls and NaNs, and the constant factor can be changed\n"
     "in RankQuantileOptions."),
    {"input"}, "RankQuantileOptions");

// A bit that is set in the sort indices when the value at the current sort index
// is the same as the value at the previous sort index.
constexpr uint64_t kDuplicateMask = 1ULL << 63;

template <typename ValueSelector>
void MarkDuplicates(const NullPartitionResult& sorted, ValueSelector&& value_selector) {
  using T = decltype(value_selector(int64_t{}));

  // Process non-nulls
  if (sorted.non_nulls_end != sorted.non_nulls_begin) {
    auto it = sorted.non_nulls_begin;
    T prev_value = value_selector(*it);
    while (++it < sorted.non_nulls_end) {
      T curr_value = value_selector(*it);
      if (curr_value == prev_value) {
        *it |= kDuplicateMask;
      }
      prev_value = curr_value;
    }
  }

  // Process nulls
  if (sorted.nulls_end != sorted.nulls_begin) {
    // TODO this should be able to distinguish between NaNs and real nulls (GH-45193)
    auto it = sorted.nulls_begin;
    while (++it < sorted.nulls_end) {
      *it |= kDuplicateMask;
    }
  }
}


template <typename ArrowType>
Result<NullPartitionResult> DoSortAndMarkDuplicate(
    ExecContext* ctx, uint64_t* indices_begin, uint64_t* indices_end, const Array& input,
    const std::shared_ptr<DataType>& physical_type, const SortOrder order,
    bool needs_duplicates) {
  using GetView = GetViewType<ArrowType>;
  using ArrayType = typename TypeTraits<ArrowType>::ArrayType;

  ARROW_ASSIGN_OR_RAISE(auto array_sorter, GetArraySorter(*physical_type));

  ArrayType array(input.data());
  ARROW_ASSIGN_OR_RAISE(auto sorted,
                        array_sorter(indices_begin, indices_end, array, 0,
                                     ArraySortOptions(order, NullPlacement::AtStart), ctx));
   if (needs_duplicates) {
    auto value_selector = [&array](int64_t index) {
      return GetView::LogicalValue(array.GetView(index));
    };
    MarkDuplicates(sorted, value_selector);
  }
  return sorted;
}

template <typename InputType>
class SortAndMarkDuplicate : public TypeVisitor {
 public:
  SortAndMarkDuplicate(ExecContext* ctx, uint64_t* indices_begin, uint64_t* indices_end,
                       const InputType& input, const SortOrder order,
                       const bool needs_duplicate)
      : TypeVisitor(),
        ctx_(ctx),
        indices_begin_(indices_begin),
        indices_end_(indices_end),
        input_(input),
        order_(order),
        needs_duplicates_(needs_duplicate),
        physical_type_(GetPhysicalType(input.type())) {}

  Result<NullPartitionResult> Run() {
    RETURN_NOT_OK(physical_type_->Accept(this));
    return sorted_;
  }

#define VISIT(TYPE)                                                                 \
  Status Visit(const TYPE& type) {                                                  \
    ARROW_ASSIGN_OR_RAISE(                                                          \
        sorted_, DoSortAndMarkDuplicate<TYPE>(ctx_, indices_begin_, indices_end_,   \
                                              input_, physical_type_, order_,       \
                                              needs_duplicates_));                  \
    return Status::OK();                                                            \
  }

  VISIT_SORTABLE_PHYSICAL_TYPES(VISIT)

#undef VISIT

 private:
  ExecContext* ctx_;
  uint64_t* indices_begin_;
  uint64_t* indices_end_;
  const InputType& input_;
  const SortOrder order_;
  const bool needs_duplicates_;
  const std::shared_ptr<DataType> physical_type_;
  NullPartitionResult sorted_{};
};

template <typename Derived>
class RankMetaFunctionBase : public MetaFunction {
 public:
 using FunctionOptionsType = RankQuantileOptions;
  using MetaFunction::MetaFunction;

  Result<Datum> ExecuteImpl(const std::vector<Datum>& args,
                            const FunctionOptions* options,
                            ExecContext* ctx) const override {
    switch (args[0].kind()) {
      case Datum::ARRAY: {
        return Rank(*args[0].make_array(), *options, ctx);
      } break;
      default:
        break;
    }
    return Status::NotImplemented(
        "Unsupported types for rank operation: "
        "values=",
        args[0].ToString());
  }
 protected:
  template <typename T>
  Result<Datum> Rank(const T& input, const FunctionOptions& function_options,
                     ExecContext* ctx) const {
    const auto& options =
        checked_cast<const typename Derived::FunctionOptionsType&>(function_options);

    SortOrder order = SortOrder::Ascending;
    if (!options.sort_keys.empty()) {
      order = options.sort_keys[0].order;
    }

    int64_t length = input.length();
    std::cout << "Input length to Rank: " << length << std::endl;
    ARROW_ASSIGN_OR_RAISE(auto indices,
                          MakeMutableUInt64Array(length, ctx->memory_pool()));
    auto* indices_begin = indices->GetMutableValues<uint64_t>(1);
    auto* indices_end = indices_begin + length;
    std::iota(indices_begin, indices_end, 0);
    auto needs_duplicates = Derived::NeedsDuplicates(options);
    ARROW_ASSIGN_OR_RAISE(
        auto sorted, SortAndMarkDuplicate(ctx, indices_begin, indices_end, input, order,
                                          needs_duplicates)
                         .Run());

    auto ranker = Derived::GetRanker(options);
    return ranker.CreateRankings(ctx, sorted);
  }
};
inline Result<std::shared_ptr<ArrayData>> MakeMutableFloat64Array(
    int64_t length, MemoryPool* memory_pool) {
  auto buffer_size = length * sizeof(double);
  ARROW_ASSIGN_OR_RAISE(auto data, AllocateBuffer(buffer_size, memory_pool));
  return ArrayData::Make(float64(), length, {nullptr, std::move(data)}, /*null_count=*/0);
}


// A helper class that emits rankings for the "rank_quantile" function
struct QuantileRanker {
  explicit QuantileRanker(double factor) : factor_(factor) {}

  Result<Datum> CreateRankings(ExecContext* ctx, const NullPartitionResult& sorted) {
    std::cout << "CreateRankings" << std::endl;
    const int64_t length = sorted.overall_end() - sorted.overall_begin();
    ARROW_ASSIGN_OR_RAISE(auto rankings,
                          MakeMutableFloat64Array(length, ctx->memory_pool()));
    auto out_begin = rankings->GetMutableValues<double>(1);

    auto is_duplicate = [](uint64_t index) { return (index & kDuplicateMask) != 0; };
    auto original_index = [](uint64_t index) { return index & ~kDuplicateMask; };

    // The count of values strictly less than the value being considered
    int64_t cum_freq = 0;
    auto it = sorted.overall_begin();

    while (it < sorted.overall_end()) {
      // Look for a run of duplicate values
      DCHECK(!is_duplicate(*it));
      auto run_end = it;
      while (++run_end < sorted.overall_end() && is_duplicate(*run_end)) {
      }
      // The run length, i.e. the frequency of the current value
      int64_t freq = run_end - it;
      double quantile = (cum_freq + 0.5 * freq) * factor_ / static_cast<double>(length);
      // Output quantile rank values
      for (; it < run_end; ++it) {
        out_begin[original_index(*it)] = quantile;
      }
      cum_freq += freq;
    }
    DCHECK_EQ(cum_freq, length);
    return Datum(rankings);
  }

 private:
  const double factor_;
};

const RankQuantileOptions* GetDefaultQuantileRankOptions() {
  static const auto kDefaultQuantileRankOptions = RankQuantileOptions::Defaults();
  return &kDefaultQuantileRankOptions;
}



    class RankQuantileMetaFunction : public RankMetaFunctionBase<RankQuantileMetaFunction>
    {
        public:
        using RankerType = QuantileRanker;
        static bool NeedsDuplicates(const RankQuantileOptions&) { return true; }
        static RankerType GetRanker(const RankQuantileOptions& options) {
            return RankerType(options.factor);
        }
          RankQuantileMetaFunction()
      : RankMetaFunctionBase("vendored_rank_quantile", Arity::Unary(), rank_quantile_doc,
                             GetDefaultQuantileRankOptions()) {}

    };
    
} // namespace

void RegisterVectorRank(FunctionRegistry* registry) {
  //DCHECK_OK(registry->AddFunction(std::make_shared<RankMetaFunction>()));
  DCHECK_OK(registry->AddFunction(std::make_shared<RankQuantileMetaFunction>()));
}
}  // namespace arrow::compute::internal