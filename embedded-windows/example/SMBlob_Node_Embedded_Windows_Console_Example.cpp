#include <iostream>

#include "SMBlobNodeEmbeddedWindows.h"


int main(int argc, char **argv) {
    const SMBlob::EmbeddedWindows::SMBlobApp &args = SMBlob::EmbeddedWindows::Init();
    auto embeddedWindows = std::make_shared<SMBlob::EmbeddedWindows::SMBlobApp>(args);

    if (embeddedWindows) {
        SMBlob::EmbeddedWindows::Release(*embeddedWindows);
    }
    return 0;
}