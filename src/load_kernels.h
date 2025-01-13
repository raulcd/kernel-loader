#ifndef LOAD_KERNELS_H
#define LOAD_KERNELS_H

namespace arrow {
    namespace compute {
        Status LoadKernels(FunctionRegistry* registry);
    }  // namespace compute
}  // namespace arrow

extern "C" int LoadKernels();

#endif  // LOAD_KERNELS_H