#include "SMBlobNodeEmbeddedWindows.h"
#include "wxWidgets/SMBlobAppPrivate_wxWidgets.h"
#include "wx/app.h"


class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
    wxTextCtrl *textLog;

    MyFrame(const wxString &title);

    virtual ~MyFrame() {
    }

private:
//DECLARE_EVENT_TABLE()
};


bool MyApp::OnInit() {

    MyFrame *frame = new MyFrame(
            _T("My wxWidgets Demo"));
//    frame->Show(true);
//    frame->OnIdle()
    return true;
}


MyFrame::MyFrame(const wxString &title)
        : wxFrame(NULL, wxID_ANY, title) {
}

MyApp &wxGetMyApp() { return *static_cast<MyApp *>(wxApp::GetInstance()); }

wxAppConsole *wxCreateApp() {
    wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE,
                                    "smblob-node-embedded-windows");
    return new MyApp;
}

// Make optional if we run in wxWidget environment
wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);


namespace SMBlob {
    namespace EmbeddedWindows {

#ifdef _WIN32
        DWORD WINAPI hook_thread_proc(LPVOID arg) {
#else

        void *wxWidgets_app_thread_proc(void *arg) {
#endif
            wxWidgets::wxWidgetsAppWrapper *wrapper = (wxWidgets::wxWidgetsAppWrapper *) arg;

            int argc = 0;
            char **argv = nullptr;

            wxInitializer initializer(argc, argv);
            // Create and set the app value.
            wrapper->app = wxTheApp;

            // Make sure we signal that we have passed any exception throwing code for
            // the waiting application.
#ifdef _WIN32
            // Make sure we signal that we have passed any exception throwing code for
            // the waiting application.
            SetEvent(&worker->hook_control_cond);

            return status;
#else
            // Make sure we signal that we have passed any exception throwing code for
            // the waiting application.

            pthread_cond_signal(&wrapper->wxWidgets_app_control_cond);
            pthread_mutex_unlock(&wrapper->wxWidgets_app_control_mutex);

            wxEntry(argc, argv);

            return NULL;
#endif
        }

        SMBlobApp Init() {
            SMBlobApp res;
            std::shared_ptr<wxWidgets::wxWidgetsAppWrapper> wrapper = std::make_shared<wxWidgets::wxWidgetsAppWrapper>();

            wxApp *pApp = wxTheApp;
            if (!pApp) {

                // Create the thread attribute.
                pthread_attr_t thread_attr;
                pthread_attr_init(&thread_attr);

                void *arg = (void *) wrapper.get();
                if (pthread_create(&wrapper->wxWidgets_app_thread, &thread_attr, wxWidgets_app_thread_proc, arg) == 0) {
                    pthread_cond_wait(&wrapper->wxWidgets_app_control_cond, &wrapper->wxWidgets_app_control_mutex);
                }

                wxWidgets::SMBlobAppPrivate_wxWidgets *appPrivate = new wxWidgets::SMBlobAppPrivate_wxWidgets(
                        wrapper->app,
                        false);
                appPrivate->wrapper = wrapper;

                res.applicationInstance = reinterpret_cast<SMBlobEmbeddedWindowsApplicationInstance>(appPrivate);

            } else {
                wxWidgets::SMBlobAppPrivate_wxWidgets *appPrivate = new wxWidgets::SMBlobAppPrivate_wxWidgets(pApp,
                                                                                                              true);
                res.applicationInstance = static_cast<SMBlobEmbeddedWindowsApplicationInstance>(appPrivate);
            }

            return res;
        }

        void Release(SMBlobApp &app) {
            if (app.applicationInstance) {
                wxWidgets::SMBlobAppPrivate_wxWidgets *appPrivate = reinterpret_cast<wxWidgets::SMBlobAppPrivate_wxWidgets *>(app.applicationInstance);
                if (appPrivate->existed) {
                    // TODO implement and test
                } else {
                    if (appPrivate->application) {
                        appPrivate->application->Exit();
                        appPrivate->application = nullptr;
                    }
                }

            }
        }

    }
}