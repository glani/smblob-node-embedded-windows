#include "ProcessorPrivate.h"
#include "BaseProcessor.h"
#include "SMBlobNodeEmbeddedWindowsShared.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedLog.h"
#include <plog/Log.h>
#include <plog/Init.h>


namespace SMBlob {
    namespace EmbeddedWindows {

        ProcessorPrivate::ProcessorPrivate(const ProcessorPrivateSetup &setup, BaseProcessor *processor) : params(
                setup), processor(processor), daemonConnected(false) {
            this->processor->processorPrivate = this;
        }

        ProcessorPrivate::~ProcessorPrivate() {
            uv_library_shutdown();
        }


        void ProcessorPrivate::start() {
            LOG_DEBUG << "Welcome to Processor!";
            this->initLog();

            stopSign = false;
            this->loop = uvw::Loop::getDefault();
            this->thread = loop->resource<uvw::Thread>(SMBEW_CC_CALLBACK_1(ProcessorPrivate::startThread, this));
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
            this->idleHandle->on<uvw::IdleEvent>(SMBEW_CC_CALLBACK_2(ProcessorPrivate::onIdleCallback, this));
            this->idleHandle->start();
            // idle
            this->checkHandle = loop->resource<uvw::CheckHandle>();
            this->checkHandle->on<uvw::CheckEvent>(SMBEW_CC_CALLBACK_2(ProcessorPrivate::onCheckCallback, this));
            this->checkHandle->start();

            // pipe
            this->ipcClient = loop->resource<uvw::PipeHandle>();
            this->ipcClient->on<uvw::ErrorEvent>(SMBEW_CC_CALLBACK_2(ProcessorPrivate::onIpcClientErrorCallback, this));
            this->ipcClient->once<uvw::ConnectEvent>(
                    SMBEW_CC_CALLBACK_2(ProcessorPrivate::onIpcClientConnectCallback, this));
            this->ipcClient->once<uvw::ShutdownEvent>(
                    SMBEW_CC_CALLBACK_2(ProcessorPrivate::onIpcClientShutdownCallback, this));
            this->ipcClient->on<uvw::DataEvent>(SMBEW_CC_CALLBACK_2(ProcessorPrivate::onIpcClientDataCallback, this));
            this->ipcClient->on<uvw::WriteEvent>(SMBEW_CC_CALLBACK_2(ProcessorPrivate::onIpcClientWriteCallback, this));
            this->ipcClient->on<uvw::EndEvent>(SMBEW_CC_CALLBACK_2(ProcessorPrivate::onIpcClientEndCallback, this));


            this->ipcClient->connect(this->params.pipeName);
            LOGD << "Pipe name: " << this->params.pipeName;

            //async
            this->asyncHandle = loop->resource<uvw::AsyncHandle>();
            this->asyncHandle->on<uvw::AsyncEvent>(SMBEW_CC_CALLBACK_2(ProcessorPrivate::onAsyncCallback, this));

            //signal
            this->signalSigCloseHandle = loop->resource<uvw::SignalHandle>();
            this->signalSigCloseHandle->on<uvw::SignalEvent>(
                    SMBEW_CC_CALLBACK_2(ProcessorPrivate::onSignalCallback, this));
            this->signalSigCloseHandle->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &t, uvw::SignalHandle &signal) {
            });

            this->signalSigCloseHandle->oneShot(SMBEW_SIGNAL_TERMINATE);

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

            std::vector<struct IODataHolder> internalVector;
            internalVector.reserve(10);
            std::unique_lock<std::mutex> lk(requestQueueMutex);
            if (requestQueue.size() > 0) {
                internalVector.reserve(requestQueue.size());
            }
            while (!requestQueue.empty()) {
                internalVector.push_back(std::move(requestQueue.front()));
                requestQueue.pop();
            }
            lk.unlock();

            auto it = internalVector.begin();
            for (; it != internalVector.end(); ++it) {

                if (this->ipcClient && this->daemonConnected) {
                    this->ipcClient->write(std::move(it->data), it->size);
                }
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
            if (evt.signum == SMBEW_SIGNAL_TERMINATE) {
                LOG_DEBUG << "this->processor->requestExit";
                this->processor->requestExit();
                closeLoop();
            }
        }

        void ProcessorPrivate::initLog() {

            const auto& fileName = params.logFileName;

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
            SMBEW_LIBUV_ERR("onIpcClientErrorCallback: ")
        }

        void ProcessorPrivate::onIpcClientConnectCallback(const uvw::ConnectEvent &evt, uvw::PipeHandle &client) {
            LOG_DEBUG << "onIpcClientConnectCallback";
            client.read();
            this->daemonConnected = true;
            this->processor->connectApplication();
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
            struct IODataHolder req;
            std::unique_ptr<char[]> ptr(new char[evt.length]);
            memcpy(ptr.get(), evt.data.get(), evt.length);
            req.data = std::move(ptr);
            req.size = evt.length;
            this->processor->request(std::move(req));
        }

        void ProcessorPrivate::onIpcClientWriteCallback(const uvw::WriteEvent &evt, uvw::PipeHandle &client) {
            LOG_DEBUG << "onIpcClientWriteCallback";
        }

        void ProcessorPrivate::enqueueRequest(std::unique_ptr<char[]> &data, size_t size) {
            std::unique_lock<std::mutex> lk(requestQueueMutex);
            struct IODataHolder req;
            req.data = std::move(data);
            req.size = size;
            requestQueue.emplace(std::move(req));
        }

        void ProcessorPrivate::initApplication() {
            this->processor->initApplication();
        }

        // ProcessorPrivateSetup
        ProcessorPrivateSetup ProcessorPrivateSetup::fromArgs(int argc, char *argv[]) {
            ProcessorPrivateSetup res;

            const auto &logSeverityParam = SMBEW_S_LOG;
            const auto &logFile = SMBEW_S_LOG_FILE;
            const auto &debugParam = SMBEW_S_DEBUG;
            const auto &pipeNameParam = SMBEW_S_PIPE_NAME;
            int i = 0;
            while (i < argc) {
                if (i != 0) { // skip exec path
                    const auto &argValue = std::string(argv[i]);
                    if (argValue == logSeverityParam) {
                        if (i + 1 < argc) {
                            i++;
                            res.logSeverity = std::atoi(argv[i]);
                        }
                    } else if (argValue == debugParam) {
                        if (i + 1 < argc) {
                            i++;
                            res.debug = std::atoi(argv[i]) > 0;
                        }
                    } else if (argValue == pipeNameParam) {
                        if (i + 1 < argc) {
                            i++;
                            res.pipeName = argv[i];
                        }
                    } else if (argValue == logFile) {
                        if (i + 1 < argc) {
                            i++;
                            res.logFileName = argv[i];
                        }
                    }
                }
                i++;
            }
            return res;
        }

    }


}