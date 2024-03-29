#include "LinuxWindowActor.h"
#include "linux/XcbInitializer.h"
#include "plog/Log.h"

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


#define XCB_CASE(VALUE, EVENT_NAME) \
    case VALUE: {                   \
        /* make it silent. It is used for DEBUG and development purposes */    \
        LOGD << "" #VALUE;           \
        break;                                \
    }

#define connectionPtr this->xcbInitializer->xcbConnection()
#define xdoPtr xcbInitializer->xdo()

namespace SMBlob {
    namespace EmbeddedWindows {

        LinuxWindowActor::LinuxWindowActor() : BaseWindowActor() {
            LinuxWindowActorPrivate::getInstance();
            xcbInitializer = std::make_unique<XcbInitializer>();
            this->netWmOpaqueRegion = xcbInitializer->internAtom("_NET_WM_OPAQUE_REGION");
            this->kdeNetWmFrameStrut = this->xcbInitializer->internAtom("_KDE_NET_WM_FRAME_STRUT");
            this->gtkFrameExtents = this->xcbInitializer->internAtom("_GTK_FRAME_EXTENTS");
            this->netFrameExtents = this->xcbInitializer->internAtom("_NET_FRAME_EXTENTS");
        }

        LinuxWindowActor::~LinuxWindowActor() {

        }

        bool LinuxWindowActor::sendKeySequenceToWindow(const SMBEWEmbedWindow &window,
                                                       const std::string &keySequence,
                                                       uint32_t milliSecondsDelay) const {
            uint64_t microSeconds = milliSecondsDelay * 1000;
            int ret = xdo_send_keysequence_window(xdoPtr, window,
                                                  keySequence.c_str(),
                                                  microSeconds);
            return ret == XDO_SUCCESS;
        }

        bool LinuxWindowActor::sendFocusToWindow(const SMBEWEmbedWindow &window, bool focus) const {
            int ret = XDO_SUCCESS;
            if (focus) {
                ret = xdo_focus_window(xdoPtr, window);
            } else {
                ret = xdo_focus_window(xdoPtr, xcbInitializer->rootWindow());
            }
            return ret == XDO_SUCCESS;
        }

