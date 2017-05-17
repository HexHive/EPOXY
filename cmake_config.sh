#!/bin/bash
# needs to be run EPOXY dir

cd ..
mkdir -p llvm_build
mkdir -p llvm_bins
cd llvm_build

cmake ../EPOXY-llvm -GNinja \
       -DCMAKE_BUILD_TYPE=Debug \
       -DCMAKE_C_COMPILER=clang \
       -DCMAKE_CXX_COMPILER=clang++ \
       -DLLVM_ENABLE_ASSERTIONS=ON  \
       -DLLVM_BUILD_TESTS=OFF  \
       -DLLVM_BUILD_EXAMPLES=OFF  \
       -DLLVM_INCLUDE_TESTS=OFF \
       -DLLVM_INCLUDE_EXAMPLES=OFF \
       -DLLVM_TARGETS_TO_BUILD="X86;ARM" \
       -DBUILD_SHARED_LIBS=ON  \
       -DLLVM_BINUTILS_INCDIR=/usr/include \
       -DCMAKE_C_FLAGS=-fstandalone-debug \
       -DCMAKE_CXX_FLAGS=-fstandalone-debug \
       -DCMAKE_INSTALL_PREFIX=../llvm_bin \
