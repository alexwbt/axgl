#! /bin/sh

DIR="$(cd "$(dirname "$0")" && pwd)"

# configure
cmake \
    -DGLFW_BUILD_DOCS=OFF \
    -B $DIR/../_build
