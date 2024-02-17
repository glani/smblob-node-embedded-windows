#pragma once
#include "BaseProcessor.h"
#include "types.h"
#include <QApplication>



namespace SMBlob {
    namespace EmbeddedWindows {
        class EmbeddedWindow;
        using EmbeddedWindowPtr = QSharedPointer<EmbeddedWindow>;

        class ProxyObject : public QObject {
        Q_OBJECT
        public:
            explicit ProxyObject(QObject *parent = 0);

        public slots:

        signals:
            void embedWindowRequested(std::shared_ptr<SMBEWEmbedWindowReq>);
            void exitRequested();
            void embeddedWindowDestroyed(const SMBEWEmbedWindow&);
            void embeddedWindowFocused(const SMBEWEmbedWindow &, bool);
            void embeddedWindowSubscribed(const SMBEWEmbedWindow &, bool);
            void embeddedWindowReparented(const SMBEWEmbedWindow &, const SMBEWEmbedWindow&, int);
            void embeddedWindowCustomOpaqueRequested(const SMBEWEmbedWindow &);
        };


        class Application : public QApplication, public SMBlob::EmbeddedWindows::BaseProcessor {
        Q_OBJECT

        public:

            explicit Application(int &argc, char **argv);
            virtual ~Application();

            void releaseWindowById(const SMBEWEmbedWindow &window);

            // calls come from another thread
            void requestExit() override {
                emit proxyObject.exitRequested();
            }

            void releaseWindow(const SMBEWReleaseWindowReq &req) override;

            void closeWindow(const SMBEWCloseWindowReq &req) override;

            void embedWindow(const SMBEWEmbedWindowReq &req) override {
                std::shared_ptr<SMBEWEmbedWindowReq> request = std::make_shared<SMBEWEmbedWindowReq>(req);
                emit proxyObject.embedWindowRequested(std::move(request));
            }

            void embeddedWindowDestroy(const SMBEWEmbedWindow& window) {
                emit proxyObject.embeddedWindowDestroyed(window);
            }

            void embeddedWindowFocus(const SMBEWEmbedWindow &window, bool focus) {
                emit proxyObject.embeddedWindowFocused(window, focus);
            }

            void embeddedWindowSubscribe(const SMBEWEmbedWindow &window, bool success) {
                emit proxyObject.embeddedWindowSubscribed(window, success);
            }

            void embeddedWindowReparent(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent, int mask) {
                emit proxyObject.embeddedWindowReparented(window, parent, mask);
            }
            void embeddedWindowCustomOpaqueRequest(const SMBEWEmbedWindow &window) {
                emit proxyObject.embeddedWindowCustomOpaqueRequested(window);
            }


        public slots:

            // main thread execution
            void exitRequested();

            void embedWindowRequested(std::shared_ptr<SMBEWEmbedWindowReq> request);

            void embeddedWindowDestroyed(const SMBEWEmbedWindow& window);

            void embeddedWindowFocused(const SMBEWEmbedWindow &window, bool focus);

            void embeddedWindowSubscribed(const SMBEWEmbedWindow &window, bool success);

            void embeddedWindowReparented(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent, int mask);

            void embeddedWindowCustomOpaqueRequested(const SMBEWEmbedWindow &window);

        private:
            // required to delegate all requests to main thread
            ProxyObject proxyObject;

            static void logSMBEWEmbedWindowReq(std::shared_ptr<SMBEWEmbedWindowReq>& sharedPtr);

            // all operations should be done on the main applcication thread
            std::vector<EmbeddedWindowPtr> embeddedWindows;
        };
    }
}
