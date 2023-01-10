#pragma once

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"


// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/artprov.h"
#include "wx/bookctrl.h"
#include "wx/sysopt.h"
#include "wx/wupdlock.h"

#include "wx/display.h"
#include <iostream>

#include "SMBlobNodeEmbeddedWindows.h"

namespace SMBlob {
    namespace EmbeddedWindows {
        struct SMBlobApp;
        namespace wxWidgets {
            struct wxWidgetsAppWrapper {

                pthread_t wxWidgets_app_thread;
                pthread_mutex_t wxWidgets_app_control_mutex;
                pthread_mutex_t wxWidgets_app_running_mutex;
                pthread_cond_t wxWidgets_app_control_cond;


                virtual ~wxWidgetsAppWrapper() {
                    pthread_mutex_destroy(&wxWidgets_app_running_mutex);
                    pthread_mutex_destroy(&wxWidgets_app_control_mutex);
                    pthread_cond_destroy(&wxWidgets_app_control_cond);
                }

                explicit wxWidgetsAppWrapper() {
                    pthread_mutex_init(&wxWidgets_app_running_mutex, NULL);
                    pthread_mutex_init(&wxWidgets_app_control_mutex, NULL);
                    pthread_cond_init(&wxWidgets_app_control_cond, NULL);
                }

                wxApp *app;
            };

            class SMBlobAppPrivate_wxWidgets {
            public:
                explicit SMBlobAppPrivate_wxWidgets(wxApp *application, bool existed) :
                        application(application), existed(existed) {

                }

                virtual ~SMBlobAppPrivate_wxWidgets() {
                    if (!existed) {
                        if (application) {
                            delete application;
                            application = nullptr;
                        }
                    }
                }

            private:
                wxApp* application;
                bool existed;


                friend void SMBlob::EmbeddedWindows::Release(SMBlobApp &app);
                friend SMBlobApp SMBlob::EmbeddedWindows::Init();

                std::shared_ptr<wxWidgetsAppWrapper> wrapper;
            };

        }
    }
}