#include "application.h"
#include "SMBlobNodeEmbeddedWindowsShared.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedPbModels.h"
#include "plog/Log.h"
#include "embedded-window.h"
#include <QThread>

#define FIND_BY_NATIVE_WINDOW(window) \
    std::find_if(std::begin(this->embeddedWindows), std::end(this->embeddedWindows), [&window](const EmbeddedWindowPtr& item) { \
        return window == item->getNativeWindow(); \
    })

namespace SMBlob {
    namespace EmbeddedWindows {
        Application::Application(int &argc, char **argv) : QApplication(argc, argv, ApplicationFlags),
                                                           SMBlob::EmbeddedWindows::BaseProcessor() {
            QObject::connect(&proxyObject, SIGNAL(exitRequested()), this, SLOT(exitRequested()));
            QObject::connect(&proxyObject, SIGNAL(embedWindowRequested(std::shared_ptr<SMBEWEmbedWindowReq>)), this,
                             SLOT(embedWindowRequested(std::shared_ptr<SMBEWEmbedWindowReq>)));
            QObject::connect(&proxyObject, SIGNAL(embeddedWindowDestroyed(const SMBEWEmbedWindow&)), this,
                             SLOT(embeddedWindowDestroyed(const SMBEWEmbedWindow&)));
            QObject::connect(&proxyObject, SIGNAL(embeddedWindowFocused(const SMBEWEmbedWindow &, bool)), this,
                             SLOT(embeddedWindowFocused(const SMBEWEmbedWindow &, bool)));
            QObject::connect(&proxyObject, SIGNAL(embeddedWindowSubscribed(const SMBEWEmbedWindow &, bool)), this,
                             SLOT(embeddedWindowSubscribed(const SMBEWEmbedWindow &, bool)));
            QObject::connect(&proxyObject, SIGNAL(embeddedWindowReparented(const SMBEWEmbedWindow &, int)), this,
                             SLOT(embeddedWindowReparented(const SMBEWEmbedWindow &, int)));
            QObject::connect(&proxyObject,
                             SIGNAL(embeddedWindowCustomOpaqueRequested(const SMBEWEmbedWindow &, FrameExtents, struct OpaqueParameters)),
                             this,
                             SLOT(embeddedWindowCustomOpaqueRequested(const SMBEWEmbedWindow &, FrameExtents, struct OpaqueParameters)));

            BaseProcessor::windowActor->setOnEmbeddedWindowDestroyedCallback(
                    SMBEW_CC_CALLBACK_1(Application::embeddedWindowDestroy, this));
            BaseProcessor::windowActor->setOnEmbeddedWindowFocusedCallback(
                    SMBEW_CC_CALLBACK_2(Application::embeddedWindowFocus, this));
            BaseProcessor::windowActor->setOnEmbeddedWindowSubscribedCallback(
                    SMBEW_CC_CALLBACK_2(Application::embeddedWindowSubscribe, this));
            BaseProcessor::windowActor->setOnEmbeddedWindowReparentedCallback(
                    SMBEW_CC_CALLBACK_2(Application::embeddedWindowReparent, this));
            BaseProcessor::windowActor->setOnEmbeddedWindowCustomOpaqueRequestedCallback(
                    SMBEW_CC_CALLBACK_3(Application::embeddedWindowCustomOpaqueRequest, this));
        }


        Application::~Application() {

        }


        void Application::releaseWindow(const SMBEWReleaseWindowReq &req) {

        }

        void Application::closeWindow(const SMBEWCloseWindowReq &req) {

        }

        // main thread logic
        void Application::exitRequested() {
            LOG_DEBUG << "Application::exitRequested";
            this->quit();
            this->closeAllWindows();
        }

        void Application::embedWindowRequested(std::shared_ptr<SMBEWEmbedWindowReq> request) {
            LOG_DEBUG << "Application::embedWindowRequested";
            Application::logSMBEWEmbedWindowReq(request);
            if (request) {
                EmbeddedWindow *pWindow = new EmbeddedWindow(*request, (SMBlob::EmbeddedWindows::BaseProcessor *) this);
                EmbeddedWindowPtr mainWindow(pWindow);
                this->embeddedWindows.emplace_back(std::move(mainWindow));
            }
        }

        // move to log utils
        void Application::logSMBEWEmbedWindowReq(std::shared_ptr<SMBEWEmbedWindowReq> &windowPtr) {
            if (windowPtr) {
                LOG_DEBUG << "SMBEWEmbedWindowReq with tag [ " << (windowPtr->has_tag() ? windowPtr->tag() : "NONE")
                          << " ]";
                if (windowPtr->has_window()) {
                    LOG_DEBUG << "window Id: " << windowPtr->window().windowid();
                    LOG_DEBUG << "native window Id: " << windowPtr->window().nativewindowid();
                }
            } else {
                LOG_DEBUG << "SMBEWEmbedWindowReq is null ";
            }
        }

        void Application::embeddedWindowDestroyed(const SMBEWEmbedWindow &window) {
            LOG_DEBUG << "Application::embeddedWindowDestroyed";
        }

        void Application::embeddedWindowFocused(const SMBEWEmbedWindow &window, bool focus) {
            LOG_DEBUG << "Application::embeddedWindowFocused";
        }

        void Application::embeddedWindowSubscribed(const SMBEWEmbedWindow &window, bool success) {
            auto result = FIND_BY_NATIVE_WINDOW(window);
            if (result != this->embeddedWindows.end()) {
                LOG_DEBUG << "Application::embeddedWindowSubscribed: " << window;
                result->get()->windowSubscribed(success);
            } else {
                LOG_WARNING << "Application::embeddedWindowSubscribed not found: " << window;
            }
        }

        void Application::embeddedWindowReparented(const SMBEWEmbedWindow &window, int mask) {
            auto result = FIND_BY_NATIVE_WINDOW(window);
            if (result != this->embeddedWindows.end()) {
                LOG_DEBUG << "Application::embeddedWindowReparented: " << window;
                result->get()->windowReparented(mask);
            } else {
                LOG_WARNING << "Application::embeddedWindowReparented not found: " << window;
            }
        }

        void Application::embeddedWindowCustomOpaqueRequested(const SMBEWEmbedWindow &window,
                                                              struct FrameExtents frameExtents,
                                                              struct OpaqueParameters opaqueParameters) {
            auto result = FIND_BY_NATIVE_WINDOW(window);
            if (result != this->embeddedWindows.end()) {
                LOG_DEBUG << "Application::embeddedWindowCustomOpaqueRequested: " << window;
                result->get()->customOpaqueRequested(frameExtents, opaqueParameters);
            } else {
                LOG_WARNING << "Application::embeddedWindowCustomOpaqueRequested not found: " << window;
            }
        }

        // ProxyObject
        ProxyObject::ProxyObject(QObject *parent) : QObject(parent) {

        }

    }
}
