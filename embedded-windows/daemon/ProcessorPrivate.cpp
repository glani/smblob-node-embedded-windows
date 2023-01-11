#include "ProcessorPrivate.h"
#include "BaseProcessor.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedLog.h"
#include <plog/Log.h>
#include <plog/Init.h>


namespace SMBlob {
    namespace EmbeddedWindows {

        ProcessorPrivate::ProcessorPrivate(const ProcessorPrivateSetup &setup, BaseProcessor *processor) : params(
                setup), processor(processor), daemonConnected(false) {

        }

        ProcessorPrivate::~ProcessorPrivate() {
            uv_library_shutdown();
        }


        void ProcessorPrivate::start() {
            LOG_DEBUG << "Welcome to Processor!";
            this->initLog();

            stopSign = false;
            this->loop = uvw::Loop::getDefault();
            this->thread = loop->resource<uvw::Thread>(CC_CALLBACK_1(ProcessorPrivate::startThread, this));
            this->thread->run();

            std::unique_lock<std::mutex> lock(this->startMutex);
            this->startCondition.wait(lock, [this] { return this->startSign; });
            lock.unlock();
        }

        void ProcessorPrivate::wait() {
            std::unique_lock<std::mutex> lock(this->stopMutex);
            this->stopCondition.wait(lock, [this] { return this->stopSign; });
        }

        void ProcessorPrivate::stop() {
            if (loop) {
                asyncHandle->send();
            }

            if (this->thread) {
                this->thread->join();
                LOG_DEBUG << "uv thread join successfully";
                this->thread = nullptr;
            }
        }

        void ProcessorPrivate::closeLoop() {
            if (loop) {
                loop->close();
            }
        }

        void ProcessorPrivate::startThread(std::shared_ptr<void> data) {

            loop->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &t, uvw::Loop &loop) {
                int err = t.code();
                if (err == UV_EBUSY) {
                    loop.walk([](auto &handle) {
                        if (!handle.closing()) {
                            handle.close();
                        }
                    });
                }

                if (err < 0) {
                    if (err == UV_EBUSY) {
                        LOGE << "false positive";
                    }
                    LOGE << "loop failed: " << uv_err_name(err) << " str: " << uv_strerror(err);
                }
            });

            // idle
            this->idleHandle = loop->resource<uvw::IdleHandle>();
            this->idleHandle->on<uvw::IdleEvent>(CC_CALLBACK_2(ProcessorPrivate::onIdleCallback, this));
            this->idleHandle->start();
            // idle
            this->checkHandle = loop->resource<uvw::CheckHandle>();
            this->checkHandle->on<uvw::CheckEvent>(CC_CALLBACK_2(ProcessorPrivate::onCheckCallback, this));
            this->checkHandle->start();

            // pipe
            this->ipcClient = loop->resource<uvw::PipeHandle>();
            this->ipcClient->on<uvw::ErrorEvent>(CC_CALLBACK_2(ProcessorPrivate::onIpcClientErrorCallback, this));
            this->ipcClient->once<uvw::ConnectEvent>(CC_CALLBACK_2(ProcessorPrivate::onIpcClientConnectCallback, this));
            this->ipcClient->once<uvw::ShutdownEvent>(CC_CALLBACK_2(ProcessorPrivate::onIpcClientShutdownCallback, this));
            this->ipcClient->on<uvw::DataEvent>(CC_CALLBACK_2(ProcessorPrivate::onIpcClientDataCallback, this));
            this->ipcClient->on<uvw::WriteEvent>(CC_CALLBACK_2(ProcessorPrivate::onIpcClientWriteCallback, this));
            this->ipcClient->on<uvw::EndEvent>(CC_CALLBACK_2(ProcessorPrivate::onIpcClientEndCallback, this));



            this->ipcClient->connect(this->params.pipeName);
            LOGD << "Pipe name: " << this->params.pipeName;

            //async
            this->asyncHandle = loop->resource<uvw::AsyncHandle>();
            this->asyncHandle->on<uvw::AsyncEvent>(CC_CALLBACK_2(ProcessorPrivate::onAsyncCallback, this));

