#pragma once

#include <memory>
#include <condition_variable>
#include "uvw.hpp"
#include "SMBlobNodeEmbeddedWindows.h"
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <queue>


namespace SMBlob {
    namespace EmbeddedWindows {
        struct SMBlobAppInitConsumer;
        struct RequestDataHolder;

        class ConsumerPrivate {
        public:
            explicit ConsumerPrivate();
            virtual ~ConsumerPrivate();

            void start(const SMBlobAppInitConsumer &params);
            void wait();
            void close();


            void enqueueRequest(std::unique_ptr<char[]>& data, size_t size);

        private:
            void initLog(const SMBlobAppInitConsumer &params);

            void closeLoop();
            void startThread(std::shared_ptr<void> data);
            void onIdleCallback(const uvw::IdleEvent& evt, uvw::IdleHandle& idle);
            void onCheckCallback(const uvw::CheckEvent& evt, uvw::CheckHandle& check);
            void onAsyncCallback(const uvw::AsyncEvent& evt, uvw::AsyncHandle& async);
            void onSignalCallback(const uvw::SignalEvent& evt, uvw::SignalHandle& signal);

            void onIpcServerListenCallback(const uvw::ListenEvent evt, uvw::PipeHandle& server);
            void onIpcServerErrorCallback(const uvw::ErrorEvent & evt, uvw::PipeHandle & server);


            // client callbacks
            void onIpcClientErrorCallback(const uvw::ErrorEvent & evt, uvw::PipeHandle & client);
            void onIpcClientCloseCallback(const uvw::CloseEvent & evt, uvw::PipeHandle & client);
            void onIpcClientEndCallback(const uvw::EndEvent & evt, uvw::PipeHandle & client);
            void onIpcClientDataCallback(const uvw::DataEvent & evt, uvw::PipeHandle & client);
            void onIpcClientWriteCallback(const uvw::WriteEvent & evt, uvw::PipeHandle & client);
            void onIpcClientShutdownCallback(const uvw::ShutdownEvent & evt, uvw::PipeHandle & client);
            void onProcessErrorCallback(const uvw::ErrorEvent & evt, uvw::ProcessHandle & process);


            void onProcessExitEventCallback(const uvw::ExitEvent & evt, uvw::ProcessHandle & process);

            void onProcessStdOutDataCallback(const uvw::DataEvent & evt, uvw::PipeHandle & client);
            void onProcessStdErrDataCallback(const uvw::DataEvent & evt, uvw::PipeHandle & client);

        private:
            bool firstCheck;
            bool startSign;
            std::mutex startMutex;
            std::condition_variable startCondition;

            bool stopSign;
            std::mutex stopMutex;
            std::condition_variable stopCondition;


            std::shared_ptr<uvw::Loop> loop;
            std::shared_ptr<uvw::Thread> thread;
            std::shared_ptr<uvw::IdleHandle> idleHandle;
            std::shared_ptr<uvw::AsyncHandle> asyncHandle;
            std::shared_ptr<uvw::SignalHandle> signalSigCloseHandle;
            std::shared_ptr<uvw::CheckHandle> checkHandle;
            std::shared_ptr<uvw::ProcessHandle> processHandle;
            std::shared_ptr<uvw::PipeHandle> ipcServer;
            std::shared_ptr<uvw::PipeHandle> ipcClient;

            std::shared_ptr<plog::RollingFileAppender<plog::TxtFormatter>> fileAppender;
            std::shared_ptr<plog::ConsoleAppender<plog::TxtFormatter>> consoleAppender;

            std::string daemonExec;
            int logSeverity;
            std::string logSeverityParam;
            std::string logSeverityStr;
            std::string logDaemonFilename;
            std::unique_ptr<char*[]> daemonArgv;
            std::string logDaemonFilenameParam;
            std::string debugParam;
            std::string debugStr;
            std::string pipeNameParam;
            std::string pipeName;
            bool debug;
            std::shared_ptr<uvw::PipeHandle> pipeStdout;
            std::shared_ptr<uvw::PipeHandle> pipeStderr;

            std::mutex requestQueueMutex;
            std::queue<struct RequestDataHolder> requestQueue;

        };
    }
}
