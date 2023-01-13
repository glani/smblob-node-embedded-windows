#include "application.h"
#include "SMBlobNodeEmbeddedWindowsShared.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedPbModels.h"
#include "plog/Log.h"
#include <QThread>


namespace SMBlob {
    namespace EmbeddedWindows {
        Application::Application(int &argc, char **argv) : QApplication(argc, argv, ApplicationFlags),
                                                           SMBlob::EmbeddedWindows::BaseProcessor() {
            QObject::connect(&proxyObject, SIGNAL(exitRequested()), this, SLOT(exitRequested()));
        }


        Application::~Application() {

        }

        void Application::requestExit() {
            emit proxyObject.exitRequested();
        }

        void Application::exitRequested() {
            LOG_DEBUG << "Application::exitRequested";
            this->quit();
            this->closeAllWindows();
        }


        void Application::releaseWindow(const SMBEWReleaseWindowReq &req) {

        }

        void Application::closeWindow(const SMBEWCloseWindowReq &req) {

        }

        void Application::embedWindow(const SMBEWEmbedWindowReq &req) {

        }

        // ProxyObject
        ProxyObject::ProxyObject(QObject *parent) : QObject(parent) {

        }

    }
}