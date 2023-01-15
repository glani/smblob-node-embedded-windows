#include "embedded-window.h"
#include "BaseWindowActor.h"
#include "plog/Log.h"
#include <QEvent>
#include <QGuiApplication>
#include <QWindow>
#include <QWidget>
#include <QTimer>
#include <QScreen>


namespace SMBlob {
    namespace EmbeddedWindows {

        EmbeddedWindow::EmbeddedWindow(const SMBEWEmbedWindowReq &request,
                                       SMBlob::EmbeddedWindows::BaseProcessor *processor) : foreignWindow(0),
                                                                                            container(0),
                                                                                            embeddedNativeWindowId(0),
                                                                                            nativeWindowReady(false),
                                                                                            processor(processor),
                                                                                            reparentReadyMask(
                                                                                                    ReparentReadyMask::NONE) {

            this->windowHelperPtr = QSharedPointer<EmbeddedWindowHelper>::create(this);
            this->requestPtr = QSharedPointer<SMBEWEmbedWindowReq>::create(request);

            const QString title = QLatin1String("Qt ") + QLatin1String(QT_VERSION_STR);
            setWindowTitle(title);

            setObjectName("EmbeddedWindow");

            this->embeddedNativeWindowId = request.window().nativewindowid();

#ifdef LINUX
            this->container = new NativeWindowWidget(getNativeWindow(), this);
            this->windowHelperPtr->assign(container);
            this->windowId = this->winId();

            container->setObjectName("NativeContainer");
            const QString &nativeWindowIdStr = QString::number(embeddedNativeWindowId, 16);

            LOGD << "ForeignWindow Id:" << embeddedNativeWindowId << " [ 0x" << nativeWindowIdStr.toStdString() << " ]";
            const QString &windowIdStr = QString::number(this->windowId, 16);
            LOGD << "EmbeddedWindow Id:" << this->winId() << " [ 0x" << windowIdStr.toStdString() << " ]";
            LOGD << "container Id:" << this->container->winId();


            if (container) {
                setCentralWidget(container);
                this->processor->windowActor->subscribe(getNativeWindow());
            }

#else
            this->foreignWindow = QWindow::fromWinId(embeddedNativeWindowId);
            if (foreignWindow) {
                this->nativeWindowHelperPtr = QSharedPointer<NativeWindowHelper>::create(this->foreignWindow);
                QObject::connect(this->nativeWindowHelperPtr.get(), SIGNAL(onVisible(bool)), this, SLOT(nativeVisible(bool)));

                const QString &nativeWindowIdStr = QString::number(embeddedNativeWindowId, 16);
                foreignWindow->setObjectName("ForeignWindow_" + nativeWindowIdStr);
                LOGD << "ForeignWindow Id:" << embeddedNativeWindowId <<" [ 0x" << nativeWindowIdStr.toStdString() << " ]";
                const QString &windowIdStr = QString::number(this->winId(), 16);
                LOGD << "EmbeddedWindow Id:" << this->winId() <<" [ 0x" << windowIdStr.toStdString() << " ]";

                this->processor->windowActor->sendNewParent(getNativeWindow(), getWindow());

                this->container = createWindowContainer(foreignWindow, nullptr, Qt::Widget);
                this->windowHelperPtr->assign(container);
                container->setObjectName("Container");
                if (container) {
                    setCentralWidget(container);
                }
            }
#endif
            const QRect availableGeometry = QGuiApplication::primaryScreen()->availableGeometry();
            QPoint pos =
                    availableGeometry.topLeft() + QPoint(availableGeometry.width(), availableGeometry.height()) / 3;
            this->move(pos);
            this->resize(availableGeometry.size() / 4);
            this->show();
            tryToActivateForeignWindow();
            this->container->activateWindow();
        }

        void EmbeddedWindow::tryToActivateForeignWindow() {
            if (foreignWindow) {
                foreignWindow->requestActivate();
            }
        }

        EmbeddedWindow::~EmbeddedWindow() {

        }

        void EmbeddedWindow::nativeVisible(bool show) {
            LOGD << "EmbeddedWindow NativeWindow " << (show ? "SHOW" : "HIDE");
            // assume that native window is ready in case of TRUE
            this->nativeWindowReady = show;
            if (this->nativeWindowReady) {
                tryToActivateForeignWindow();
                this->container->activateWindow();
//                this->processor->windowActor->sendKeySequenceToWindow(window, s, 500);
//                this->foreignWindow->winId()
            }
        }

        const SMBEWEmbedWindow &EmbeddedWindow::getNativeWindow() const {
            return this->embeddedNativeWindowId;
        }

        const SMBEWEmbedWindow &EmbeddedWindow::getWindow() const {
            return this->windowId;
        }

