#include <string>
#include <iostream>
#include "SMBlobNodeEmbeddedWindows.h"
#include "SMBlob_Node_Embedded_Windows_Console_Example.h"


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
    params.debug = true;
    const SMBlob::EmbeddedWindows::SMBlobApp &args = SMBlob::EmbeddedWindows::Init(params);
    auto embeddedWindows = std::make_shared<SMBlob::EmbeddedWindows::SMBlobApp>(args);


    std::cout << std::endl;
    PressAnyKey( "Press any button: " );

    if (embeddedWindows) {
        SMBlob::EmbeddedWindows::Release(*embeddedWindows);
    }
    return 0;
}