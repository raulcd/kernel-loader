#include <iostream>
#include <arrow/api.h>
#include "arrow/compute/internal/vendored/load_kernels.h"
#include "arrow/compute/internal/vendored/api_vector.h"

arrow::Status CreateArray(std::shared_ptr<arrow::Array>& arr) {
    arrow::Int32Builder builder;
    ARROW_RETURN_NOT_OK(builder.Append(1));
    ARROW_RETURN_NOT_OK(builder.Append(2));
    ARROW_RETURN_NOT_OK(builder.Append(3));
    ARROW_ASSIGN_OR_RAISE(arr, builder.Finish())
    std::cout << arr->ToString() << std::endl;
    return arrow::Status::OK();
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::shared_ptr<arrow::Array> arr;
    auto status = CreateArray(arr);
    if (!status.ok()) {
        std::cerr << "Failed to create array: " << status.ToString() << std::endl;
        return -1;
    }

    std::vector<arrow::Datum> datums {arr};
    status = arrow::compute::internal::vendored::LoadKernels(arrow::compute::GetFunctionRegistry());
    if (!status.ok()) {
        std::cerr << "Failed to load kernels: " << status.ToString() << std::endl;
        return -1;
    }

    arrow::compute::internal::vendored::RankQuantileOptions options = arrow::compute::internal::vendored::RankQuantileOptions::Defaults();
    auto result = arrow::compute::CallFunction("vendored_rank_quantile", {datums}, &options);
    if (!result.ok()) {
        std::cerr << "Failed to call function: " << result.status().ToString() << std::endl;
        return -1;
    }

    std::cout << result->ToString() << std::endl;
    return 0;
}
