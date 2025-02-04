#ifndef LOAD_KERNELS_H
#define LOAD_KERNELS_H

namespace vendored_arrow {
    namespace compute {
        arrow::Status LoadKernels(arrow::compute::FunctionRegistry* registry);
    }  // namespace compute
}  // namespace vendored_arrow

extern "C" int LoadKernels();

#endif  // LOAD_KERNELS_H