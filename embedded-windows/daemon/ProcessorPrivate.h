#pragma once

#include <string>
#include <memory>
#include <condition_variable>

#include "uvw.hpp"
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <queue>

namespace SMBlob {
    namespace EmbeddedWindows {
        class BaseProcessor;

        struct ProcessorPrivateSetup {

            static ProcessorPrivateSetup fromArgs(int argc, char *argv[]);

            std::string pipeName;
            std::string logFileName;
            bool debug;
            int logSeverity;
            int logMaxFilesize;
            int logMaxFiles;

            ProcessorPrivateSetup() :
                    pipeName(""), logFileName(""), debug(false), logSeverity(-1), logMaxFilesize(-1),  logMaxFiles(-1)
            {

            }
        };

        class ProcessorPrivate {
        public:
            explicit ProcessorPrivate(const ProcessorPrivateSetup &setup, BaseProcessor* processor);

            virtual ~ProcessorPrivate();

            void start();
            void wait();
            void stop();

            void enqueueRequest(std::unique_ptr<char[]>& data, size_t size);

            void initApplication();

        private:
            void initLog();

            void closeLoop();
            void startThread(std::shared_ptr<void> data);
            void onIdleCallback(const uvw::IdleEvent& evt, uvw::IdleHandle& idle);
            void onCheckCallback(const uvw::CheckEvent& evt, uvw::CheckHandle& check);
            void onAsyncCallback(const uvw::AsyncEvent& evt, uvw::AsyncHandle& async);
            void onSignalCallback(const uvw::SignalEvent& evt, uvw::SignalHandle& signal);

            void onIpcClientErrorCallback(const uvw::ErrorEvent & evt, uvw::PipeHandle & client);
            void onIpcClientConnectCallback(const uvw::ConnectEvent & evt, uvw::PipeHandle & client);
            void onIpcClientShutdownCallback(const uvw::ShutdownEvent &evt, uvw::PipeHandle &client);

            void onIpcClientDataCallback(const uvw::DataEvent & evt, uvw::PipeHandle & client);
            void onIpcClientWriteCallback(const uvw::WriteEvent & evt, uvw::PipeHandle & client);
            void onIpcClientEndCallback(const uvw::EndEvent & evt, uvw::PipeHandle & client);




        private:
            ProcessorPrivateSetup params;
            BaseProcessor* processor;
            // uv
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
            std::shared_ptr<uvw::PipeHandle> ipcClient;

            std::shared_ptr<uvw::CheckHandle> checkHandle;
            std::shared_ptr<plog::RollingFileAppender<plog::TxtFormatter>> fileAppender;
            std::shared_ptr<plog::ConsoleAppender<plog::TxtFormatter>> consoleAppender;
            bool daemonConnected;
            std::mutex requestQueueMutex;
            std::queue<struct RequestDataHolder> requestQueue;

        };
    }
}