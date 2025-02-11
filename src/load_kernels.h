#ifndef LOAD_KERNELS_H
#define LOAD_KERNELS_H

namespace arrow::compute::vendored::internal {
        arrow::Status LoadKernels(arrow::compute::FunctionRegistry* registry);
}  // namespace arrow::compute::vendored::internal

extern "C" int LoadKernels();

#endif  // LOAD_KERNELS_H