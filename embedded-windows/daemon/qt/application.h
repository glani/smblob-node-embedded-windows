#pragma once

#include "BaseProcessor.h"
#include <QApplication>

using WidgetPtr = QSharedPointer<QWidget>;

namespace SMBlob {
    namespace EmbeddedWindows {
//
//        class SMBEWReleaseWindowReq;
//
//        class SMBEWCloseWindowReq;
//
//        class SMBEWEmbedWindowReq;

        class ProxyObject : public QObject {
        Q_OBJECT
        public:
            explicit ProxyObject(QObject *parent = 0);

        public slots:

        signals:
            void embedWindowRequested(std::shared_ptr<SMBEWEmbedWindowReq> request);
            void exitRequested();
        };


        class Application : public QApplication, public SMBlob::EmbeddedWindows::BaseProcessor {
        Q_OBJECT
        public:
            explicit Application(int &argc, char **argv);

            virtual ~Application();

            // calls come from another thread
            void requestExit() override;

            void releaseWindow(const SMBEWReleaseWindowReq &req) override;

            void closeWindow(const SMBEWCloseWindowReq &req) override;

            void embedWindow(const SMBEWEmbedWindowReq &req) override;

        public slots:

            // main thread execution
            void exitRequested();

            void embedWindowRequested(std::shared_ptr<SMBEWEmbedWindowReq> request);

        private:
            // required to delegate all requests to main thread
            ProxyObject proxyObject;

            static void logSMBEWEmbedWindowReq(std::shared_ptr<SMBEWEmbedWindowReq>& sharedPtr);

            std::vector<WidgetPtr> embeddedWindows;
        };
    }
}
