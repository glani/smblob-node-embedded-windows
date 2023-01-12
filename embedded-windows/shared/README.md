# Generate protoc models

    rm -rf ./src/models
    mkdir -p ./src/models
    protoc --cpp_out ./src/models embedded-windows.scheme.proto