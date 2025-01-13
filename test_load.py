import ctypes
import pyarrow as pa
import pyarrow.compute as pc

# Load the shared library
lib = ctypes.CDLL('/home/runner/work/kernel-loader/kernel-loader/build/libarrow_kernel_loader.so')
res = lib.LoadKernels()
assert res == 0
array1 = pa.array([1,2])
result = pc.call_function("custom_add", [array1, array1])

assert result == pa.array([2, 4])
