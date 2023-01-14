#pragma once
#include <memory>
#include "BaseWindowActor.h"


namespace SMBlob {
    namespace EmbeddedWindows {

        class XcbInitializer;


        class LinuxWindowActor: public BaseWindowActor {
        public:
            LinuxWindowActor();
            bool sendKeySequenceToWindow(const SMBEWEmbedWindow& window,
                                         const std::string &keySequence,
                                         uint32_t milliSecondsDelay = 1000) const override;

            bool sendFocusToWindow(const SMBEWEmbedWindow &window, bool focus = true) const override;

            bool sendNewParent(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const;

            virtual ~LinuxWindowActor();
            private:
                std::unique_ptr<XcbInitializer> xcbInitializer;
        };

        // LinuxWindowActorPrivate
        class LinuxWindowActorPrivate
        {
        public:
            static LinuxWindowActorPrivate& getInstance()
            {
                static LinuxWindowActorPrivate instance; // Guaranteed to be destroyed.
                // Instantiated on first use.
                return instance;
            }
        private:
            LinuxWindowActorPrivate();
        public:
            LinuxWindowActorPrivate(LinuxWindowActorPrivate const&)               = delete;
            void operator=(LinuxWindowActorPrivate const&)  = delete;
        };
    }
}