        bool LinuxWindowActor::setNewParent(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const {
            int ret = XDO_SUCCESS;
//            xdo_t *value = xdo_new(NULL);
//            xdo_pointer _{value, XdoFreeDeleter()};
            auto xdo = xdoPtr;
            ret |= xdo_unmap_window(xdo, window);
            ret |= xdo_reparent_window(xdo, window, parent);
            ret |= xdo_map_window(xdo, window);
//            xdo_wait_for_window_map_state(xdo.get(), window, IsViewable);
//            XUnmapWindow(xcbInitializer->xlibDisplay(), window);
//            XMapWindow(xcbInitializer->xlibDisplay(), window);
//            XSync(xcbInitializer->xlibDisplay(), False);
//            XReparentWindow(xcbInitializer->xlibDisplay(), window, parent, 0, 0);
//            LOGD << "setNewParent: " << window << " " << parent;
//            XMapWindow(xcbInitializer->xlibDisplay(), window);
//            XFlush(xcbInitializer->xlibDisplay());
//            // 1 ms seems to be enough even during `nice -n -19 stress -c $cpuThreadsCount` (pacman -S stress) on linux-tkg-pds.
//            // Probably can be decreased even further.
//            usleep(1e3);
//            XSync(xcbInitializer->xlibDisplay(), False);
//            XFlush(xcbInitializer->xlibDisplay());
//            ret = xdo_reparent_window(xdoPtr, window, parent);
//            xcb_unmap_window(connectionPtr, window);
//            xcbInitializer->flush();
//            xdo_reparent_window(xdoPtr, window, parent);
//            xcb_reparent_window(connectionPtr, window, parent, 0, 0);
//            xcbInitializer->flush();
            return ret == XDO_SUCCESS;
        }

        bool LinuxWindowActor::setSize(const SMBEWEmbedWindow &window, int width, int height) const {
            int ret = XDO_SUCCESS;
            LOGD << "width: " << width << " height: " << height;
            ret = xdo_set_window_size(xdoPtr, window, width, height, 0);
            return ret == XDO_SUCCESS;
        }

        void LinuxWindowActor::listen() {
            this->tryToSubscribe();

            xcb_generic_event_t *event;

            while ((event = xcb_poll_for_event(this->xcbInitializer->xcbConnection()))) {
                auto type = event->response_type & ~0x80;
                switch (type) {
                    XCB_CASE(XCB_KEY_PRESS, xcb_key_press_event_t)
                    XCB_CASE(XCB_KEY_RELEASE, xcb_key_release_event_t)
                    XCB_CASE(XCB_BUTTON_PRESS, xcb_button_press_event_t)
                    XCB_CASE(XCB_BUTTON_RELEASE, xcb_button_release_event_t)
                    XCB_CASE(XCB_MOTION_NOTIFY, xcb_motion_notify_event_t)
                    XCB_CASE(XCB_LEAVE_NOTIFY, xcb_leave_notify_event_t)
//                    XCB_CASE(XCB_FOCUS_IN, xcb_focus_in_event_t)
                    case XCB_FOCUS_IN: {
                        this->focusInEvent((xcb_focus_in_event_t *) event);
                        break;
                    }
//                    XCB_CASE(XCB_FOCUS_OUT, xcb_focus_out_event_t)
                    case XCB_FOCUS_OUT: {
                        this->focusOutEvent((xcb_focus_out_event_t *) event);
                        break;
                    }
                    XCB_CASE(XCB_KEYMAP_NOTIFY, xcb_keymap_notify_event_t)
                    XCB_CASE(XCB_EXPOSE, xcb_expose_event_t)
                    XCB_CASE(XCB_GRAPHICS_EXPOSURE, xcb_graphics_exposure_event_t)
                    XCB_CASE(XCB_NO_EXPOSURE, xcb_no_exposure_event_t)
                    XCB_CASE(XCB_VISIBILITY_NOTIFY, xcb_visibility_notify_event_t)
                    XCB_CASE(XCB_CREATE_NOTIFY, xcb_create_notify_event_t)
//                    XCB_CASE(XCB_DESTROY_NOTIFY, xcb_destroy_notify_event_t)
                    case XCB_DESTROY_NOTIFY: {
                        this->destroyEvent((xcb_destroy_notify_event_t *) event);
                        break;
                    }
//                    XCB_CASE(XCB_UNMAP_NOTIFY, xcb_unmap_notify_event_t)
                    case XCB_UNMAP_NOTIFY: {
                        this->unmapNotifyEvent((xcb_unmap_notify_event_t *) event);
                        break;
                    }
//                    XCB_CASE(XCB_MAP_NOTIFY, xcb_map_notify_event_t)
                    case XCB_MAP_NOTIFY: {
                        this->mapNotifyEvent((xcb_map_notify_event_t *) event);
                        break;
                    }
                    XCB_CASE(XCB_MAP_REQUEST, xcb_map_request_event_t)
//                    XCB_CASE(XCB_REPARENT_NOTIFY, xcb_reparent_notify_event_t)
                    case XCB_REPARENT_NOTIFY: {
                        this->reparentNotifyEvent((xcb_reparent_notify_event_t *) event);
                        break;
                    }
                    XCB_CASE(XCB_CONFIGURE_NOTIFY, xcb_configure_notify_event_t)
                    XCB_CASE(XCB_CONFIGURE_REQUEST, xcb_configure_request_event_t)
                    XCB_CASE(XCB_GRAVITY_NOTIFY, xcb_gravity_notify_event_t)
                    XCB_CASE(XCB_RESIZE_REQUEST, xcb_resize_request_event_t)
                    XCB_CASE(XCB_CIRCULATE_NOTIFY, xcb_circulate_notify_event_t)
                    XCB_CASE(XCB_CIRCULATE_REQUEST, xcb_circulate_request_event_t)
//                    XCB_CASE(XCB_PROPERTY_NOTIFY, xcb_property_notify_event_t)
                    case XCB_PROPERTY_NOTIFY: {
                        this->notifyEvent((xcb_property_notify_event_t *) event);
                        break;
                    }
                    XCB_CASE(XCB_SELECTION_CLEAR, xcb_selection_clear_event_t)
                    XCB_CASE(XCB_SELECTION_REQUEST, xcb_selection_request_event_t)
                    XCB_CASE(XCB_SELECTION_NOTIFY, xcb_selection_notify_event_t)
                    XCB_CASE(XCB_COLORMAP_NOTIFY, xcb_colormap_notify_event_t)
                    XCB_CASE(XCB_CLIENT_MESSAGE, xcb_client_message_event_t)
                    XCB_CASE(XCB_MAPPING_NOTIFY, xcb_mapping_notify_event_t)
                    XCB_CASE(XCB_GE_GENERIC, xcb_ge_generic_event_t)
                }

                free(event);
            }
        }


        void LinuxWindowActor::startListener() {
            BaseWindowActor::startListener();

//            Screen *screen = XDefaultScreenOfDisplay(this->xcbInitializer->xlibDisplay());
//            auto rootAttribs = XCB_REPLY_UNCHECKED(xcb_get_window_attributes, this->xcbInitializer->xcbConnection(),
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
//            xcb_change_window_attributes(this->xcbInitializer->xcbConnection(), screen->root, mask, values);
//            auto reply = XCB_REPLY_UNCHECKED(xcb_get_property, this->xcbInitializer->xcbConnection(),
//                                             false, screen->root,
//                                             this->xcbInitializer->atom(XcbAtom::_NET_SUPPORTING_WM_CHECK),
//                                             XCB_ATOM_WINDOW, 0, 1024);
//            if (reply && reply->format == 32 && reply->type == XCB_ATOM_WINDOW) {
//                xcb_window_t windowManager = *((xcb_window_t *)xcb_get_property_value(reply.get()));
//
//                if (windowManager != XCB_WINDOW_NONE) {
//                    LOGD << "Window manager Id: " << windowManager;
////                    m_windowManagerName = QXcbWindow::windowTitle(connection, windowManager);
//                }
//            }
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
            this->xcbInitializer->flush();
        }

        void LinuxWindowActor::tryToSubscribe() {
            std::unique_lock<std::mutex> lock(this->subscribeMutex);

            while (!requestQueue.empty()) {
                SubscribeHolder &t = requestQueue.front();
                this->subscribeWindow(t.window);
                requestQueue.pop();
            }

            lock.unlock();
        }

        void LinuxWindowActor::subscribe(const SMBEWEmbedWindow &window) {
            LOGD << "Window Subscription scheduled: " << window;
            BaseWindowActor::subscribe(window);
            std::unique_lock<std::mutex> lock(this->subscribeMutex);
            struct SubscribeHolder v{window};
            this->requestQueue.emplace(std::move(v));
            lock.unlock();

        }

        void LinuxWindowActor::subscribeWindow(const SMBEWEmbedWindow &window) {
            uint32_t values[] = {
                    XCB_EVENT_MASK_NO_EVENT
                    | XCB_EVENT_MASK_FOCUS_CHANGE
                    | XCB_EVENT_MASK_PROPERTY_CHANGE
                    | XCB_EVENT_MASK_STRUCTURE_NOTIFY
            };

            const uint32_t mask = XCB_CW_EVENT_MASK;
            xcb_change_window_attributes(this->xcbInitializer->xcbConnection(), window, mask, values);

            this->xcbInitializer->flush();
            LOGD << "Window has subscribed: " << window;
            if (onEmbeddedWindowSubscribedCallback) {
                // true always ?
                onEmbeddedWindowSubscribedCallback(window, true);
            }
        }

        void LinuxWindowActor::focusInEvent(xcb_focus_in_event_t *pEvent) {
            LOGD << "focusInEvent";
            if (onEmbeddedWindowFocusedCallback) {
                SMBEWEmbedWindow window = pEvent->event;
                onEmbeddedWindowFocusedCallback(window, true);
            }
        }

        void LinuxWindowActor::focusOutEvent(xcb_focus_out_event_t *pEvent) {
            LOGD << "focusOutEvent";
            if (onEmbeddedWindowFocusedCallback) {
                SMBEWEmbedWindow window = pEvent->event;
                onEmbeddedWindowFocusedCallback(window, false);
            }
        }

        void LinuxWindowActor::destroyEvent(xcb_destroy_notify_event_t *pEvent) {
            LOGD << "destroyEvent";
            if (onEmbeddedWindowDestroyedCallback) {
                SMBEWEmbedWindow window = pEvent->window;
                onEmbeddedWindowDestroyedCallback(window);
            }
        }

        void LinuxWindowActor::notifyEvent(xcb_property_notify_event_t *pEvent) {
            LOGD << "notifyEvent";
            xcb_window_t window = pEvent->window;
            LOGD << "pEvent->window: " << window;
            LOGD << "pEvent->atom: " << pEvent->atom;
            LOGD << "pEvent->atom: " << xcbInitializer->atomName(pEvent->atom);
            LOGD << "pEvent->sequence: " << pEvent->sequence;
            if (onEmbeddedWindowCustomOpaqueRequestedCallback) {
                if (this->netWmOpaqueRegion == pEvent->atom) {
                    std::shared_ptr<FrameExtents> extentsPtr = this->retrieveFrameExtents(window);
                    FrameExtents empty;
                    auto extents = &empty;
                    if (extentsPtr) {
                        extents = extentsPtr.get();
                        // NULL ptr means fullscreen mode
                    }
                    auto opaqueParameters = getOpaqueParameters(window);

                    if (opaqueParameters) {
                        opaqueParameters->fullscreen = !extentsPtr;
                        onEmbeddedWindowCustomOpaqueRequestedCallback(window);
                    }
                }
            }
        }

        bool LinuxWindowActor::forceUpdateSize(const SMBEWEmbedWindow &window, int width, int height) const {

            auto opaqueParameters = getOpaqueParameters(window);
            if (opaqueParameters) {
                // calculate new opaque region
                uint32_t *items = opaqueParameters->items;
                if (opaqueParameters->len == 8) {
                    uint32_t delta = delta = items[6] - items[2];
                    items[7] = height - opaqueParameters->frameExtents.yb - items[5];
                    items[6] = width - opaqueParameters->frameExtents.xl - opaqueParameters->frameExtents.xr;
                    items[2] = items[6] - delta;
                } else if (opaqueParameters->len == 4) {
                    // could be in fullscreen
                    items[2] = width - 2 * items[0];
                    items[3] = height - 2 * items[1];
                }
                xcb_change_property(connectionPtr, XCB_PROP_MODE_REPLACE, window, this->netWmOpaqueRegion,
                                    XCB_ATOM_CARDINAL, 32, opaqueParameters->len, items);
                this->setSize(window, width, height);
            }

            return true;
        }

        std::shared_ptr<OpaqueParameters> LinuxWindowActor::getOpaqueParameters(const SMBEWEmbedWindow &window) const {
            auto valueWmOpaqueRegion = XCB_REPLY_UNCHECKED(xcb_get_property, connectionPtr, false,
                                                           window,
                                                           netWmOpaqueRegion, XCB_ATOM_CARDINAL, 0, 1024);

            if (valueWmOpaqueRegion && valueWmOpaqueRegion->type == XCB_ATOM_CARDINAL &&
                valueWmOpaqueRegion->format == 32 && valueWmOpaqueRegion->value_len >= 4) {
                std::shared_ptr<OpaqueParameters> opaqueParameters = std::make_shared<OpaqueParameters>();
                uint32_t *ptr = (uint32_t *) xcb_get_property_value(valueWmOpaqueRegion.get());
                opaqueParameters->len = valueWmOpaqueRegion->value_len;
                memcpy(&opaqueParameters->items, ptr, sizeof(uint32_t) * valueWmOpaqueRegion->value_len);
                std::shared_ptr<FrameExtents> extentsPtr = this->retrieveFrameExtents(window);
                FrameExtents empty;
                auto extents = &empty;
                if (extentsPtr) {
                    extents = extentsPtr.get();
                    // NULL ptr means fullscreen mode
                }
                opaqueParameters->frameExtents = *extents;
                return opaqueParameters;
            }
            return nullptr;
        }


        void LinuxWindowActor::unmapNotifyEvent(xcb_unmap_notify_event_t *pEvent) {
            LOGD << "unmapNotifyEvent";
            LOGD << "pEvent->window: " << pEvent->window;
            LOGD << "pEvent->from_configure: " << pEvent->from_configure;
            LOGD << "pEvent->sequence: " << pEvent->sequence;

            if (onEmbeddedWindowReparentedCallback) {
                SMBEWEmbedWindow window = pEvent->window;
                onEmbeddedWindowReparentedCallback(window, 0, ReparentReadyMask::STEP_UNMAP);
            }

        }

        void LinuxWindowActor::mapNotifyEvent(xcb_map_notify_event_t *pEvent) {
            LOGD << "mapNotifyEvent";
            if (onEmbeddedWindowReparentedCallback) {
                SMBEWEmbedWindow window = pEvent->window;
                onEmbeddedWindowReparentedCallback(window, 0, ReparentReadyMask::STEP_MAP);
            }
        }

        void LinuxWindowActor::reparentNotifyEvent(xcb_reparent_notify_event_t *pEvent) {
            LOGD << "reparentNotifyEvent";
            LOGD << "pEvent->window: " << pEvent->window;
            LOGD << "pEvent->parent: " << pEvent->parent;
            LOGD << "pEvent->sequence: " << pEvent->sequence;

            if (onEmbeddedWindowReparentedCallback) {
                SMBEWEmbedWindow window = pEvent->window;
                onEmbeddedWindowReparentedCallback(window, pEvent->parent, ReparentReadyMask::STEP_REPARENT);
            }
        }

        void LinuxWindowActor::setOnEmbeddedWindowDestroyedCallback(
                const std::function<void(const SMBEWEmbedWindow &)> &onEmbeddedWindowDestroyedCallback) {
            this->onEmbeddedWindowDestroyedCallback = onEmbeddedWindowDestroyedCallback;
        }

        void LinuxWindowActor::setOnEmbeddedWindowFocusedCallback(
                const std::function<void(const SMBEWEmbedWindow &, bool)> &onEmbeddedWindowFocusedCallback) {
            this->onEmbeddedWindowFocusedCallback = onEmbeddedWindowFocusedCallback;
        }

        void LinuxWindowActor::setOnEmbeddedWindowSubscribedCallback(
                const std::function<void(const SMBEWEmbedWindow &, bool)> &onEmbeddedWindowSubscribedCallback) {
            this->onEmbeddedWindowSubscribedCallback = onEmbeddedWindowSubscribedCallback;
        }

        void LinuxWindowActor::setOnEmbeddedWindowReparentedCallback(
                const std::function<void(const SMBEWEmbedWindow &, const SMBEWEmbedWindow &,
                                         int)> &onEmbeddedWindowReparentedCallback) {
            this->onEmbeddedWindowReparentedCallback = onEmbeddedWindowReparentedCallback;
        }

        void LinuxWindowActor::setOnEmbeddedWindowCustomOpaqueRequestedCallback(
                const std::function<void(const SMBEWEmbedWindow &)> &onEmbeddedWindowCustomOpaqueRequestedCallback) {
            this->onEmbeddedWindowCustomOpaqueRequestedCallback = onEmbeddedWindowCustomOpaqueRequestedCallback;
        }

        std::shared_ptr<FrameExtents> LinuxWindowActor::retrieveFrameExtents(xcb_window_t window) const {
            std::shared_ptr<FrameExtents> res;

            if (!res) {
                res = getExtents(window, this->netFrameExtents);
                if (res) {
                    return res;
                }
            }
            if (!res) {
                res = getExtents(window, this->gtkFrameExtents);
                if (res) {
                    return res;
                }
            }

            if (!res) {
                res = getExtents(window, this->kdeNetWmFrameStrut);
                if (res) {
                    return res;
                }
            }
        }

        std::shared_ptr<FrameExtents> LinuxWindowActor::getExtents(xcb_window_t window, xcb_atom_t atom) const {
            auto valueFrameExtents = XCB_REPLY_UNCHECKED(xcb_get_property, connectionPtr, false, window,
                                                         atom, XCB_ATOM_CARDINAL, 0, 1024);
            if (valueFrameExtents && valueFrameExtents->type == XCB_ATOM_CARDINAL &&
                valueFrameExtents->format == 32 && valueFrameExtents->value_len == 4) {
                int32_t *ptr = (int32_t *) xcb_get_property_value(valueFrameExtents.get());
                auto res = std::make_shared<FrameExtents>();
                res->xl = ptr[0];
                res->xr = ptr[1];
                res->yt = ptr[2];
                res->yb = ptr[3];
                return res;
            }
            return nullptr;
        }

        bool LinuxWindowActor::closeWindowGracefully(const SMBEWEmbedWindow &window) const {

            xcb_client_message_event_t event;

            event.response_type = XCB_CLIENT_MESSAGE;
            event.window = window;
            event.type = xcbInitializer->atom(XcbAtom::Atom::_NET_CLOSE_WINDOW);
            event.format = 32;
            event.data.data32[0] = 1;
            event.data.data32[1] = XCB_CURRENT_TIME;
            event.data.data32[2] = XCB_NONE;
            event.data.data32[3] = 0;
            event.data.data32[4] = 0;

            auto parentWindow = xcbInitializer->rootWindow();
            xcb_send_event(connectionPtr, false, parentWindow,
                           XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                           (char *) &event);
            this->xcbInitializer->flush();
            return true;
        }

        bool LinuxWindowActor::closeWindow(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const {
            auto rootWindow = xcbInitializer->rootWindow();
            Window parentX;
            Window rootX;
            Window* children;
            unsigned int nchildren;
            auto windowX = window;
            // TODO add window checks every where
            if (xcbInitializer->windowExists(window)) {
                auto status = XQueryTree(xdoPtr->xdpy, windowX, &rootX, &parentX, &children, &nchildren);
                XFree(children);
                if (status > 0) {
                    if (parent && parent == parentX) {
                        xdo_reparent_window(xdoPtr, window, rootWindow);
                        xdo_unmap_window(xdoPtr, window);
                        // error ?
                        closeWindowGracefully(window);
                        return false;
                    } else if (parentX == rootWindow) /* probably all cases */{
                        closeWindowGracefully(window);
                    }
                }
            }

            return true;
        }

        bool LinuxWindowActor::validateWindowEquality(const SMBEWEmbedWindow &window,
                                                      const SMBEWEmbedWindow &windowToCompare) const {
            if (!window) {
                return true;
            } else {
                return window == windowToCompare;
            }
        }

        std::unique_ptr<SMBEWEmbedWindowDecorations>
        LinuxWindowActor::removeDecorations(const SMBEWEmbedWindow &window, const std::unique_ptr<SMBEWEmbedWindowDecorations>& newDecorations) const {
            Atom type;

            xcb_atom_t atom = xcbInitializer->atom(XcbAtom::_MOTIF_WM_HINTS);
            auto valueMotif = XCB_REPLY_UNCHECKED(xcb_get_property, connectionPtr, false, window,
                                                  atom, XCB_ATOM_ANY, 0, 1024);
            if (valueMotif && valueMotif->type == atom &&
                    valueMotif->format == 32 && valueMotif->value_len > 0) {
                std::unique_ptr<SMBEWEmbedWindowDecorations> res = std::make_unique<SMBEWEmbedWindowDecorations>();
                MotifHints *ptr = (MotifHints*) xcb_get_property_value(valueMotif.get());
                res->hints = *ptr;
                if (newDecorations) {
                    this->applyDecorations(window, newDecorations);
                } else {
                    std::unique_ptr<SMBEWEmbedWindowDecorations> noDecorations = std::make_unique<SMBEWEmbedWindowDecorations>();
                    noDecorations->hints.flags = 2;
                    this->applyDecorations(window, noDecorations);
                }
                return res;
            }

            return nullptr;
        }


        bool LinuxWindowActor::applyDecorations(const SMBEWEmbedWindow &window,
                                                const std::unique_ptr<SMBEWEmbedWindowDecorations> &decorations) const {
            if (decorations) {
                xcb_atom_t atom = xcbInitializer->atom(XcbAtom::_MOTIF_WM_HINTS);
                xcb_change_property(connectionPtr, XCB_PROP_MODE_REPLACE, window, atom,
                                    XCB_ATOM_CARDINAL, 32, 5, &decorations->hints);
                this->xcbInitializer->flush();
//                XChangeProperty(xcbInitializer->xlibDisplay(),
//                                window,
//                                atom,
//                                atom,
//                                32,
//                                PropModeReplace,
//                                reinterpret_cast<unsigned char*>(hints),
//                                sizeof(MotifWmHints)/sizeof(long));
//                XChangeProperty(xcbInitializer->xlibDisplay(), window, atom, atom, 32, PropModeReplace, (unsigned char *)&decorations->hints, 5);
//                LOGD << "applyDecorations: " << remove.sequence;
                return true;
            }
            return false;
        }


        LinuxWindowActorPrivate::LinuxWindowActorPrivate() {
            ::main_logger_fprintf = &self_fprintf;
            ::main_logger_vfprintf = &self_vfprintf;
        }
    }
}
