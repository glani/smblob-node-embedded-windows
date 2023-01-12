#! /bin/sh
rm -rf ./protobuf/build
mkdir -p ./protobuf/build && cd ./protobuf/build

cmake -G "Ninja" .. -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF
cmake --build . -- -j4