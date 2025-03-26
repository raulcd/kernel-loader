#include <iostream>
#include <arrow/api.h>
#include <arrow/acero/exec_plan.h>
#include <arrow/acero/options.h>
#include "arrow/compute/internal/vendored/load_kernels.h"
#include "arrow/compute/internal/vendored/api_vector.h"
#include "arrow/compute/internal/vendored/api_aggregate.h"

arrow::Status CreateIntArray(std::shared_ptr<arrow::Array>& arr) {
    arrow::Int32Builder builder;
    ARROW_RETURN_NOT_OK(builder.Append(1));
    ARROW_RETURN_NOT_OK(builder.Append(2));
    ARROW_RETURN_NOT_OK(builder.Append(3));
    ARROW_ASSIGN_OR_RAISE(arr, builder.Finish())
    return arrow::Status::OK();
}

arrow::Status CreateStrArray(std::shared_ptr<arrow::Array>& arr) {
    arrow::StringBuilder builder;
    ARROW_RETURN_NOT_OK(builder.Append("height"));
    ARROW_RETURN_NOT_OK(builder.Append("width"));
    ARROW_RETURN_NOT_OK(builder.Append("depth"));
    ARROW_ASSIGN_OR_RAISE(arr, builder.Finish())
    return arrow::Status::OK();
}

std::shared_ptr<arrow::Table> GetTable(std::shared_ptr<arrow::Array>& value_array, std::shared_ptr<arrow::Array>& key_array) {
    auto schema = arrow::schema({arrow::field("Value", arrow::int32()), arrow::field("Key", arrow::utf8())});
    std::shared_ptr<arrow::ChunkedArray> values_chunks =
        std::make_shared<arrow::ChunkedArray>(arrow::ArrayVector{value_array});
    std::shared_ptr<arrow::ChunkedArray> keys_chunks =
        std::make_shared<arrow::ChunkedArray>(arrow::ArrayVector{key_array});
    return arrow::Table::Make(schema, {values_chunks, keys_chunks}, 3);
}

std::shared_ptr<arrow::Table> GetTable2(std::shared_ptr<arrow::Array>& value_array) {
    auto schema = arrow::schema({arrow::field("Value", arrow::int32())});
    std::shared_ptr<arrow::ChunkedArray> values_chunks =
        std::make_shared<arrow::ChunkedArray>(arrow::ArrayVector{value_array});
    return arrow::Table::Make(schema, {values_chunks}, 3);
}

int main() {
    std::cout << "Starting validation for kernel-loader" << std::endl;
    std::shared_ptr<arrow::Array> arr;
    auto status = CreateIntArray(arr);
    if (!status.ok()) {
        std::cerr << "Failed to create array: " << status.ToString() << std::endl;
        return -1;
    }

    auto datums {arr};
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
    std::cout << "vendored_rank_quantile: " << result->ToString() << std::endl;

    result = arrow::compute::CallFunction("vendored_rank_normal", {datums}, &options);
    if (!result.ok()) {
        std::cerr << "Failed to call function: " << result.status().ToString() << std::endl;
        return -1;
    }
    std::cout << "vendored_rank_normal: " << result->ToString() << std::endl;

    std::shared_ptr<arrow::Array> arr2;
    status = CreateStrArray(arr2);
    auto datums2 {arr2};

    arrow::compute::internal::vendored::PivotWiderOptions pivot_options = arrow::compute::internal::vendored::PivotWiderOptions::Defaults();
    pivot_options.key_names = {"width", "height"};
    result = arrow::compute::CallFunction("vendored_pivot_wider", {datums2, datums}, &pivot_options);
    if (!result.ok()) {
        std::cerr << "Failed to call function: " << result.status().ToString() << std::endl;
        return -1;
    }
    std::cout << "vendored_pivot_wider: " << result->ToString() << std::endl;


    arrow::compute::Aggregate aggregates{"vendored_hash_pivot_wider", std::make_shared<arrow::compute::internal::vendored::PivotWiderOptions>(pivot_options),
        /*target=*/std::vector<arrow::FieldRef>{"Key", "Value"}, /*name=*/"out"};

    std::vector<arrow::FieldRef> key_refs{arrow::FieldRef("Value")};
    std::vector<arrow::FieldRef> segment_key_refs{arrow::FieldRef("Key")};
    arrow::acero::AggregateNodeOptions aggregate_node_options{{aggregates}, key_refs, segment_key_refs};
    std::shared_ptr<arrow::Table> table = GetTable(arr, arr2);
    arrow::acero::Declaration plan = arrow::acero::Declaration::Sequence(
        {{"table_source", arrow::acero::TableSourceNodeOptions(std::move(table))},
         {"aggregate", aggregate_node_options}});

   bool use_threads = false;
   arrow::MemoryPool* memory_pool = arrow::default_memory_pool();
   result = arrow::acero::DeclarationToTable(std::move(plan), use_threads, memory_pool);
   std::cout << "vendored_hash_pivot_wider: " <<  result->ToString() << std::endl;

   arrow::compute::internal::vendored::SkewOptions skew_options = arrow::compute::internal::vendored::SkewOptions::Defaults();
   result = arrow::compute::CallFunction("vendored_skew", {datums}, &skew_options);
   if (!result.ok()) {
       std::cerr << "Failed to call function: " << result.status().ToString() << std::endl;
       return -1;
   }
   std::cout << "vendored_skew: " << result->ToString() << std::endl;
   result = arrow::compute::CallFunction("vendored_kurtosis", {datums}, &skew_options);
   if (!result.ok()) {
       std::cerr << "Failed to call function: " << result.status().ToString() << std::endl;
       return -1;
   }
   std::cout << "vendored_kurtosis: " << result->ToString() << std::endl;

   arrow::compute::Aggregate aggregates2{"vendored_hash_skew", std::make_shared<arrow::compute::internal::vendored::SkewOptions>(skew_options),
       /*target=*/std::vector<arrow::FieldRef>{"Value"}, /*name=*/"out"};

   arrow::acero::AggregateNodeOptions aggregate_node_options2{{aggregates2}, key_refs};
   std::shared_ptr<arrow::Table> table2 = GetTable2(arr);
   arrow::acero::Declaration plan2 = arrow::acero::Declaration::Sequence(
       {{"table_source", arrow::acero::TableSourceNodeOptions(std::move(table2))},
        {"aggregate", aggregate_node_options2}});
    result = arrow::acero::DeclarationToTable(std::move(plan2), use_threads, memory_pool);
    std::cout << "vendored_hash_skew: " <<  result->ToString() << std::endl;

   return 0;
}
