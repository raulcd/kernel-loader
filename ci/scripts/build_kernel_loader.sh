set -eux

source_dir=${1}
build_dir=${1}/build

rm -rf ${build_dir}
mkdir -p ${build_dir}
pushd ${build_dir}

mkdir arrow_tmp
pushd arrow_tmp

sha="14e45a24957f76d3aea8a594d9c308ba0dee1dd7"

# Download Arrow source code
url="https://github.com/apache/arrow/archive/${sha}.zip"
curl -L -o arrow_vendored.zip ${url}
unzip arrow_vendored.zip
cp ../../src/rank_kernel.patch arrow-${sha}

pushd arrow-${sha}
# Apply the required patch
patch -p1 -i rank_kernel.patch
popd

popd

# Copy only the vendored files to compile against old Arrow instead of building Arrow from clone

mkdir -p arrow_vendored/cpp/src/arrow/compute/kernels
mkdir -p arrow_vendored/cpp/src/arrow/util

mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/api_vector.cc arrow_vendored/cpp/src/arrow/compute/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/api_vector.h arrow_vendored/cpp/src/arrow/compute/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/function_internal.cc arrow_vendored/cpp/src/arrow/compute/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/function_internal.h arrow_vendored/cpp/src/arrow/compute/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/chunked_internal.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/util/reflection_internal.h arrow_vendored/cpp/src/arrow/util/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/vector_sort_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/chunked_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/chunk_resolver.cc arrow_vendored/cpp/src/arrow/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/chunk_resolver.h arrow_vendored/cpp/src/arrow/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/codegen_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/vector_rank.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/vector_array_sort.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/vector_sort.cc arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/common_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/
mv arrow_tmp/arrow-${sha}/cpp/src/arrow/compute/kernels/util_internal.h arrow_vendored/cpp/src/arrow/compute/kernels/

# Build the kernel loader
cmake \
    -DCMAKE_INSTALL_PREFIX=/tmp/kernel-loader \
    -DARROW_LINK_SHARED=ON \
    ${source_dir}
cmake --build .

popd