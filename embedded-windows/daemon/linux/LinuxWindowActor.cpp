#include "LinuxWindowActor.h"
#include "linux/XcbInitializer.h"
#include "plog/Log.h"

int self_fprintf(FILE *__restrict __stream,
                 const char *__restrict __format, ...) {

    return 0;
}

int self_vfprintf(FILE *__restrict __s, const char *__restrict __format,
                  __gnuc_va_list __arg) {
    return 0;
}


namespace SMBlob {
    namespace EmbeddedWindows {

        LinuxWindowActor::LinuxWindowActor() : BaseWindowActor() {
            LinuxWindowActorPrivate::getInstance();
            xcbInitializer = std::make_unique<XcbInitializer>();
        }

        LinuxWindowActor::~LinuxWindowActor() {

        }

        bool LinuxWindowActor::sendKeySequenceToWindow(const SMBEWEmbedWindow &window,
                                                       const std::string &keySequence,
                                                       uint32_t milliSecondsDelay) const {
            uint64_t microSeconds = milliSecondsDelay * 1000;
            int ret = xdo_send_keysequence_window(xcbInitializer->xdo(), window,
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
            ret = xdo_reparent_window(xcbInitializer->xdo(), window, parent);
//            if (focus) {
//                ret = xdo_focus_window(xdoPtr.get(), window);
//            } else {
//                ret = xdo_focus_window(xdoPtr.get(), XDefaultRootWindow(xdoPtr.get()->xdpy));
//            }
//            LOGD << ret;
            return ret == XDO_SUCCESS;
        }

        LinuxWindowActorPrivate::LinuxWindowActorPrivate() {
//            ::main_logger_fprintf = &self_fprintf;
//            ::main_logger_vfprintf = &self_vfprintf;
        }

    }
}
