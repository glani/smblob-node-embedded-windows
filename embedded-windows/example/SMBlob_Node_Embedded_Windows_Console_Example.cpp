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
    std::string daemonLog;
#ifdef DAEMON_PATH
    daemonExec = std::string(DAEMON_PATH);
    daemonExec.append("").append(
            "embedded-windows/daemon/smblob-node-embedded-windows-daemon"
            );
    daemonLog = std::string(DAEMON_PATH);
    daemonLog.append("").append(
            "log-smblob-node-embedded-windows-daemon.log"
    );
#endif

    plog::init<SecondLog>(plog::Severity::debug);

    auto consoleAppender = std::make_shared<plog::ConsoleAppender<plog::TxtFormatter>>();
    plog::get<SecondLog>()->addAppender(consoleAppender.get());

    ProcessRunner runner;
    auto runnerArgvPtr = std::unique_ptr<char*[]>(
            new char*[3]{ "search", "--name", "New Tab" }
    );
    runner.RunCommand("/usr/bin/xdotool", runnerArgvPtr.get(), 3);

    PLOGD_(SecondLog) << "window Id:" << runner.getOutput();

    SMBlob::EmbeddedWindows::SMBlobAppInitConsumer params;
    params.daemonExec = daemonExec;
    params.debug = true;
    params.logDaemonFilename = daemonLog;

    SMBlob::EmbeddedWindows::SMBlobApp args = SMBlob::EmbeddedWindows::Init(params);
    auto embeddedWindows = std::make_shared<SMBlob::EmbeddedWindows::SMBlobApp>(args);

    std::cout << std::endl;
    PressAnyKey( "Embed window press button: " );
    SMBlob::EmbeddedWindows::SMBlobWindow window;
    window.nativeWindowId = std::atoi(runner.getOutput().c_str());
    window.windowId = 0L;
    SMBlob::EmbeddedWindows::EmbedWindow(*embeddedWindows, window);

    std::cout << std::endl;
    PressAnyKey( "Press any button: " );

    if (embeddedWindows) {
        SMBlob::EmbeddedWindows::Release(*embeddedWindows);
    }
    return 0;
}