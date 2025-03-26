import ctypes
import pyarrow as pa
import pyarrow.compute as pc

# Load the shared library
lib = ctypes.CDLL('/home/runner/work/kernel-loader/kernel-loader/build/libarrow_kernel_loader.so')

res = lib.LoadKernels()
assert res == 0
array1 = pa.array([2, 1 ,2])

result = pc.call_function("vendored_rank_quantile", [array1])
print(f"Rank Quantile: {result}")

chunked_array1 = pa.chunked_array([[2, 1 ,2], [1,3]])

result = pc.call_function("vendored_rank_quantile", [chunked_array1])
print(f"Rank Quantile for chunked array: {result}")

# Function options not exposed in pyarrow
result = pc.call_function("vendored_pivot_wider", [["height", "width", "depth"], [10, None, 11]])
print(f"Pivot Wider: {result}")

result = pc.call_function("vendored_skew", [chunked_array1])
print(f"Skew for chunked array: {result}")

result = pc.call_function("vendored_kurtosis", [chunked_array1])
print(f"Kurtosis for chunked array: {result}")