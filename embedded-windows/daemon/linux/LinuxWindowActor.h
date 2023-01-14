#pragma once
#include <memory>
#include "BaseWindowActor.h"
#include "xdo.h"
#include "xdo_util.h"
#include "xdo_version.h"

template <auto F>
struct delete_with {
    void operator()(auto* p) {
        F(p);
    }
};

namespace SMBlob {
    namespace EmbeddedWindows {

        using xdo_pointer = std::unique_ptr<xdo_t, delete_with<xdo_free>>;

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
                xdo_pointer xdoPtr;
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
