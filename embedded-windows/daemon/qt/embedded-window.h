#pragma once

#include <QMainWindow>
#include "internal/SMBlobNodeEmbeddedWindowsSharedPbModels.h"
#include "BaseProcessor.h"

namespace SMBlob {
    namespace EmbeddedWindows {
        class EmbeddedWindow;

        class EmbeddedWindowHelper : public QObject {
        Q_OBJECT
        public:
            explicit EmbeddedWindowHelper(EmbeddedWindow
                                 *embWindow);

            virtual ~EmbeddedWindowHelper();

            bool eventFilter(QObject *, QEvent *ev) override;

        private:
            void assign(QWidget *w) {
                w->installEventFilter(this);
                w->update();
            }
            bool firstTime;
            EmbeddedWindow *embWindow;

            friend class EmbeddedWindow;
        };

        class NativeWindowHelper : public QObject {
        Q_OBJECT
        public:
            explicit NativeWindowHelper(QWindow *window);

            virtual ~NativeWindowHelper();

            bool eventFilter(QObject *, QEvent *ev) override;
        signals:
            void onVisible(bool show);
        private:
            QWindow *window;
            friend class EmbeddedWindow;
        };

        class EmbeddedWindow : public QMainWindow {
        Q_OBJECT

        public:
            explicit EmbeddedWindow(const SMBEWEmbedWindowReq &request, SMBlob::EmbeddedWindows::BaseProcessor *processor);

            virtual ~EmbeddedWindow();

        public slots:
            void nativeVisible(bool show);
        public:

        private:
            QSharedPointer<EmbeddedWindowHelper> windowHelperPtr;
            QSharedPointer<NativeWindowHelper> nativeWindowHelperPtr;
            QSharedPointer<SMBEWEmbedWindowReq> requestPtr;

            friend class EmbeddedWindowHelper;
            QWidget *container;

            QWindow *foreignWindow;

            void tryToActivateForeignWindow();

            bool nativeWindowReady;
        };

    }
}