#include <arrow/compute/registry.h>
#include <arrow/compute/ordering.h>
#include <arrow/compute/function_options.h>

namespace arrow::compute::internal {
    class ARROW_EXPORT RankPercentileOptions : public FunctionOptions {
 public:
 explicit RankPercentileOptions(std::vector<SortKey> sort_keys = {},
                                 double factor = 1.0);
  /// Convenience constructor for array inputs
  explicit RankPercentileOptions(SortOrder order,
                                 double factor = 1.0)
      : RankPercentileOptions({SortKey("", order)}, factor) {}

  static constexpr char const kTypeName[] = "RankPercentileOptions";
  static RankPercentileOptions Defaults() { return RankPercentileOptions(); }

  /// Column key(s) to order by and how to order by these sort keys.
  std::vector<SortKey> sort_keys;
  /// Factor to apply to the output.
  /// Use 1.0 for results in (0, 1), 100.0 for percentages, etc.
  double factor;
};
void RegisterVectorRank(FunctionRegistry* registry);
}  // namespace arrow::compute::internal