        void EmbeddedWindow::tryFirstRunKeys() {
//            LOGD << "EmbeddedWindow tryFirstRunKeys ";
//            std::string s("F11");
//            SMBEWEmbedWindow window = this->getWindow();
//            SMBEWEmbedWindow nativeWindow = this->getNativeWindow();
//             IMPORTANT linux specific
//            this->processor->windowActor->sendFocusToWindow(nativeWindow, true);
//            this->processor->windowActor->sendKeySequenceToWindow(nativeWindow, s, 500);
//            auto size = this->container->size();
//            this->processor->windowActor->setSize(this->getNativeWindow(), size.width(), size.height());
        }

        bool EmbeddedWindow::event(QEvent *event) {
            if (event->type() == QEvent::HoverEnter
                || event->type() == QEvent::HoverMove
                || event->type() == QEvent::Leave
                || event->type() == QEvent::HoverLeave) {
                return QMainWindow::event(event);
            }

            if (event->type() == QEvent::Resize) {
                resizeNative();
            } else if (event->type() == QEvent::WindowActivate) {
//                tryToActivateForeignWindow();
//                this->container->activateWindow();
            }
            qDebug() << "EmbeddedWindow::event " << event->type();
            return QMainWindow::event(event);
        }

        void EmbeddedWindow::resizeNative() const {
            auto size = container->size();
            processor->windowActor->setSize(getNativeWindow(), size.width(), size.height());
        }

        void EmbeddedWindow::windowSubscribed(bool success) {
            this->processor->windowActor->sendNewParent(getNativeWindow(), this->container->winId());
        }

        void EmbeddedWindow::windowReparented(int mask) {
            this->reparentReadyMask |= mask;
            if (this->reparentReadyMask == ReparentReadyMask::READY_LAST) {
                // ready
                LOGD << "Native window ready: " << this->getNativeWindow();
                this->nativeWindowReady = true;
                auto size = container->size();
                processor->windowActor->forceUpdateSize(getNativeWindow(), size.width(), size.height());

                QTimer::singleShot(500, this, SLOT(tryFirstRunKeys()));
            }
        }

        void EmbeddedWindow::customOpaqueRequested(const FrameExtents &frameExtents,
                                                   const OpaqueParameters &opaqueParameters) {
//            uint32_t opaqueWidth = 0;
//            uint32_t opaqueHeight = 0;
//            const uint32_t * items = opaqueParameters.items;
//            if (opaqueParameters.len == 8) {
//                opaqueWidth = items[6];
//                opaqueHeight = items[7];
//            } else if (opaqueParameters.len == 4) {
//                opaqueWidth = items[2];
//                opaqueHeight = items[3];
//            }
//            auto size = container->size();
//            LOGD << "opaqueWidth: " << opaqueWidth;
//            LOGD << "opaqueHeight: " << opaqueHeight;
//            LOGD << "size.width(): " << size.width();
//            LOGD << "size.height(): " << size.height();
//            if (opaqueWidth > size.width() || opaqueHeight > size.height()) {
////                processor->windowActor->forceUpdateSize(getNativeWindow(), size.width() - 1, size.height());
////                this->resizeNative(); // hack for linux
//            }
        }

        EmbeddedWindowHelper::EmbeddedWindowHelper(EmbeddedWindow *embWindow) :
                QObject(0),
                embWindow(embWindow) {
        }


        EmbeddedWindowHelper::~EmbeddedWindowHelper() {

        }

        bool EmbeddedWindowHelper::eventFilter(QObject *, QEvent *ev) {
            if (ev->type() == QEvent::Show) {
//                QTimer::singleShot(5000, embWindow, SLOT(tryFirstRunKeys()));
            }
//            if (ev->type() == QEvent::Hide) {
//                // IMPORTANT for Linux
//                // TODO check other systems
//                return true;
//            }
//            if (ev->type() == QEvent::WindowDeactivate) {
//                // IMPORTANT for Linux
//                // TODO check other systems
//                return true;
//            }
//            qDebug() << "EmbeddedWindowHelper::eventFilter" << ev->type();
//            if (ev->type() == QEvent::ChildRemoved) {
//                LOGD << "EmbeddedWindowHelper ChildRemoved: " << ev->type();
////                qDebug
//            }
//            if (ev->type() == QEvent::ChildAdded) {
////                qDebug
//            }
//            LOGD << "EmbeddedWindowHelper: " << ev->type();
            return false;
        }

        // NativeWindowHelper
        NativeWindowHelper::NativeWindowHelper(QWindow *window)
                : window(window) {
            window->installEventFilter(this);
        }

        NativeWindowHelper::~NativeWindowHelper() {

        }

        bool NativeWindowHelper::eventFilter(QObject *, QEvent *ev) {
            LOGD << "NativeWindowHelper: " << ev->type();
            if (ev->type() == QEvent::Show) {
                emit onVisible(true);
            } else if (ev->type() == QEvent::Hide) {
                emit onVisible(false);
            }
            return false;
        }

        NativeWindowWidget::NativeWindowWidget(SMBEWEmbedWindow nativeWindow, QWidget *parent) : QWidget(parent),
                                                                                                 nativeWindow(
                                                                                                         nativeWindow) {

        }

        NativeWindowWidget::~NativeWindowWidget() {

        }
    }
}