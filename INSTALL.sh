#/bin/sh

rm -rf CMakeFiles
rm -rf CMakeCache.txt
rm -rf cmake_install.cmake

# build zydis
if [ ! -d zydis ]; then
    git submodule update --init --recursive
fi
cd zydis
if [ ! -d build ]; then
    mkdir build && cd build
    cmake ..
    make
fi
cd ..

# build tracer
if [ -d build ]; then
   rm -rf build
fi

mkdir build && cd build
cmake ..
make

cd ..
