#ifndef LOAD_KERNELS_H
#define LOAD_KERNELS_H

namespace arrow::compute::internal::vendored {
        arrow::Status LoadKernels(arrow::compute::FunctionRegistry* registry);
}  // namespace arrow::compute::internal::vendored

extern "C" int LoadKernels();

#endif  // LOAD_KERNELS_H