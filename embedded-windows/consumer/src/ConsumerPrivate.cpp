#include <mutex>
#include "ConsumerPrivate.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedLog.h"
#include <plog/Log.h>
#include <plog/Init.h>


namespace SMBlob {
    namespace EmbeddedWindows {

        ConsumerPrivate::ConsumerPrivate() : startSign(false), stopSign(true), firstCheck(false) {
            std::srand(unsigned(std::time(0)));
#ifdef _DEBUG
            this->pipeName = std::string(PIPE_NAME);
#else
            this->pipeName = std::string(PIPE_NAME) + std::to_string(std::rand());
#endif
        }

        ConsumerPrivate::~ConsumerPrivate() {
            uv_library_shutdown();
        }

        void ConsumerPrivate::start(const SMBlobAppInitConsumer &params) {
            LOG_DEBUG << "Welcome to Consumer!";


            this->daemonExec = params.daemonExec;
            this->logSeverity = params.logSeverity;
            this->debug = params.debug;
            this->initLog(params);

            stopSign = false;
            this->loop = uvw::Loop::getDefault();
            this->thread = loop->resource<uvw::Thread>(CC_CALLBACK_1(ConsumerPrivate::startThread, this));
            this->thread->run();

            std::unique_lock<std::mutex> lock(this->startMutex);
            this->startCondition.wait(lock, [this] { return this->startSign; });
            lock.unlock();
        }

        void ConsumerPrivate::wait() {
            std::unique_lock<std::mutex> lock(this->stopMutex);
            this->stopCondition.wait(lock, [this] { return this->stopSign; });
        }

        void ConsumerPrivate::close() {
            if (loop) {
                asyncHandle->send();
            }

            if (this->thread) {
                this->thread->join();
                LOG_DEBUG << "uv thread join successfully";
                this->thread = nullptr;
            }
        }

        void ConsumerPrivate::closeLoop() {
            if (loop) {
                loop->close();
            }
        }

