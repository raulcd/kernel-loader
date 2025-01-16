#include "vendored/api_vector.h"
#include "vendored/function_internal.h"
#include "arrow/compute/api_scalar.h"
#include "arrow/compute/ordering.h"
#include "vendored/vector_sort_internal.h"


namespace arrow {
namespace compute {
namespace internal {

using compute::NullPlacement;

  static auto kRankPercentileOptionsType = GetFunctionOptionsType<RankPercentileOptions>(
    arrow::internal::DataMember("sort_keys", &RankPercentileOptions::sort_keys),
    arrow::internal::DataMember("null_placement", &RankPercentileOptions::null_placement),
    arrow::internal::DataMember("factor", &RankPercentileOptions::factor));
}  // namespace internal
RankPercentileOptions::RankPercentileOptions(std::vector<SortKey> sort_keys,
                                             NullPlacement null_placement, double factor)
    : FunctionOptions(arrow::compute::internal::kRankPercentileOptionsType),
      sort_keys(std::move(sort_keys)),
      null_placement(null_placement),
      factor(factor) {}

constexpr char RankPercentileOptions::kTypeName[];

}  // namespace compute
}  // namespace arrow