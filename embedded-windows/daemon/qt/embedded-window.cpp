#include "embedded-window.h"
#include "BaseWindowActor.h"
#include "plog/Log.h"
#include "application.h"
#include <QEvent>
#include <QGuiApplication>
#include <QWindow>
#include <QWidget>
#include <QTimer>
#include <QScreen>
#include <QCloseEvent>


namespace SMBlob {
    namespace EmbeddedWindows {

        EmbeddedWindow::EmbeddedWindow(const SMBEWEmbedWindowReq &request,
                                       SMBlob::EmbeddedWindows::BaseProcessor *processor) :
                container(0),
                nativeWindowClosed(true),
                nativeWindowClosing(false),
                nativeWindowReady(false),
                processor(processor),
                reparentReadyMask(
                        ReparentReadyMask::NONE) {

            this->windowHelperPtr = QSharedPointer<EmbeddedWindowHelper>::create(this);
            this->requestPtr = QSharedPointer<SMBEWEmbedWindowReq>::create(request);
            this->realParentWindowId = SMBEWEmbedWindowNull;

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
            auto realParentWindowId = this->container->winId();
            LOGD << "Real parent Id: " << realParentWindowId;

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
            this->container->activateWindow();
        }

        EmbeddedWindow::~EmbeddedWindow() {
            LOG_DEBUG << "EmbeddedWindow::~EmbeddedWindow";
        }

        void EmbeddedWindow::tryToActivateForeignWindow() {
//            if (foreignWindow) {
//                foreignWindow->requestActivate();
//            }
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

        void EmbeddedWindow::tryFirstRunKeys() {
//            LOGD << "EmbeddedWindow tryFirstRunKeys ";
//            std::string s("F11");
//            SMBEWEmbedWindow window = this->getWindow();
//            SMBEWEmbedWindow nativeWindow = this->getNativeWindow();
//            // IMPORTANT linux specific
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
            } else if (event->type() ==
                       QEvent::ShowToParent) /* checked on Linux probably QEvent::Show is ok as well*/ {
                // TODO via windowActor
                // !this->processor->windowActor->validateWindowId(this->getRealParentWindow())
                if (this->getRealParentWindow() == SMBEWEmbedWindowNull) {
                    // 500 ms delay just hardcoded
                    this->realParentWindowId = this->container->winId();
                    QTimer::singleShot(500, this, SLOT(setNewParent()));
                }
            }
//            qDebug() << "EmbeddedWindow::event " << event->type();
            return QMainWindow::event(event);
        }

        void EmbeddedWindow::resizeNative() const {
            auto size = container->size();
            processor->windowActor->setSize(getNativeWindow(), size.width(), size.height());
        }

        void EmbeddedWindow::setNewParent() {
            if (this->getRealParentWindow() > 0) {
                // TODO via windowActor
                // this->processor->windowActor->validateWindowId(this->getRealParentWindow())
                const SMBEWEmbedWindow &nativeWindow = getNativeWindow();
                LOGD << "setNewParent" << nativeWindow << " " << this->getRealParentWindow();
                this->processor->windowActor->setNewParent(nativeWindow, this->getRealParentWindow());
            }
        }

        void EmbeddedWindow::windowSubscribed(bool success) {
            this->nativeWindowClosed = false;
            const SMBEWEmbedWindow &nativeWindow = getNativeWindow();
            this->nativeDecorations = this->processor->windowActor->removeDecorations(nativeWindow);
//            LOGD << "xdotool windowunmap " << nativeWindow << "\n"
//<< "xdotool windowreparent " << nativeWindow << " " << this->realParentWindowId << "\n"
//<< "xdotool windowmap --sync " << nativeWindow << "\n";
////             $CHID $NEWPID
////            xdotool windowmap --sync $CHID
////            this->resizeNative()
//this->setNewParent();
//            qDebug() << "dfssdfsdf: " << this->container->isVisible();
//            QTimer::singleShot(1000, this, SLOT(setNewParent()));
//            QTimer::singleShot(1000, this, SLOT(setNewParent()));

//
        }

        void EmbeddedWindow::windowReparented(const SMBEWEmbedWindow &parentId, int mask) {

            bool properParent = this->processor->windowActor->validateWindowEquality(parentId,
                                                                                     this->getRealParentWindow());
            if (mask == ReparentReadyMask::STEP_REPARENT
                && properParent) {
                // ready
                LOGD << "Native window ready: " << this->getNativeWindow();
                this->nativeWindowReady = true;
                auto size = container->size();
                processor->windowActor->forceUpdateSize(getNativeWindow(), size.width(), size.height());
            } else {
                if (nativeWindowClosing) {
                    this->close();
                }
            }
        }

        void EmbeddedWindow::customOpaqueRequested() {
            auto opaqueParameters = processor->windowActor->getOpaqueParameters(getNativeWindow());
            if (opaqueParameters) {
                uint32_t opaqueWidth = 0;
                uint32_t opaqueHeight = 0;
                const uint32_t *items = opaqueParameters->items;
                if (opaqueParameters->len == 8) {
                    opaqueWidth = items[6];
                    opaqueHeight = items[7];
                } else if (opaqueParameters->len == 4) {
                    opaqueWidth = items[2];
                    opaqueHeight = items[3];
                }
                auto size = container->size();
                if (opaqueWidth > size.width() || opaqueHeight > size.height()) {
                    processor->windowActor->forceUpdateSize(getNativeWindow(), size.width() - 1, size.height());
                    this->resizeNative(); // hack for linux
                }
            }
        }

        void EmbeddedWindow::closeEvent(QCloseEvent *event) {
            if (!this->nativeWindowClosed && this->nativeWindowReady) {
                this->nativeWindowClosing = true;
                this->nativeWindowClosed = true;
                this->nativeWindowReady = false;
                reparentReadyMask = ReparentReadyMask::NONE;
                auto accept = this->processor->windowActor->closeWindow(this->getNativeWindow(),
                                                                        this->getRealParentWindow()
                );
                if (accept) {
                    event->accept();
                } else {
                    event->ignore();
                }
            } else {
                this->processor->windowActor->closeWindowGracefully(this->getNativeWindow());
                (static_cast<Application *>qApp)->releaseWindowById(this->getWindow());
                event->accept();
            }
        }

        void EmbeddedWindow::nativeWindowDestroyed() {
            this->close();
        }

        const SMBEWEmbedWindow &EmbeddedWindow::getNativeWindow() const {
            return this->embeddedNativeWindowId;
        }

        const SMBEWEmbedWindow &EmbeddedWindow::getWindow() const {
            return this->windowId;
        }

        const SMBEWEmbedWindow &EmbeddedWindow::getRealParentWindow() {
            return this->realParentWindowId;
        }


        // EmbeddedWindowHelper 

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