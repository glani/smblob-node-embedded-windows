#pragma once
#include <xcb/xcb.h>
#include <string>
#include <memory>
#include "../../../../thirdparties/xdotool-wrapper/src/xdo.h"
#include "../../../../thirdparties/xdotool-wrapper/src/xdo_util.h"
#include "../../../../thirdparties/xdotool-wrapper/src/xdo_version.h"

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

#define XCB_REPLY(call, ...)                                                                                                                         \
    std::unique_ptr<call##_reply_t, StdFreeDeleter>(call##_reply(XCB_REPLY_CONNECTION_ARG(__VA_ARGS__), call(__VA_ARGS__), nullptr))

#define XCB_REPLY_UNCHECKED(call, ...)                                                                                                               \
    std::unique_ptr<call##_reply_t, StdFreeDeleter>(call##_reply(XCB_REPLY_CONNECTION_ARG(__VA_ARGS__), call##_unchecked(__VA_ARGS__), nullptr))


struct XdoFreeDeleter {
    void operator()(xdo_t* p) const noexcept {
        xdo_free(p);
    }
};

namespace SMBlob {
    namespace EmbeddedWindows {
        using xdo_pointer = std::unique_ptr<xdo_t, XdoFreeDeleter>;

        class XcbInitializer {
        public:
            XcbInitializer();

            virtual ~XcbInitializer();

        public:
            void init();

            const xcb_setup_t *setup() const { return setupValues; }

            bool isConnected() const { return xcbConnectionValue && !xcb_connection_has_error(xcbConnectionValue); }

            xcb_connection_t *xcbConnection() const { return xcbConnectionValue; }

            static XcbInitializer &getInstance();

            XcbInitializer(XcbInitializer const &) = delete;

            void operator=(XcbInitializer const &) = delete;

            Display *xlibDisplay() const { return xlibDisplayValue; }

            XID rootWindow() const { return rootWindowValue; }

            inline xcb_atom_t atom(XcbAtom::Atom atom) const { return xcbAtomValue.atom(atom); }


            XcbAtom::Atom xcbAtom(xcb_atom_t atom) const { return xcbAtomValue.xcbAtom(atom); }

            xcb_atom_t internAtom(const char *name);

            std::string atomName(xcb_atom_t atom);

            inline xdo_t* xdo() const { return xdoPtr.get(); }

            void flush();

        private:
            xdo_pointer xdoPtr;
            int primaryScreenNumberValue = -1;
            Display *xlibDisplayValue = nullptr;
            XID rootWindowValue;
            xcb_connection_t *xcbConnectionValue = nullptr;
            XcbAtom xcbAtomValue;
            const xcb_setup_t *setupValues;

            void setWindowState(const xcb_window_t &win, uint32_t state);
        };
    }
}