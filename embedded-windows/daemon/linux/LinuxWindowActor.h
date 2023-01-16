#pragma once

#include <memory>
#include "BaseWindowActor.h"

#include <condition_variable>
#include <queue>
#include <mutex>
#include <xcb/xproto.h>


namespace SMBlob {
    namespace EmbeddedWindows {

        class XcbInitializer;


        class LinuxWindowActor : public BaseWindowActor {
        public:
            LinuxWindowActor();

            virtual ~LinuxWindowActor();

            bool sendKeySequenceToWindow(const SMBEWEmbedWindow &window,
                                         const std::string &keySequence,
                                         uint32_t milliSecondsDelay = 1000) const override;

            bool sendFocusToWindow(const SMBEWEmbedWindow &window, bool focus = true) const override;

            bool setNewParent(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const;

            bool setSize(const SMBEWEmbedWindow &window, int width, int height) const;

            bool forceUpdateSize(const SMBEWEmbedWindow &window, int width, int height) const;

            virtual bool validateWindowEquality(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &windowToCompare) const;

            void listen() override;

            void startListener() override;

            void subscribe(const SMBEWEmbedWindow &window) override;

            bool closeWindow(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const override;

            bool closeWindowGracefully(const SMBEWEmbedWindow &window) const override;

            // callbacks
            void setOnEmbeddedWindowDestroyedCallback(
                    const std::function<void(const SMBEWEmbedWindow &)> &onEmbeddedWindowDestroyedCallback) override;

            void setOnEmbeddedWindowFocusedCallback(const std::function<void(const SMBEWEmbedWindow &,
                                                                             bool focus)> &onEmbeddedWindowFocusedCallback) override;

            void setOnEmbeddedWindowSubscribedCallback(const std::function<void(const SMBEWEmbedWindow &,
                                                                                bool success)> &onEmbeddedWindowSubscribedCallback) override;

            void setOnEmbeddedWindowReparentedCallback(
                    const std::function<void(const SMBEWEmbedWindow &, const SMBEWEmbedWindow &,
                                             int mask)> &onEmbeddedWindowReparentedCallback) override;

            void setOnEmbeddedWindowCustomOpaqueRequestedCallback(const std::function<void(
                    const SMBEWEmbedWindow &)> &onEmbeddedWindowCustomOpaqueRequestedCallback) override;

            std::shared_ptr<OpaqueParameters> getOpaqueParameters(const SMBEWEmbedWindow &window) const override;

        private:
            // callbacks variables
            std::function<void(const SMBEWEmbedWindow &)> onEmbeddedWindowDestroyedCallback;
            std::function<void(const SMBEWEmbedWindow &, bool focus)> onEmbeddedWindowFocusedCallback;
            std::function<void(const SMBEWEmbedWindow &, bool success)> onEmbeddedWindowSubscribedCallback;
            std::function<void(const SMBEWEmbedWindow &, const SMBEWEmbedWindow &,
                               int mask)> onEmbeddedWindowReparentedCallback;
            std::function<void(const SMBEWEmbedWindow &)> onEmbeddedWindowCustomOpaqueRequestedCallback;
            //
            std::unique_ptr<XcbInitializer> xcbInitializer;
            std::mutex subscribeMutex;
            std::queue<struct SubscribeHolder> requestQueue;

            void tryToSubscribe();

            void subscribeWindow(const SMBEWEmbedWindow &window);

            void focusInEvent(xcb_focus_in_event_t *pEvent);

            void focusOutEvent(xcb_focus_out_event_t *pEvent);

            void destroyEvent(xcb_destroy_notify_event_t *pEvent);

            void notifyEvent(xcb_property_notify_event_t *pEvent);

            void unmapNotifyEvent(xcb_unmap_notify_event_t *pEvent);

            void mapNotifyEvent(xcb_map_notify_event_t *pEvent);

            void reparentNotifyEvent(xcb_reparent_notify_event_t *pEvent);

            xcb_atom_t kdeNetWmFrameStrut;
            xcb_atom_t gtkFrameExtents;
            xcb_atom_t netFrameExtents;
            xcb_atom_t netWmOpaqueRegion;

            std::shared_ptr<FrameExtents> retrieveFrameExtents(xcb_window_t window) const;

            std::shared_ptr<FrameExtents> getExtents(xcb_window_t window, xcb_atom_t atom) const;


        };

        // LinuxWindowActorPrivate
        class LinuxWindowActorPrivate {
        public:
            static LinuxWindowActorPrivate &getInstance() {
                static LinuxWindowActorPrivate instance; // Guaranteed to be destroyed.
                // Instantiated on first use.
                return instance;
            }

        private:
            LinuxWindowActorPrivate();

        public:
            LinuxWindowActorPrivate(LinuxWindowActorPrivate const &) = delete;

            void operator=(LinuxWindowActorPrivate const &) = delete;
        };
    }
}
