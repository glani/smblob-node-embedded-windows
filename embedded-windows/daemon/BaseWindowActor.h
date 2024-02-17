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
            STEP_UNMAP = 1,
            READY_STEP1 = STEP_UNMAP,
            STEP_REPARENT = 2,
            READY_STEP2 = STEP_UNMAP | STEP_REPARENT,
            STEP_MAP = 4,
            READY_STEP3 = STEP_UNMAP | STEP_REPARENT | STEP_MAP,
            READY_LAST = STEP_UNMAP | STEP_REPARENT | STEP_MAP
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

            virtual void setOnEmbeddedWindowReparentedCallback(
                    const std::function<void(const SMBEWEmbedWindow &, const SMBEWEmbedWindow &,
                                             int mask)> &onEmbeddedWindowReparentedCallback);

            virtual void setOnEmbeddedWindowCustomOpaqueRequestedCallback(
                    const std::function<void(const SMBEWEmbedWindow &)> &onEmbeddedWindowCustomOpaqueRequestedCallback);

            virtual bool sendKeySequenceToWindow(const SMBEWEmbedWindow &window,
                                                 const std::string &keySequence,
                                                 uint32_t milliSecondsDelay = 1000) const;

            virtual bool sendFocusToWindow(const SMBEWEmbedWindow &window, bool focus = true) const;

            virtual bool setNewParent(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const;

            virtual bool validateWindowEquality(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &windowToCompare) const;

            virtual bool closeWindow(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const;

            virtual bool closeWindowGracefully(const SMBEWEmbedWindow &window) const;

            virtual bool forceUpdateSize(const SMBEWEmbedWindow &window, int width, int height) const;

            virtual bool setSize(const SMBEWEmbedWindow &window, int width, int height) const;

            virtual std::shared_ptr<OpaqueParameters> getOpaqueParameters(const SMBEWEmbedWindow &window) const;

            virtual std::unique_ptr<SMBEWEmbedWindowDecorations> removeDecorations(const SMBEWEmbedWindow &window, const std::unique_ptr<SMBEWEmbedWindowDecorations>& newDecorations = nullptr) const;

            virtual void listen();
            virtual void startListener();

            virtual void subscribe(const SMBEWEmbedWindow &window);


        };
    }
}
