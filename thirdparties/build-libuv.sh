#! /bin/sh
rm -rf ./libuv/build
mkdir -p ./libuv/build && cd ./libuv/build

cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build . -- -j4