            //signal
            this->signalSigCloseHandle = loop->resource<uvw::SignalHandle>();
            this->signalSigCloseHandle->on<uvw::SignalEvent>(CC_CALLBACK_2(ProcessorPrivate::onSignalCallback, this));
            this->signalSigCloseHandle->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &t, uvw::SignalHandle &signal) {
            });

            this->signalSigCloseHandle->oneShot(SIGNAL_TERMINATE);

            LOG_DEBUG << "Before UV start";
            loop->run();
            loop = nullptr;
            // notify exit monitor
            std::unique_lock<std::mutex> ___e(this->stopMutex);
            this->stopSign = true;
            this->stopCondition.notify_one();
            ___e.unlock();
        }

        void ProcessorPrivate::onCheckCallback(const uvw::CheckEvent &evt, uvw::CheckHandle &check) {
            if (!firstCheck) {
                LOG_DEBUG << "UV First check occurred";
                firstCheck = true;
                std::unique_lock<std::mutex> ___s(this->startMutex);
                this->startSign = true;
                this->startCondition.notify_one();
                ___s.unlock();
            }

        }

        void ProcessorPrivate::onIdleCallback(const uvw::IdleEvent &evt, uvw::IdleHandle &idle) {

        }

        void ProcessorPrivate::onAsyncCallback(const uvw::AsyncEvent &evt, uvw::AsyncHandle &async) {
            LOG_DEBUG << "onAsyncCallback";
            LOG_DEBUG << "Close loop";
            closeLoop();
        }

        void ProcessorPrivate::onSignalCallback(const uvw::SignalEvent &evt, uvw::SignalHandle &signal) {
            LOG_DEBUG << "onSignalCallback: " << evt.signum;
            if (evt.signum == SIGNAL_TERMINATE) {
                LOG_DEBUG << "this->processor->requestExit";
                this->processor->requestExit();
                closeLoop();
            }
        }

        void ProcessorPrivate::initLog() {

            std::string fileName(params.logFileName);

            plog::Severity severity = __SEVERITY_D__(params.logSeverity);

            plog::init(severity);
            if (fileName.length() > 0) {
                this->fileAppender = std::make_shared<plog::RollingFileAppender<plog::TxtFormatter>>(fileName.c_str(),
                                                                                                     params.logMaxFilesize >
                                                                                                     0
                                                                                                     ? params.logMaxFilesize
                                                                                                     : 1024 * 1024 * 5,
                                                                                                     params.logMaxFiles >
                                                                                                     0
                                                                                                     ? params.logMaxFiles
                                                                                                     : 10);

                plog::get()->addAppender(this->fileAppender.get());
            }

            this->consoleAppender = std::make_shared<plog::ConsoleAppender<plog::TxtFormatter>>();
            plog::get()->addAppender(this->consoleAppender.get());

            LOG_DEBUG << "Log system initialized!";
        }

        void ProcessorPrivate::onIpcClientErrorCallback(const uvw::ErrorEvent &evt, uvw::PipeHandle &client) {
            LIBUV_ERR("onIpcClientErrorCallback: ")
        }

        void ProcessorPrivate::onIpcClientConnectCallback(const uvw::ConnectEvent &evt, uvw::PipeHandle &client) {
            LOG_DEBUG << "onIpcClientConnectCallback";
            this->daemonConnected = true;
            auto data = std::unique_ptr<char[]>(new char[3]{ 'a', 'b', 'c' });
            client.write(data.get(), 3);

        }

        void ProcessorPrivate::onIpcClientShutdownCallback(const uvw::ShutdownEvent &evt, uvw::PipeHandle &client) {
            LOG_DEBUG << "onIpcClientShutdownCallback";
            this->daemonConnected = false;
            client.close();
        }

        void ProcessorPrivate::onIpcClientEndCallback(const uvw::EndEvent &evt, uvw::PipeHandle &client) {
            LOG_DEBUG << "onIpcClientEndCallback";
            this->daemonConnected = false;
            client.close();
        }

        void ProcessorPrivate::onIpcClientDataCallback(const uvw::DataEvent &evt, uvw::PipeHandle &client) {
            LOG_DEBUG << "onIpcClientDataCallback";
        }

        void ProcessorPrivate::onIpcClientWriteCallback(const uvw::WriteEvent &evt, uvw::PipeHandle &client) {
            LOG_DEBUG << "onIpcClientWriteCallback";
        }

        // ProcessorPrivateSetup
        ProcessorPrivateSetup ProcessorPrivateSetup::fromArgs(const std::unique_ptr<char *[]> &args, size_t size) {
            ProcessorPrivateSetup res;


            return res;
        }
    }


}