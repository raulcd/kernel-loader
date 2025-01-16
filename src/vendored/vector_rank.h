#ifndef RANK_PERCENTILE_META_FUNCTION_H
#define RANK_PERCENTILE_META_FUNCTION_H

// #include <arrow/compute/function_options.h>

namespace arrow {
namespace compute {
namespace internal {

void RegisterVectorRank(FunctionRegistry* registry);

}  // namespace internal
}  // namespace compute
}  // namespace arrow

#endif  // RANK_PERCENTILE_META_FUNCTION_H