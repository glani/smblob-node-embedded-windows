#include "application.h"
#include "SMBlobNodeEmbeddedWindowsShared.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedPbModels.h"
#include "plog/Log.h"
#include "embedded-window.h"
#include <QThread>


namespace SMBlob {
    namespace EmbeddedWindows {
        Application::Application(int &argc, char **argv) : QApplication(argc, argv, ApplicationFlags),
                                                           SMBlob::EmbeddedWindows::BaseProcessor() {
            QObject::connect(&proxyObject, SIGNAL(exitRequested()), this, SLOT(exitRequested()));
            QObject::connect(&proxyObject, SIGNAL(embedWindowRequested(std::shared_ptr<SMBEWEmbedWindowReq>)), this, SLOT(embedWindowRequested(std::shared_ptr<SMBEWEmbedWindowReq>)));
        }


        Application::~Application() {

        }

        void Application::requestExit() {
            emit proxyObject.exitRequested();
        }


        void Application::releaseWindow(const SMBEWReleaseWindowReq &req) {

        }

        void Application::closeWindow(const SMBEWCloseWindowReq &req) {

        }

        void Application::embedWindow(const SMBEWEmbedWindowReq &req) {
            std::shared_ptr<SMBEWEmbedWindowReq> request = std::make_shared<SMBEWEmbedWindowReq>(req);
            emit proxyObject.embedWindowRequested(std::move(request));
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
                WidgetPtr mainWindow(new EmbeddedWindow(*request, (SMBlob::EmbeddedWindows::BaseProcessor*)this));
                this->embeddedWindows.emplace_back(std::move(mainWindow));
            }
        }

        // move to log utils
        void Application::logSMBEWEmbedWindowReq(std::shared_ptr<SMBEWEmbedWindowReq>& windowPtr) {
            if (windowPtr) {
                LOG_DEBUG << "SMBEWEmbedWindowReq with tag [ " << (windowPtr->has_tag() ? windowPtr->tag() : "NONE") << " ]";
                if (windowPtr->has_window()) {
                    LOG_DEBUG << "window Id: " << windowPtr->window().windowid();
                    LOG_DEBUG << "native window Id: " << windowPtr->window().nativewindowid();
                }
            } else {
                LOG_DEBUG << "SMBEWEmbedWindowReq is null ";
            }
        }

        // ProxyObject
        ProxyObject::ProxyObject(QObject *parent) : QObject(parent) {

        }

    }
}
