#include <string>
#include <iostream>
#include "SMBlobNodeEmbeddedWindows.h"
#include "SMBlob_Node_Embedded_Windows_Console_Example.h"
#include "ProcessRunner.h"
#include "plog/Log.h"
#include "plog/Init.h"
#include "plog/Appenders/ConsoleAppender.h"
#include "plog/Formatters/TxtFormatter.h"


int main(int argc, char **argv) {
    std::string daemonExec;
#ifdef DAEMON_PATH
    daemonExec = std::string(DAEMON_PATH);
    daemonExec.append("").append(
            "embedded-windows/daemon/smblob-node-embedded-windows-daemon"
            );
#endif

    plog::init<SecondLog>(plog::Severity::debug);

    auto consoleAppender = std::make_shared<plog::ConsoleAppender<plog::TxtFormatter>>();
    plog::get<SecondLog>()->addAppender(consoleAppender.get());


    ProcessRunner runner;
    char *rargv[1] = {"-l"};
    runner.RunCommand("/usr/bin/wmctrl", rargv, 1);

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