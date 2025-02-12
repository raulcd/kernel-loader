set -eux

source_dir=${1}/tests
build_dir=${1}/build-test

rm -rf ${build_dir}
mkdir -p ${build_dir}
pushd ${build_dir}

# Build the kernel loader
cmake \
    -DCMAKE_INSTALL_PREFIX=/tmp/kernel-loader \
    -DARROW_LINK_SHARED=ON \
    ${source_dir}
cmake --build .

popd