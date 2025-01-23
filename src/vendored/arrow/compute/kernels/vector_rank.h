#include <arrow/compute/registry.h>
#include <arrow/compute/ordering.h>
#include <arrow/compute/function_options.h>

namespace arrow::compute::internal {

void RegisterVectorRank(FunctionRegistry* registry);
}  // namespace arrow::compute::internal