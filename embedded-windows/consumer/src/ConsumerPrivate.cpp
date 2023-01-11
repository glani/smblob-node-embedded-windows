#include <mutex>
#include "ConsumerPrivate.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedLog.h"
#include <plog/Log.h>
#include <plog/Init.h>


#define SIGNAL_TERMINATE SIGTERM


namespace SMBlob {
    namespace EmbeddedWindows {

        ConsumerPrivate::ConsumerPrivate() : startSign(false), stopSign(true), firstCheck(false) {

        }

        ConsumerPrivate::~ConsumerPrivate() {

        }

        void ConsumerPrivate::start(const SMBlobAppInitConsumer &params) {
            LOG_DEBUG << "Welcome to Consumer!";


            this->daemonExec = params.daemonExec;
            this->logSeverity = params.logSeverity;

            stopSign = false;
            this->loop = uvw::Loop::getDefault();
            this->thread = loop->resource<uvw::Thread>(CC_CALLBACK_1(ConsumerPrivate::startThread, this));
            this->thread->run();

            std::unique_lock<std::mutex> lock(this->startMutex);
            this->startCondition.wait(lock, [this] { return this->startSign; });
            lock.unlock();
        }

        void ConsumerPrivate::wait() {
            std::unique_lock<std::mutex> lock(this->endMutex);
            this->endCondition.wait(lock, [this] { return this->stopSign; });
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

            //async
            this->asyncHandle = loop->resource<uvw::AsyncHandle>();
            this->asyncHandle->on<uvw::AsyncEvent>(CC_CALLBACK_2(ConsumerPrivate::onAsyncCallback, this));

            // spawner
            this->processHandle = loop->resource<uvw::ProcessHandle>();
//            this->spawnerHandle->on<uvw::ListenEvent>(CC_CALLBACK_2(ConsumerPrivate::onSpawnerListenCallback, this));
            this->processHandle->on<uvw::ErrorEvent>(CC_CALLBACK_2(ConsumerPrivate::onProcessErrorCallback, this));


            //signal
            this->signalSigCloseHandle = loop->resource<uvw::SignalHandle>();
            this->signalSigCloseHandle->on<uvw::CheckEvent>(CC_CALLBACK_2(ConsumerPrivate::onSignalCallback, this));
            this->signalSigCloseHandle->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &t, uvw::SignalHandle &signal) {
            });

            this->signalSigCloseHandle->oneShot(SIGNAL_TERMINATE);

            LOG_DEBUG << "Before UV start";
            loop->run();
            loop = nullptr;
            // notify exit monitor
            std::unique_lock<std::mutex> ___e(this->endMutex);
            this->stopSign = true;
            this->endCondition.notify_one();
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
                    daemonArgv = std::move(std::unique_ptr<char*[]>(new char*[111]));
                    int i = 0;
                    daemonArgv[i] = (char*)this->daemonExec.c_str();
                    i++;
                    this->logSeverityParam = std::string("--log");
                    daemonArgv[i] = (char*)this->logSeverityParam.c_str();
                    i++;
                    this->logSeverityStr = std::to_string(this->logSeverity);
                    std::string severity = logSeverityStr.c_str();
                    daemonArgv[i] = (char*)severity.c_str();
                    i++;
                    if (logDaemonFilename.length() > 0) {
                        this->logDaemonFilenameParam = std::string("--logFile");
                        daemonArgv[i] = (char*)this->logDaemonFilenameParam.c_str();
                        i++;
                        daemonArgv[i] = (char*)this->logDaemonFilename.c_str();
                        i++;
                    }

                    daemonArgv[i] = nullptr;


                    LOG_DEBUG << "UV try to spawn a daemon";
                    this->processHandle->spawn(
                            (char*)this->daemonExec.c_str(),
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
                this->processHandle->close();
            }
            LOG_DEBUG << "Close loop";
            closeLoop();
        }

        void ConsumerPrivate::onSignalCallback(const uvw::CheckEvent &evt, uvw::SignalHandle &signal) {

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

        void ConsumerPrivate::onProcessErrorCallback(const uvw::ErrorEvent &evt, uvw::ProcessHandle &process) {
            auto err = evt.code();
            if (err < 0) {
                LOGE << "onProcessErrorCallback: " << uv_err_name(err) << " str: " << uv_strerror(err);
            }
        }

    }
}