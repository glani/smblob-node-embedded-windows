#include <iostream>
#include "SMBlobNodeEmbeddedWindows.h"


int main(int argc, char **argv) {
    SMBlob::EmbeddedWindows::SMBlobAppInitConsumer params;
    const SMBlob::EmbeddedWindows::SMBlobApp &args = SMBlob::EmbeddedWindows::Init(params);
    auto embeddedWindows = std::make_shared<SMBlob::EmbeddedWindows::SMBlobApp>(args);

    if (embeddedWindows) {
        SMBlob::EmbeddedWindows::Release(*embeddedWindows);
    }
    return 0;
}