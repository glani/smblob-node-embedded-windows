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


        void LinuxWindowActor::listen() {
           this->tryToSubscribe();

            xcb_generic_event_t *event;

            while ((event = xcb_poll_for_event(this->xcbInitializer->xcbConnection()))) {
                auto type = event->response_type & ~0x80;
                free(event);
            }
        }


        void LinuxWindowActor::startListener() {
            BaseWindowActor::startListener();
//
//            Screen *screen = XDefaultScreenOfDisplay(this->xlibDisplayValue);
//            auto rootAttribs = XCB_REPLY_UNCHECKED(xcb_get_window_attributes, this->xcbConnectionValue,
//                                                   screen->root);
//            const auto existingEventMask = !rootAttribs ? 0 : rootAttribs->your_event_mask;
//
//            const uint32_t mask = XCB_CW_EVENT_MASK;
//            uint32_t values[] = {
//                    XCB_EVENT_MASK_NO_EVENT
//                    | XCB_EVENT_MASK_EXPOSURE
//                    | XCB_EVENT_MASK_VISIBILITY_CHANGE
//                    | XCB_EVENT_MASK_FOCUS_CHANGE
//
//                    | XCB_EVENT_MASK_ENTER_WINDOW
//                    | XCB_EVENT_MASK_LEAVE_WINDOW
//                    | XCB_EVENT_MASK_PROPERTY_CHANGE
//                    | XCB_EVENT_MASK_STRUCTURE_NOTIFY // for the "MANAGER" atom (system tray notification).
//                    | existingEventMask // don't overwrite the event mask on the root window
//            };
//
//            xcb_change_window_attributes(this->xcbConnectionValue, screen->root, mask, values);
//            auto reply = XCB_REPLY_UNCHECKED(xcb_get_property, this->xcbConnectionValue,
//                                             false, screen->root,
//                                             atom(XcbAtom::_NET_SUPPORTING_WM_CHECK),
//                                             XCB_ATOM_WINDOW, 0, 1024);
//            if (reply && reply->format == 32 && reply->type == XCB_ATOM_WINDOW) {
//                xcb_window_t windowManager = *((xcb_window_t *)xcb_get_property_value(reply.get()));
//
//                if (windowManager != XCB_WINDOW_NONE) {
//                    LOGD << "Window manager Id: " << windowManager;
////                    m_windowManagerName = QXcbWindow::windowTitle(connection, windowManager);
//                }
//            }
////
////            xcb_depth_iterator_t depth_iterator =
////                    xcb_screen_allowed_depths_iterator(screen);
////
////            while (depth_iterator.rem) {
////                xcb_depth_t *depth = depth_iterator.data;
////                xcb_visualtype_iterator_t visualtype_iterator =
////                        xcb_depth_visuals_iterator(depth);
////
////                while (visualtype_iterator.rem) {
////                    xcb_visualtype_t *visualtype = visualtype_iterator.data;
////                    m_visuals.insert(visualtype->visual_id, *visualtype);
////                    m_visualDepths.insert(visualtype->visual_id, depth->depth);
////                    xcb_visualtype_next(&visualtype_iterator);
////                }
////
////                xcb_depth_next(&depth_iterator);
////            }
//            xcb_flush(this->xcbConnectionValue);
        }

        void LinuxWindowActor::tryToSubscribe() {
            std::unique_lock<std::mutex> lock(this->subscribeMutex);

            if (!this->requestQueue.empty()) {

            }

            lock.unlock();
        }

        void LinuxWindowActor::subscribe(const SMBEWEmbedWindow &window) {
            BaseWindowActor::subscribe(window);
            std::unique_lock<std::mutex> lock(this->subscribeMutex);
            struct SubscribeHolder v{window};
            this->requestQueue.emplace(std::move(v));
            lock.unlock();
        }


        LinuxWindowActorPrivate::LinuxWindowActorPrivate() {
            ::main_logger_fprintf = &self_fprintf;
            ::main_logger_vfprintf = &self_vfprintf;
        }
    }
}
