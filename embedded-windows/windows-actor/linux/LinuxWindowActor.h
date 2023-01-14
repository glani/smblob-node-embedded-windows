#pragma once

#include <memory>
#include "BaseWindowActor.h"
#include "xdo.h"
#include "xdo_util.h"
#include "xdo_version.h"

#include <xcb/xcb.h>

#include "XcbAtom.h"

#define XCB_CONFIG_WINDOW_X_Y               (XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y)
#define XCB_CONFIG_WINDOW_WIDTH_HEIGHT      (XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT)
#define XCB_CONFIG_WINDOW_X_Y_WIDTH_HEIGHT  (XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT)
#define ROOT_EVENT_MASK     (XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_FOCUS_CHANGE)
#define CLIENT_EVENT_MASK   (XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_FOCUS_CHANGE)


#define XCB_REPLY_CONNECTION_ARG(connection, ...) connection

struct StdFreeDeleter {
    void operator()(void *p) const noexcept { return std::free(p); }
};


struct XdoFreeDeleter {
    void operator()(xdo_t* p) const noexcept {
        xdo_free(p);
    }
};
//
//template<auto F>
//struct delete_with {
//    void operator()(auto *p) {
//        F(p);
//    }
//};


namespace SMBlob {
    namespace EmbeddedWindows {

        using xdo_pointer = std::unique_ptr<xdo_t, XdoFreeDeleter>;

        class LinuxWindowActor : public BaseWindowActor {
        public:
            LinuxWindowActor();

            bool sendKeySequenceToWindow(const SMBEWEmbedWindow &window,
                                         const std::string &keySequence,
                                         uint32_t milliSecondsDelay = 1000) const override;

            bool sendFocusToWindow(const SMBEWEmbedWindow &window, bool focus = true) const override;

            bool sendNewParent(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const;

            void listen() override;

            virtual ~LinuxWindowActor();

        private:
            bool isConnected() const { return xcbConnectionValue && !xcb_connection_has_error(xcbConnectionValue); }

            xcb_connection_t *xcbConnection() const { return xcbConnectionValue; }

            inline xcb_atom_t atom(XcbAtom::Atom atom) const { return xcbAtomValue.atom(atom); }
            XcbAtom::Atom xcbAtom(xcb_atom_t atom) const { return xcbAtomValue.xcbAtom(atom); }
            xcb_atom_t internAtom(const char *name);
            std::string  atomName(xcb_atom_t atom);


        private:
            xdo_pointer xdoPtr;
            int primaryScreenNumberValue = -1;
            xcb_connection_t *xcbConnectionValue = nullptr;
            Display *xlibDisplayValue = nullptr;
            XID rootWindowValue;
            XcbAtom xcbAtomValue;
            const xcb_setup_t *setupValues;
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
