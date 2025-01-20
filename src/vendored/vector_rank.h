#include <arrow/compute/registry.h>
#include <arrow/compute/ordering.h>
#include <arrow/compute/function_options.h>

namespace arrow::compute::internal {
    class ARROW_EXPORT RankQuantileOptions : public FunctionOptions {
 public:
 explicit RankQuantileOptions(std::vector<SortKey> sort_keys = {},
                                 double factor = 1.0);
  /// Convenience constructor for array inputs
  explicit RankQuantileOptions(SortOrder order,
                                 double factor = 1.0)
      : RankQuantileOptions({SortKey("", order)}, factor) {}

  static constexpr char const kTypeName[] = "RankQuantileOptions";
  static RankQuantileOptions Defaults() { return RankQuantileOptions(); }

  /// Column key(s) to order by and how to order by these sort keys.
  std::vector<SortKey> sort_keys;
  /// Factor to apply to the output.
  /// Use 1.0 for results in (0, 1), 100.0 for percentages, etc.
  double factor;
};
void RegisterVectorRank(FunctionRegistry* registry);
}  // namespace arrow::compute::internal