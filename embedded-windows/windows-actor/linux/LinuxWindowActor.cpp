#include "LinuxWindowActor.h"
#include "plog/Log.h"

#include <xcb/xcb_icccm.h>
#include <X11/Xlibint.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


int self_fprintf(FILE *__restrict __stream,
                 const char *__restrict __format, ...) {

    LOGE << "Error here";
    return 0;
}

int self_vfprintf(FILE *__restrict __s, const char *__restrict __format,
                  __gnuc_va_list __arg) {
    LOGE << "Error here";
    return 0;
}


static const char *const xcbConnectionErrors[] = {
        "No error",                                  /* Error 0 */
        "I/O error",                                 /* XCB_CONN_ERROR */
        "Unsupported extension used",                /* XCB_CONN_CLOSED_EXT_NOTSUPPORTED */
        "Out of memory",                             /* XCB_CONN_CLOSED_MEM_INSUFFICIENT */
        "Maximum allowed requested length exceeded", /* XCB_CONN_CLOSED_REQ_LEN_EXCEED */
        "Failed to parse display string",            /* XCB_CONN_CLOSED_PARSE_ERR */
        "No such screen on display",                 /* XCB_CONN_CLOSED_INVALID_SCREEN */
        "Error during FD passing"                    /* XCB_CONN_CLOSED_FDPASSING_FAILED */
};

static int nullErrorHandler(Display *dpy, XErrorEvent *err)
{
    const int buflen = 1024;
    char buf[buflen];

    XGetErrorText(dpy, err->error_code, buf, buflen);
    self_fprintf(stderr, "X Error: serial %lu error %d %s\n", err->serial, (int)err->error_code, buf);
    return 0;
}

static int ioErrorHandler(Display *dpy)
{
    xcb_connection_t *conn = XGetXCBConnection(dpy);
    if (conn != nullptr) {
        /* Print a message with a textual description of the error */
        int code = xcb_connection_has_error(conn);
        const char *str = "Unknown error";
        int arrayLength = sizeof(xcbConnectionErrors) / sizeof(xcbConnectionErrors[0]);
        if (code >= 0 && code < arrayLength)
            str = xcbConnectionErrors[code];

        self_fprintf(stderr, "The X11 connection broke: %s (code %d)", str, code);
    }
    return _XDefaultIOError(dpy);
}


namespace SMBlob {
    namespace EmbeddedWindows {

        LinuxWindowActor::LinuxWindowActor() : BaseWindowActor() {
            LinuxWindowActorPrivate::getInstance();
            xdo_t *value = xdo_new(NULL);
            xdo_pointer a{value, XdoFreeDeleter()};
            xdoPtr = std::move(a);
            Display *dpy = xdoPtr->xdpy;
            if (dpy) {
                XSetEventQueueOwner(dpy, XCBOwnsEventQueue);
                XSetErrorHandler(nullErrorHandler);
                XSetIOErrorHandler(ioErrorHandler);
                xlibDisplayValue = dpy;
                rootWindowValue = XDefaultRootWindow(xlibDisplayValue);
                primaryScreenNumberValue = DefaultScreen(dpy);
            }
            xcbConnectionValue = xcb_connect(NULL, NULL);
            setupValues = xcb_get_setup(xcbConnectionValue);
            xcbAtomValue.initialize(xcbConnectionValue);
        }

        LinuxWindowActor::~LinuxWindowActor() {
            if (isConnected()) {
                xcb_disconnect(xcbConnectionValue);
            }
        }

        bool LinuxWindowActor::sendKeySequenceToWindow(const SMBEWEmbedWindow &window,
                                                       const std::string &keySequence,
                                                       uint32_t milliSecondsDelay) const {
            uint64_t microSeconds = milliSecondsDelay * 1000;
            int ret = xdo_send_keysequence_window(xdoPtr.get(), window,
                                                  keySequence.c_str(),
                                                  microSeconds);
            return ret == XDO_SUCCESS;
        }

        bool LinuxWindowActor::sendFocusToWindow(const SMBEWEmbedWindow &window, bool focus) const {
            int ret = XDO_SUCCESS;
//            if (focus) {
//                ret = xdo_focus_window(xdoPtr.get(), window);
//            } else {
//                ret = xdo_focus_window(xdoPtr.get(), XDefaultRootWindow(xdoPtr.get()->xdpy));
//            }
//            LOGD << ret;
            return ret == XDO_SUCCESS;
        }

        bool LinuxWindowActor::sendNewParent(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const {
            int ret = XDO_SUCCESS;
            ret = xdo_reparent_window(xdoPtr.get(), window, parent);
//            if (focus) {
//                ret = xdo_focus_window(xdoPtr.get(), window);
//            } else {
//                ret = xdo_focus_window(xdoPtr.get(), XDefaultRootWindow(xdoPtr.get()->xdpy));
//            }
//            LOGD << ret;
            return ret == XDO_SUCCESS;
        }

        void LinuxWindowActor::listen() {

        }

        LinuxWindowActorPrivate::LinuxWindowActorPrivate() {
            ::main_logger_fprintf = &self_fprintf;
            ::main_logger_vfprintf = &self_vfprintf;
        }


    }
}