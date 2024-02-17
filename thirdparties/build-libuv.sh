#! /bin/sh
rm -rf ./libuv/build
mkdir -p ./libuv/build && cd ./libuv/build

cmake -G "Ninja" .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build . -- -j4

sudo cmake  --build .  --config Release --target install -j4