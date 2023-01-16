#pragma once

#include <QMainWindow>
#include "internal/SMBlobNodeEmbeddedWindowsSharedPbModels.h"
#include "BaseProcessor.h"
#include "BaseWindowActor.h"
#include "types.h"

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

        // TODO move to a dedicated class
        class NativeWindowWidget : public QWidget {
        Q_OBJECT
        public:
            explicit NativeWindowWidget(SMBEWEmbedWindow nativeWindow, QWidget *parent);
            virtual ~NativeWindowWidget();
        private:
            SMBEWEmbedWindow nativeWindow;
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

            bool event(QEvent *event) override;

        public slots:
            void nativeVisible(bool show);

            void tryFirstRunKeys();

            void windowSubscribed(bool success);
            void nativeWindowDestroyed();
            void windowReparented(const SMBEWEmbedWindow& parentId, int mask);
            void customOpaqueRequested();
            void closeEvent(QCloseEvent *event) override;
        public:
            const SMBEWEmbedWindow& getNativeWindow() const;
            const SMBEWEmbedWindow& getWindow() const;

        private:
            QSharedPointer<EmbeddedWindowHelper> windowHelperPtr;
            QSharedPointer<NativeWindowHelper> nativeWindowHelperPtr;
            QSharedPointer<SMBEWEmbedWindowReq> requestPtr;

            QWidget *container;
            QWindow *foreignWindow;

            void tryToActivateForeignWindow();

            bool nativeWindowReady;
            SMBEWEmbedWindow embeddedNativeWindowId;
            SMBEWEmbedWindow realParentWindowId;
            SMBEWEmbedWindow windowId;
            BaseProcessor *processor;
            int reparentReadyMask;


            bool nativeWindowClosed;

            void resizeNative() const;

            friend class EmbeddedWindowHelper;

            void forceClose();

            const SMBEWEmbedWindow &getRealParentWindow();
        };

    }
}