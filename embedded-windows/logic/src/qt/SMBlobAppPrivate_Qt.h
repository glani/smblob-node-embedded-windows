#pragma once

#include <QApplication>
#include "SMBlobNodeEmbeddedWindows.h"

namespace SMBlob {
    namespace EmbeddedWindows {
        struct SMBlobApp;
        namespace qt {
            struct QtAppWrapper {

                pthread_t qt_app_thread;
                pthread_mutex_t qt_app_control_mutex;
                pthread_cond_t qt_app_control_cond;

                QApplication *qtApp;

                virtual ~QtAppWrapper();

                QtAppWrapper();
            };

            class SMBlobAppPrivate_Qt {
            public:
                explicit SMBlobAppPrivate_Qt(QApplication *qApplication, bool existed) :
                        qApplication(qApplication), existed(existed) {

                }

                virtual ~SMBlobAppPrivate_Qt() {
                    if (!existed) {
                        if (qApplication) {
                            delete qApplication;
                            qApplication = nullptr;
                        }
                    }
                }

                QApplication *getQApplication() const {
                    return qApplication;
                }

            private:
                QApplication *qApplication;
                bool existed;


                friend void SMBlob::EmbeddedWindows::Release(SMBlobApp &app);
                friend SMBlobApp SMBlob::EmbeddedWindows::Init();

                std::shared_ptr<QtAppWrapper> qtAppWrapper;
            };

        }
    }
}