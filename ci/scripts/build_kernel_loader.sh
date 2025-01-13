set -eux

source_dir=${1}
build_dir=${1}/build

LOADER_CMAKE_PREFIX_PATH=${2:-}

mkdir ${build_dir}
pushd ${build_dir}

cmake \
    -DCMAKE_INSTALL_PREFIX=/tmp/kernel-loader \
    -DARROW_LINK_SHARED=ON \
    -DCMAKE_PREFIX_PATH=${LOADER_CMAKE_PREFIX_PATH:-}
    ${source_dir}
cmake --build . --target install

popd