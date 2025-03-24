set -eux

source_dir=${1}
build_dir=${1}/build

rm -rf ${build_dir}
mkdir -p ${build_dir}
pushd ${build_dir}

mkdir arrow_tmp
pushd arrow_tmp

sha="026a933fd15fdd2b0bf0b1ae21dbf4f9dd5c8d85"

# Download Arrow source code
url="https://github.com/apache/arrow/archive/${sha}.zip"
curl -L -o arrow_vendored.zip ${url}
unzip arrow_vendored.zip
cp ../../src/kernels.patch arrow-${sha}

pushd arrow-${sha}
# Apply the required patch
patch -p1 -i kernels.patch
popd

popd

# Copy only the vendored files to compile against old Arrow instead of building Arrow from clone

mkdir -p arrow_vendored/cpp/src/arrow/compute/kernels
mkdir -p arrow_vendored/cpp/src/arrow/compute/row
mkdir -p arrow_vendored/cpp/src/arrow/util

# Rank Kernel requirements
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/api_vector.cc arrow_vendored/cpp/src/arrow/compute/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/api_vector.h arrow_vendored/cpp/src/arrow/compute/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/function_internal.h arrow_vendored/cpp/src/arrow/compute/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/function_internal.cc arrow_vendored/cpp/src/arrow/compute/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/chunked_internal.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/util/math_internal.h arrow_vendored/cpp/src/arrow/util/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/util/math_internal.cc arrow_vendored/cpp/src/arrow/util/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/util/reflection_internal.h arrow_vendored/cpp/src/arrow/util/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/vector_sort_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/chunked_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/codegen_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/codegen_internal.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/vector_rank.cc arrow_vendored/cpp/src/arrow/compute/kernels/
# Pivot Kernel requirements
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/api_aggregate.cc arrow_vendored/cpp/src/arrow/compute/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/api_aggregate.h arrow_vendored/cpp/src/arrow/compute/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/aggregate_pivot.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/hash_aggregate_pivot.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/hash_aggregate_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/aggregate_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/util_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/util_internal.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/util/int128_internal.h arrow_vendored/cpp/src/arrow/util/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/common_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/pivot_internal.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/pivot_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/aggregate_basic.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/aggregate_basic.inc.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/aggregate_basic_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
# Skew and kurtosis kernels
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/aggregate_var_std.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/aggregate_var_std_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/hash_aggregate_numeric.cc arrow_vendored/cpp/src/arrow/compute/kernels/
# Build the kernel loader
cmake \
    -DCMAKE_INSTALL_PREFIX=/tmp/kernel-loader \
    -DARROW_LINK_SHARED=ON \
    ${source_dir}
cmake --build . --target install

popd
