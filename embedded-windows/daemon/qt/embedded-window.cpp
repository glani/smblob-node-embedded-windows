#include "embedded-window.h"
#include "plog/Log.h"
#include <QEvent>
#include <QGuiApplication>
#include <QWindow>
#include <QScreen>


namespace SMBlob {
    namespace EmbeddedWindows {

        EmbeddedWindow::EmbeddedWindow(const SMBEWEmbedWindowReq &request,
                                       SMBlob::EmbeddedWindows::BaseProcessor *processor) : foreignWindow(0),
                                                                                            container(0),
                                                                                            nativeWindowReady(false){

            this->windowHelperPtr = QSharedPointer<EmbeddedWindowHelper>::create(this);
            this->requestPtr = QSharedPointer<SMBEWEmbedWindowReq>::create(request);

            const QString title = QLatin1String("Qt ") + QLatin1String(QT_VERSION_STR);
            setWindowTitle(title);

            setObjectName("EmbeddedWindow");

            uint64_t nativeWindowId = request.window().nativewindowid();
            this->foreignWindow = QWindow::fromWinId(nativeWindowId);
            if (foreignWindow) {
                this->nativeWindowHelperPtr = QSharedPointer<NativeWindowHelper>::create(this->foreignWindow);
                QObject::connect(this->nativeWindowHelperPtr.get(), SIGNAL(onVisible(bool)), this, SLOT(nativeVisible(bool)));

                const QString &nativeWindowIdStr = QString::number(nativeWindowId, 16);
                foreignWindow->setObjectName("ForeignWindow_" + nativeWindowIdStr);
                LOGD << "ForeignWindow Id: 0x" << nativeWindowIdStr.toStdString();

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
//            if (ev->type() == QEvent::ChildRemoved) {
//                LOGD << "EmbeddedWindowHelper ChildRemoved: " << ev->type();
////                qDebug
//            }
//            if (ev->type() == QEvent::ChildAdded) {
////                qDebug
//            }
            LOGD << "EmbeddedWindowHelper: " << ev->type();
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