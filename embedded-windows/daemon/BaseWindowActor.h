#pragma once

#include "types.h"


struct SubscribeHolder {
    SMBEWEmbedWindow window;
};

#include <string>
#include <memory>
#include <functional>


namespace SMBlob {
    namespace EmbeddedWindows {
        enum ReparentReadyMask {
            NONE = 0,
            STEP1 = 1,
            READY_STEP1 = STEP1,
            STEP2 = 2,
            READY_STEP2 = STEP1 | STEP2,
            STEP3 = 4,
            READY_STEP3 = STEP1 | STEP2 | STEP3,
            READY_LAST = STEP1 | STEP2 | STEP3
        };


        class BaseWindowActor {
        protected:
            BaseWindowActor();

        public:
            virtual ~BaseWindowActor();

        public:
            // callbacks
            virtual void setOnEmbeddedWindowDestroyedCallback(
                    const std::function<void(const SMBEWEmbedWindow &)> &);

            virtual void setOnEmbeddedWindowFocusedCallback(
                    const std::function<void(const SMBEWEmbedWindow &, bool focus)> &);

            virtual void setOnEmbeddedWindowSubscribedCallback(const std::function<void(const SMBEWEmbedWindow &,
                                                                                        bool sucess)> &onEmbeddedWindowSubscribedCallback);

            virtual void setOnEmbeddedWindowReparentedCallback(const std::function<void(const SMBEWEmbedWindow &,
                                                                                        int mask)> &onEmbeddedWindowReparentedCallback);

            virtual void setOnEmbeddedWindowCustomOpaqueRequestedCallback(
                    const std::function<void(const SMBEWEmbedWindow &, FrameExtents,
                                             OpaqueParameters)> &);

            virtual void setOnEmbeddedWindowCustomOpaqueRequestedCallback(const std::function<void(const SMBEWEmbedWindow &)> &onEmbeddedWindowCustomOpaqueRequestedCallback);

            virtual bool sendKeySequenceToWindow(const SMBEWEmbedWindow &window,
                                                 const std::string &keySequence,
                                                 uint32_t milliSecondsDelay = 1000) const;

            virtual bool sendFocusToWindow(const SMBEWEmbedWindow &window, bool focus = true) const;

            virtual bool sendNewParent(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const;

            virtual bool forceUpdateSize(const SMBEWEmbedWindow &window, int width, int height) const;

            virtual bool setSize(const SMBEWEmbedWindow &window, int width, int height) const;


            virtual void listen();

            virtual void startListener();

            virtual void subscribe(const SMBEWEmbedWindow &window);

        };
    }
}
