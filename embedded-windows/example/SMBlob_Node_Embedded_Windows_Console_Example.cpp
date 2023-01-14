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
//    daemonExec = std::string(DAEMON_PATH);
//    daemonExec.append("").append(
//            "embedded-windows/daemon/smblob-node-embedded-windows-daemon"
//    );
//    daemonLog = std::string(DAEMON_PATH);
//    daemonLog.append("").append(
//            "log-smblob-node-embedded-windows-daemon.log"
//    );
#endif
    plog::init<SecondaryLog>(plog::Severity::debug);

    auto consoleAppender = std::make_shared<plog::ConsoleAppender<plog::TxtFormatter>>();
    plog::get<SecondaryLog>()->addAppender(consoleAppender.get());

    SMBlob::EmbeddedWindows::SMBlobAppInitConsumer params;
    params.daemonExec = daemonExec;
    params.debug = true;
    params.logDaemonFilename = daemonLog;

    SMBlob::EmbeddedWindows::SMBlobApp args = SMBlob::EmbeddedWindows::Init(params);
    auto embeddedWindows = std::make_shared<SMBlob::EmbeddedWindows::SMBlobApp>(args);

    std::cout << std::endl;
//    PressAnyKey( "Embed window press button: " );
//    std::string search("New Tab");
    std::string search("Mozilla Firefox");
    while (true) {
        // std::cin.ignore();
        auto c = std::cin.get();
//        std::cout << c << std::endl;
        if (c == (int) 'f') {
            break;
        } else if (c == (int) 'p') {
            std::string in;
            std::cin.ignore();
            if (std::getline(std::cin, in)) {
                search = in;
            }
            PLOGD_(SecondaryLog) << "new search pattern: " << search;
        } else if (c == (int) 'n') {
            auto runnerArgvPtr = std::unique_ptr<char *[]>(
                    new char *[3]{"search", "--name", (char*)search.c_str()}
            );
            std::string command("/usr/bin/xdotool");
            ProcessRunner runner;

            runner.RunCommand(command, runnerArgvPtr.get(), 3);

            PLOGD_(SecondaryLog) << "window Id:" << runner.getOutput();
            SMBlob::EmbeddedWindows::SMBlobWindow window;
            window.nativeWindowId = std::atoi(runner.getOutput().c_str());
            window.windowId = 0L;
            if (window.nativeWindowId > 0) {
                SMBlob::EmbeddedWindows::EmbedWindow(*embeddedWindows, window);
            } else {
                LOGE << "No Window found";
            }
        }
    }


    if (embeddedWindows) {
        SMBlob::EmbeddedWindows::Release(*embeddedWindows);
    }
    return 0;
}