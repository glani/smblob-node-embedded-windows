#! /bin/sh
rm -rf ./protobuf/build
mkdir -p ./protobuf/build && cd ./protobuf/build

cmake -G "Ninja" .. -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=OFF
cmake --build . --config Release  -- -j4
# Install
sudo cmake  --build .  --config Release --target install -j4