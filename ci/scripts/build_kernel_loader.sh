set -eux

source_dir=${1}
build_dir=${1}/build

rm -rf ${build_dir}
mkdir -p ${build_dir}
pushd ${build_dir}

cmake \
    -DCMAKE_INSTALL_PREFIX=/tmp/kernel-loader \
    -DARROW_LINK_SHARED=ON \
    ${source_dir}
cmake --build .

popd