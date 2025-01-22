#include <arrow/compute/registry.h>
#include <arrow/compute/ordering.h>
#include <arrow/compute/function_options.h>

namespace arrow::compute::internal {
    class ARROW_EXPORT RankQuantileOptions : public FunctionOptions {
 public:
 explicit RankQuantileOptions(std::vector<SortKey> sort_keys = {},
                              NullPlacement null_placement = NullPlacement::AtEnd,
                              double factor = 1.0);
  /// Convenience constructor for array inputs
  explicit RankQuantileOptions(SortOrder order,
                               NullPlacement null_placement = NullPlacement::AtEnd,
                               double factor = 1.0)
      : RankQuantileOptions({SortKey("", order)}, null_placement, factor) {}

  static constexpr char const kTypeName[] = "RankQuantileOptions";
  static RankQuantileOptions Defaults() { return RankQuantileOptions(); }

  /// Column key(s) to order by and how to order by these sort keys.
  std::vector<SortKey> sort_keys;
  /// Whether nulls and NaNs are placed at the start or at the end
  NullPlacement null_placement;
  /// Factor to apply to the output.
  /// Use 1.0 for results in (0, 1), 100.0 for percentages, etc.
  double factor;
};
void RegisterVectorRank(FunctionRegistry* registry);
}  // namespace arrow::compute::internal