#include <arrow/api.h>
#include <arrow/compute/api.h>
#include "load_kernels.h"
#include "arrow/compute/kernels/vector_rank.h"
#include "arrow/compute/kernels/aggregate_pivot.h"

namespace arrow::compute::internal::vendored {

        Status ExecAddInt32(KernelContext* ctx, const ExecSpan& batch, ExecResult* out) {
            const int32_t* left_data = batch[0].array.GetValues<int32_t>(1);
            const int32_t* right_data = batch[1].array.GetValues<int32_t>(1);
            int32_t* out_data = out->array_span_mutable()->GetValues<int32_t>(1);
            for (int64_t i = 0; i < batch.length; ++i) {
                *out_data++ = *left_data++ + *right_data++;
            }
            return Status::OK();
        }

        Status ExecAddInt64(KernelContext* ctx, const ExecSpan& batch, ExecResult* out) {
            const int64_t* left_data = batch[0].array.GetValues<int64_t>(1);
            const int64_t* right_data = batch[1].array.GetValues<int64_t>(1);
            int64_t* out_data = out->array_span_mutable()->GetValues<int64_t>(1);
            for (int64_t i = 0; i < batch.length; ++i) {
                *out_data++ = *left_data++ + *right_data++;
            }
            return Status::OK();
        }

        Status LoadKernels(FunctionRegistry* registry) {
            // Define and register custom compute functions
            auto func = std::make_shared<ScalarFunction>("custom_add", Arity::Binary(),
                                                 /*doc=*/FunctionDoc::Empty());
            // Add Int64 kernel
            ScalarKernel kernel64;
            kernel64.exec = ExecAddInt64;
            kernel64.signature = KernelSignature::Make({int64(), int64()}, int64());
            ARROW_RETURN_NOT_OK(func->AddKernel(kernel64));

            // Add kernel implementation
            ARROW_RETURN_NOT_OK(registry->AddFunction(std::move(func)));
            RegisterVectorRank(registry);
            RegisterScalarAggregatePivot(registry);

            return arrow::Status::OK();
        }
}  // namespace arrow::compute::internal::vendored

extern "C" int LoadKernels() {
    auto status = arrow::compute::internal::vendored::LoadKernels(arrow::compute::GetFunctionRegistry());
    if (!status.ok()) {
      return -1;
    } else {
      return 0;
    }
}