        void ConsumerPrivate::startThread(std::shared_ptr<void> data) {

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
            this->idleHandle->on<uvw::IdleEvent>(CC_CALLBACK_2(ConsumerPrivate::onIdleCallback, this));
            this->idleHandle->start();
            // idle
            this->checkHandle = loop->resource<uvw::CheckHandle>();
            this->checkHandle->on<uvw::CheckEvent>(CC_CALLBACK_2(ConsumerPrivate::onCheckCallback, this));
            this->checkHandle->start();

            // pipe
            this->ipcServer = loop->resource<uvw::PipeHandle>();
            this->ipcServer->on<uvw::ListenEvent>(CC_CALLBACK_2(ConsumerPrivate::onIpcServerListenCallback, this));
            this->ipcServer->on<uvw::ErrorEvent>(CC_CALLBACK_2(ConsumerPrivate::onIpcServerErrorCallback, this));


            auto fsReq = loop->resource<uvw::FsReq>();

            fsReq->on<uvw::FsEvent<uvw::FsReq::Type::UNLINK>>([&](const auto &, auto &) {
                LOGW << "unlink succeed";
            });

            fsReq->on<uvw::ErrorEvent>([](const auto &evt, auto &handle) {
                int err = evt.code();
                LOGI << "unlink failed: " << uv_err_name(err) << " str: " << uv_strerror(err);
            });

            fsReq->unlink(this->pipeName);
            LOGD << "Pipe name: " << this->pipeName;

            this->ipcServer->bind(this->pipeName);
            this->ipcServer->listen();


            //async
            this->asyncHandle = loop->resource<uvw::AsyncHandle>();
            this->asyncHandle->on<uvw::AsyncEvent>(CC_CALLBACK_2(ConsumerPrivate::onAsyncCallback, this));

            // spawner
            this->processHandle = loop->resource<uvw::ProcessHandle>();
            this->processHandle->on<uvw::ExitEvent>(CC_CALLBACK_2(ConsumerPrivate::onProcessExitEventCallback, this));
            this->processHandle->on<uvw::ErrorEvent>(CC_CALLBACK_2(ConsumerPrivate::onProcessErrorCallback, this));


            //signal
            this->signalSigCloseHandle = loop->resource<uvw::SignalHandle>();
            this->signalSigCloseHandle->on<uvw::SignalEvent>(CC_CALLBACK_2(ConsumerPrivate::onSignalCallback, this));
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

        void ConsumerPrivate::onCheckCallback(const uvw::CheckEvent &evt, uvw::CheckHandle &check) {
            if (!firstCheck) {
                LOG_DEBUG << "UV First check occurred";
                firstCheck = true;
                std::unique_lock<std::mutex> ___s(this->startMutex);
                this->startSign = true;
                this->startCondition.notify_one();
                ___s.unlock();

                if (this->daemonExec.length() > 0) {
                    daemonArgv = std::unique_ptr<char *[]>(new char *[111]);
                    int i = 0;
                    // 0
                    daemonArgv[i] = (char *) this->daemonExec.c_str();
                    i++;
                    this->logSeverityParam = S_LOG;
                    daemonArgv[i] = (char *) this->logSeverityParam.c_str();
                    i++;
                    // 1
                    this->logSeverityStr = std::to_string(this->logSeverity);
                    std::string severity = logSeverityStr.c_str();
                    daemonArgv[i] = (char *) severity.c_str();
                    i++;
                    // 2
                    this->debugParam = S_DEBUG;
                    daemonArgv[i] = (char *) this->debugParam.c_str();
                    i++;
                    this->debugStr = std::to_string(this->debug ? 1 : 0);
                    daemonArgv[i] = (char *) debugStr.c_str();
                    i++;
                    // 3
                    this->pipeNameParam = S_PIPE_NAME;
                    daemonArgv[i] = (char *) this->pipeNameParam.c_str();
                    i++;
                    daemonArgv[i] = (char *) this->pipeName.c_str();
                    i++;

                    if (logDaemonFilename.length() > 0) {
                        this->logDaemonFilenameParam = S_LOG_FILE;
                        daemonArgv[i] = (char *) this->logDaemonFilenameParam.c_str();
                        i++;
                        daemonArgv[i] = (char *) this->logDaemonFilename.c_str();
                        i++;
                    }

                    daemonArgv[i] = nullptr;


                    this->pipeStdout = loop->resource<uvw::PipeHandle>();
                    this->pipeStderr = loop->resource<uvw::PipeHandle>();

                    pipeStdout->on<uvw::DataEvent>(
                            CC_CALLBACK_2(ConsumerPrivate::onProcessStdOutDataCallback, this)
                    );
                    pipeStderr->on<uvw::DataEvent>(
                            CC_CALLBACK_2(ConsumerPrivate::onProcessStdErrDataCallback, this)
                    );
                    processHandle->stdio(uvw::StdIN, uvw::ProcessHandle::StdIO::IGNORE_STREAM);
                    processHandle->stdio(*pipeStdout,
                                         uvw::Flags<uvw::ProcessHandle::StdIO>::from<uvw::ProcessHandle::StdIO::CREATE_PIPE, uvw::ProcessHandle::StdIO::WRITABLE_PIPE>());
                    processHandle->stdio(*pipeStderr,
                                         uvw::Flags<uvw::ProcessHandle::StdIO>::from<uvw::ProcessHandle::StdIO::CREATE_PIPE, uvw::ProcessHandle::StdIO::WRITABLE_PIPE>());


                    pipeStdout->on<uvw::ErrorEvent>([&](const auto &evt, auto &) {
                        LIBUV_ERR("pipeStdout:")
                    });

                    pipeStderr->on<uvw::ErrorEvent>([&](const auto &evt, auto &) {
                        LIBUV_ERR("pipeStdout:")
                    });

                    LOG_DEBUG << "UV try to spawn a daemon";
                    this->processHandle->spawn(
                            (char *) this->daemonExec.c_str(),
                            daemonArgv.get());
                }
            }

        }

        void ConsumerPrivate::onIdleCallback(const uvw::IdleEvent &evt, uvw::IdleHandle &idle) {

        }

        void ConsumerPrivate::onAsyncCallback(const uvw::AsyncEvent &evt, uvw::AsyncHandle &async) {
            LOG_DEBUG << "onAsyncCallback";
            if (this->processHandle->pid() > 0) {
                LOG_DEBUG << "Close daemon process";
                // linux or macos
                this->processHandle->kill(SIGNAL_TERMINATE);
            }
            LOG_DEBUG << "Close loop";
            closeLoop();
        }

        void ConsumerPrivate::onSignalCallback(const uvw::SignalEvent &evt, uvw::SignalHandle &signal) {
            LOG_DEBUG << "onSignalCallback: " << evt.signum;
            if (evt.signum == SIGNAL_TERMINATE) {
                closeLoop();
            }
        }

        void ConsumerPrivate::initLog(const SMBlobAppInitConsumer &params) {

            std::string fileName(params.logFileName);
            this->logDaemonFilename = params.logDaemonFilename;

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


        void ConsumerPrivate::onProcessExitEventCallback(const uvw::ExitEvent &evt, uvw::ProcessHandle &process) {
            LOG_DEBUG << "onProcessExitEventCallback";
        }

        void ConsumerPrivate::onProcessErrorCallback(const uvw::ErrorEvent &evt, uvw::ProcessHandle &process) {
            LIBUV_ERR("onProcessErrorCallback: ")
        }

        void ConsumerPrivate::onIpcServerListenCallback(const uvw::ListenEvent evt, uvw::PipeHandle &server) {
            LOG_DEBUG << "onIpcServerListenCallback";
            // for now we support only one demon
            ipcClient = server.loop().resource<uvw::PipeHandle>();

            ipcClient->on<uvw::ErrorEvent>(CC_CALLBACK_2(ConsumerPrivate::onIpcClientErrorCallback, this));
            ipcClient->on<uvw::CloseEvent>(CC_CALLBACK_2(ConsumerPrivate::onIpcClientCloseCallback, this));
            ipcClient->on<uvw::DataEvent>(CC_CALLBACK_2(ConsumerPrivate::onIpcClientDataCallback, this));
            ipcClient->on<uvw::WriteEvent>(CC_CALLBACK_2(ConsumerPrivate::onIpcClientWriteCallback, this));
            ipcClient->on<uvw::EndEvent>(CC_CALLBACK_2(ConsumerPrivate::onIpcClientEndCallback, this));
            ipcClient->on<uvw::ShutdownEvent>(CC_CALLBACK_2(ConsumerPrivate::onIpcClientShutdownCallback, this));

            server.accept(*ipcClient);
            ipcClient->read();
        }


        void ConsumerPrivate::onIpcServerErrorCallback(const uvw::ErrorEvent &evt, uvw::PipeHandle &server) {
            LIBUV_ERR("onIpcServerErrorCallback:")
        }

        void ConsumerPrivate::onIpcClientErrorCallback(const uvw::ErrorEvent &evt, uvw::PipeHandle &client) {
            LIBUV_ERR("onIpcClientErrorCallback:")
        }

        void ConsumerPrivate::onIpcClientCloseCallback(const uvw::CloseEvent &evt, uvw::PipeHandle &client) {
            LOG_DEBUG << "onIpcClientCloseCallback:";
            this->ipcServer->close();
        }

        void ConsumerPrivate::onIpcClientEndCallback(const uvw::EndEvent &evt, uvw::PipeHandle &client) {
            LOG_DEBUG << "onIpcClientEndCallback:";
            client.close();
        }

        void ConsumerPrivate::onIpcClientDataCallback(const uvw::DataEvent &evt, uvw::PipeHandle &client) {
            LOG_DEBUG << "onIpcClientDataCallback:";
        }

        void ConsumerPrivate::onIpcClientWriteCallback(const uvw::WriteEvent &evt, uvw::PipeHandle &client) {
            LOG_DEBUG << "onIpcClientWriteCallback:";
        }

        void ConsumerPrivate::onIpcClientShutdownCallback(const uvw::ShutdownEvent &evt, uvw::PipeHandle &client) {
            LOG_DEBUG << "onClientShutdownCallback:";
        }

        void ConsumerPrivate::onProcessStdOutDataCallback(const uvw::DataEvent &evt, uvw::PipeHandle &client) {
            LOGD << evt.data.get();
        }

        void ConsumerPrivate::onProcessStdErrDataCallback(const uvw::DataEvent &evt, uvw::PipeHandle &client) {
            LOGE << evt.data.get();
        }


    }
}