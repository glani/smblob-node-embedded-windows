#include <string>
#include <iostream>
#include "SMBlobNodeEmbeddedWindows.h"


int main(int argc, char **argv) {
    std::string daemonExec;
#ifdef DAEMON_PATH
    daemonExec = std::string(DAEMON_PATH);
    daemonExec.append("").append(
            "embedded-windows/daemon/smblob-node-embedded-windows-daemon"
            );
#endif
    SMBlob::EmbeddedWindows::SMBlobAppInitConsumer params;
    params.daemonExec = daemonExec;
    const SMBlob::EmbeddedWindows::SMBlobApp &args = SMBlob::EmbeddedWindows::Init(params);
    auto embeddedWindows = std::make_shared<SMBlob::EmbeddedWindows::SMBlobApp>(args);

    if (embeddedWindows) {
        SMBlob::EmbeddedWindows::Release(*embeddedWindows);
    }
    return 0;
}