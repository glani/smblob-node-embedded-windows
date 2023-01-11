#pragma once

#include <memory>
#include <condition_variable>
#include "uvw.hpp"
#include "SMBlobNodeEmbeddedWindows.h"
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Appenders/ConsoleAppender.h>


namespace SMBlob {
    namespace EmbeddedWindows {
        struct SMBlobAppInitConsumer;

        class ConsumerPrivate {
        public:
            explicit ConsumerPrivate();
            virtual ~ConsumerPrivate();


            void start(const SMBlobAppInitConsumer &params);
            void wait();
            void close();

            void initLog(const SMBlobAppInitConsumer &params);

        private:
            void closeLoop();
            void startThread(std::shared_ptr<void> data);
            void onIdleCallback(const uvw::IdleEvent& evt, uvw::IdleHandle& idle);
            void onCheckCallback(const uvw::CheckEvent& evt, uvw::CheckHandle& check);
            void onAsyncCallback(const uvw::AsyncEvent& evt, uvw::AsyncHandle& async);
            void onSignalCallback(const uvw::CheckEvent& evt, uvw::SignalHandle& signal);


            void onProcessErrorCallback(const uvw::ErrorEvent & evt, uvw::ProcessHandle & process);

        private:
            std::string daemonExec;
            int logSeverity;
            bool firstCheck;
            bool startSign;
            std::mutex startMutex;
            std::condition_variable startCondition;

            bool stopSign;
            std::mutex endMutex;
            std::condition_variable endCondition;


            std::shared_ptr<uvw::Loop> loop;
            std::shared_ptr<uvw::Thread> thread;
            std::shared_ptr<uvw::IdleHandle> idleHandle;
            std::shared_ptr<uvw::AsyncHandle> asyncHandle;
            std::shared_ptr<uvw::SignalHandle> signalSigCloseHandle;
            std::shared_ptr<uvw::CheckHandle> checkHandle;
            std::shared_ptr<uvw::ProcessHandle> processHandle;

            std::shared_ptr<plog::RollingFileAppender<plog::TxtFormatter>> fileAppender;
            std::shared_ptr<plog::ConsoleAppender<plog::TxtFormatter>> consoleAppender;
            std::string logSeverityParam;
            std::string logSeverityStr;
            std::string logDaemonFilename;
            std::unique_ptr<char*[]> daemonArgv;
            std::string logDaemonFilenameParam;
        };
    }
}
