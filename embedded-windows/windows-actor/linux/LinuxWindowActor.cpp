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

        void LinuxWindowActor::startListener() {
            BaseWindowActor::startListener();

            Screen *screen = XDefaultScreenOfDisplay(this->xlibDisplayValue);
            auto rootAttribs = XCB_REPLY_UNCHECKED(xcb_get_window_attributes, this->xcbConnectionValue,
                                                   screen->root);
            const auto existingEventMask = !rootAttribs ? 0 : rootAttribs->your_event_mask;

            const uint32_t mask = XCB_CW_EVENT_MASK;
            uint32_t values[] = {
                    XCB_EVENT_MASK_NO_EVENT
                    | XCB_EVENT_MASK_EXPOSURE
                    | XCB_EVENT_MASK_VISIBILITY_CHANGE
                    | XCB_EVENT_MASK_FOCUS_CHANGE

                    | XCB_EVENT_MASK_ENTER_WINDOW
                    | XCB_EVENT_MASK_LEAVE_WINDOW
                    | XCB_EVENT_MASK_PROPERTY_CHANGE
                    | XCB_EVENT_MASK_STRUCTURE_NOTIFY // for the "MANAGER" atom (system tray notification).
                    | existingEventMask // don't overwrite the event mask on the root window
            };

            xcb_change_window_attributes(this->xcbConnectionValue, screen->root, mask, values);
            auto reply = XCB_REPLY_UNCHECKED(xcb_get_property, this->xcbConnectionValue,
                                             false, screen->root,
                                             atom(XcbAtom::_NET_SUPPORTING_WM_CHECK),
                                             XCB_ATOM_WINDOW, 0, 1024);
            if (reply && reply->format == 32 && reply->type == XCB_ATOM_WINDOW) {
                xcb_window_t windowManager = *((xcb_window_t *)xcb_get_property_value(reply.get()));

                if (windowManager != XCB_WINDOW_NONE) {
                    LOGD << "Window manager Id: " << windowManager;
//                    m_windowManagerName = QXcbWindow::windowTitle(connection, windowManager);
                }
            }
//
//            xcb_depth_iterator_t depth_iterator =
//                    xcb_screen_allowed_depths_iterator(screen);
//
//            while (depth_iterator.rem) {
//                xcb_depth_t *depth = depth_iterator.data;
//                xcb_visualtype_iterator_t visualtype_iterator =
//                        xcb_depth_visuals_iterator(depth);
//
//                while (visualtype_iterator.rem) {
//                    xcb_visualtype_t *visualtype = visualtype_iterator.data;
//                    m_visuals.insert(visualtype->visual_id, *visualtype);
//                    m_visualDepths.insert(visualtype->visual_id, depth->depth);
//                    xcb_visualtype_next(&visualtype_iterator);
//                }
//
//                xcb_depth_next(&depth_iterator);
//            }
            xcb_flush(this->xcbConnectionValue);
        }

        void LinuxWindowActor::listen() {
            xcb_generic_event_t *event;

            while ( (event = xcb_poll_for_event (this->xcbConnectionValue)) ) {
                auto type = event->response_type & ~0x80;
                switch(type) {
                    XCB_CASE(XCB_KEY_PRESS, xcb_key_press_event_t)
                    XCB_CASE(XCB_KEY_RELEASE, xcb_key_release_event_t)
                    XCB_CASE(XCB_BUTTON_PRESS, xcb_button_press_event_t)
                    XCB_CASE(XCB_BUTTON_RELEASE, xcb_button_release_event_t)
                    XCB_CASE(XCB_MOTION_NOTIFY, xcb_motion_notify_event_t)
                    XCB_CASE(XCB_LEAVE_NOTIFY, xcb_leave_notify_event_t)
                    XCB_CASE(XCB_FOCUS_IN, xcb_focus_in_event_t)
                    XCB_CASE(XCB_FOCUS_OUT, xcb_focus_out_event_t)
                    XCB_CASE(XCB_KEYMAP_NOTIFY, xcb_keymap_notify_event_t)
                    XCB_CASE(XCB_EXPOSE, xcb_expose_event_t)
                    XCB_CASE(XCB_GRAPHICS_EXPOSURE, xcb_graphics_exposure_event_t)
                    XCB_CASE(XCB_NO_EXPOSURE, xcb_no_exposure_event_t)
                    XCB_CASE(XCB_VISIBILITY_NOTIFY, xcb_visibility_notify_event_t)
                    XCB_CASE(XCB_CREATE_NOTIFY, xcb_create_notify_event_t)
                    XCB_CASE(XCB_DESTROY_NOTIFY, xcb_destroy_notify_event_t)
                    XCB_CASE(XCB_UNMAP_NOTIFY, xcb_unmap_notify_event_t)
                    XCB_CASE(XCB_MAP_NOTIFY, xcb_map_notify_event_t)
                    XCB_CASE(XCB_MAP_REQUEST, xcb_map_request_event_t)
                    XCB_CASE(XCB_REPARENT_NOTIFY, xcb_reparent_notify_event_t)
                    XCB_CASE(XCB_CONFIGURE_NOTIFY, xcb_configure_notify_event_t)
                    XCB_CASE(XCB_CONFIGURE_REQUEST, xcb_configure_request_event_t)
                    XCB_CASE(XCB_GRAVITY_NOTIFY, xcb_gravity_notify_event_t)
                    XCB_CASE(XCB_RESIZE_REQUEST, xcb_resize_request_event_t)
                    XCB_CASE(XCB_CIRCULATE_NOTIFY, xcb_circulate_notify_event_t)
                    XCB_CASE(XCB_CIRCULATE_REQUEST, xcb_circulate_request_event_t)
                    XCB_CASE(XCB_PROPERTY_NOTIFY, xcb_property_notify_event_t)
                    XCB_CASE(XCB_SELECTION_CLEAR, xcb_selection_clear_event_t)
                    XCB_CASE(XCB_SELECTION_REQUEST, xcb_selection_request_event_t)
                    XCB_CASE(XCB_SELECTION_NOTIFY, xcb_selection_notify_event_t)
                    XCB_CASE(XCB_COLORMAP_NOTIFY, xcb_colormap_notify_event_t)
                    XCB_CASE(XCB_CLIENT_MESSAGE, xcb_client_message_event_t)
                    XCB_CASE(XCB_MAPPING_NOTIFY, xcb_mapping_notify_event_t)
                    XCB_CASE(XCB_GE_GENERIC, xcb_ge_generic_event_t)


                }
//                    case XCB_EXPOSE:
//                        break;
//                }
                /* ...handle the event */
//                LOGD << "event: " << event->response_type;
                free (event);
            }
        }


        LinuxWindowActorPrivate::LinuxWindowActorPrivate() {
            ::main_logger_fprintf = &self_fprintf;
            ::main_logger_vfprintf = &self_vfprintf;
        }


    }
}
