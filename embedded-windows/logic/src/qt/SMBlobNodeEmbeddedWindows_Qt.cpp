//#include "SMBlobNodeEmbeddedWindows.h"
////#include "internal/SMBlobNodeEmbeddedWindowsCommon.h"
//#include "qt/SMBlobAppPrivate_Qt.h"
//#include <QApplication>
//#include <QWindow>
//
//namespace SMBlob {
//    namespace EmbeddedWindows {
//
//#ifdef _WIN32
//        DWORD WINAPI hook_thread_proc(LPVOID arg) {
//#else
//
//        void *qt_app_thread_proc(void *arg) {
//#endif
//            qt::QtAppWrapper *wrapper = (qt::QtAppWrapper *) arg;
//            int argc = 0;
//            char **argv = nullptr;
//            // Create and set the app value.
//            wrapper->qtApp = new QApplication(argc, argv);
//
//            // Make sure we signal that we have passed any exception throwing code for
//            // the waiting hook_enable().
//#ifdef _WIN32
//            // Make sure we signal that we have passed any exception throwing code for
//            // the waiting application.
//            SetEvent(&worker->hook_control_cond);
//
//            return status;
//#else
//            // Make sure we signal that we have passed any exception throwing code for
//            // the waiting application.
//
//
//            pthread_cond_signal(&wrapper->qt_app_control_cond);
//            pthread_mutex_unlock(&wrapper->qt_app_control_mutex);
//            wrapper->qtApp->exec();
//
//            return NULL;
//#endif
//        }
//
//        SMBlobApp Init() {
//            SMBlobApp res;
//            auto app = qApp;
//            std::shared_ptr<qt::QtAppWrapper> qtAppWrapper = std::make_shared<qt::QtAppWrapper>();
//            if (!app) {
////                QApplication *qtApp = nullptr;
////
////                qtApp->exit(0);
////                delete qtApp;
//                // Create the thread attribute.
//                pthread_attr_t qt_app_thread_attr;
//                pthread_attr_init(&qt_app_thread_attr);
//
//
//                pthread_mutex_init(&qtAppWrapper->qt_app_control_mutex, NULL);
//                pthread_cond_init(&qtAppWrapper->qt_app_control_cond, NULL);
//
//                // Get the policy and priority for the thread attr.
////                int policy;
////                pthread_attr_getschedpolicy(&qt_app_thread_attr, &policy);
////                int priority = sched_get_priority_max(policy);
//                void *worker = (void*) qtAppWrapper.get();
//                if (pthread_create(&qtAppWrapper->qt_app_thread, &qt_app_thread_attr, qt_app_thread_proc, worker) == 0) {
//                    pthread_cond_wait(&qtAppWrapper->qt_app_control_cond, &qtAppWrapper->qt_app_control_mutex);
//                }
//
//                qt::SMBlobAppPrivate_Qt* appPrivate = new qt::SMBlobAppPrivate_Qt(qtAppWrapper->qtApp, false);
//                appPrivate->qtAppWrapper = qtAppWrapper;
//
//                res.applicationInstance = reinterpret_cast<SMBlobEmbeddedWindowsApplicationInstance>(appPrivate);
//
//            } else {
//                qt::SMBlobAppPrivate_Qt* appPrivate = new qt::SMBlobAppPrivate_Qt(app, true);
//                res.applicationInstance = static_cast<SMBlobEmbeddedWindowsApplicationInstance>(appPrivate);
//            }
//
//            return res;
//        }
//
//        void Release(SMBlobApp& app) {
//            if (app.applicationInstance) {
//                qt::SMBlobAppPrivate_Qt* appPrivate = reinterpret_cast<qt::SMBlobAppPrivate_Qt*>(app.applicationInstance);
//                if (appPrivate->existed) {
//                    // TODO implement and test
//                } else {
//                    if (appPrivate->qApplication) {
////                         appPrivate->qApplication->closeAllWindows();
////                        QApplication::quit();
//
//                        delete appPrivate->qApplication;
//                        appPrivate->qApplication = nullptr;
//                    }
//                }
//
//            }
//        }
//
//        qt::QtAppWrapper::QtAppWrapper() {}
//
//        qt::QtAppWrapper::~QtAppWrapper() {
//            pthread_mutex_destroy(&hook_running_mutex);
//            pthread_mutex_destroy(&hook_control_mutex);
//            pthread_cond_destroy(&hook_control_cond);
//
//        }
//    }
//}