#include "embedded-window.h"
#include "BaseWindowActor.h"
#include "plog/Log.h"
#include <QEvent>
#include <QGuiApplication>
#include <QWindow>
#include <QTimer>
#include <QScreen>


namespace SMBlob {
    namespace EmbeddedWindows {

        EmbeddedWindow::EmbeddedWindow(const SMBEWEmbedWindowReq &request,
                                       SMBlob::EmbeddedWindows::BaseProcessor *processor) : foreignWindow(0),
                                                                                            container(0),
                                                                                            embeddedNativeWindowId(0),
                                                                                            nativeWindowReady(false),
                                                                                            processor(processor) {

            this->windowHelperPtr = QSharedPointer<EmbeddedWindowHelper>::create(this);
            this->requestPtr = QSharedPointer<SMBEWEmbedWindowReq>::create(request);

            const QString title = QLatin1String("Qt ") + QLatin1String(QT_VERSION_STR);
            setWindowTitle(title);

            setObjectName("EmbeddedWindow");

            this->embeddedNativeWindowId = request.window().nativewindowid();
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
                QTimer::singleShot(100, this, SLOT(tryFirstRunKeys()));
//                this->processor->windowActor->sendKeySequenceToWindow(window, s, 500);
//                this->foreignWindow->winId()
            }
        }

        SMBEWEmbedWindow EmbeddedWindow::getNativeWindow() const {
            return this->embeddedNativeWindowId;
        }

        SMBEWEmbedWindow EmbeddedWindow::getWindow() const {
            return this->winId();
        }

        void EmbeddedWindow::tryFirstRunKeys() {
            LOGD << "EmbeddedWindow tryFirstRunKeys ";
            std::string s("F11");
            SMBEWEmbedWindow window = this->getWindow();
            SMBEWEmbedWindow nativeWindow = this->getNativeWindow();
            // IMPORTANT linux specific
//            this->processor->windowActor->sendFocusToWindow(nativeWindow, true);
            this->processor->windowActor->sendKeySequenceToWindow(nativeWindow, s, 500);
        }

        bool EmbeddedWindow::event(QEvent *event) {
            if (event->type() == QEvent::WindowActivate) {
                tryToActivateForeignWindow();
                this->container->activateWindow();
            }
//            qDebug() << "EmbeddedWindow::event " << event->type();
            return QMainWindow::event(event);
        }


        EmbeddedWindowHelper::EmbeddedWindowHelper(EmbeddedWindow *embWindow) :
                QObject(0),
                embWindow(embWindow) {
        }


        EmbeddedWindowHelper::~EmbeddedWindowHelper() {

        }

        bool EmbeddedWindowHelper::eventFilter(QObject *, QEvent *ev) {
            if (ev->type() == QEvent::Hide) {
                // IMPORTANT for Linux
                // TODO check other systems
                return true;
            }
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
    }
}