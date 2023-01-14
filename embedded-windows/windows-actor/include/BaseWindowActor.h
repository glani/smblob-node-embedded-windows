#pragma once

#ifdef LINUX
typedef unsigned long SMBEWEmbedWindow;
#else
typedef int SMBEWEmbedWindow;
#endif

#include <string>

namespace SMBlob {
    namespace EmbeddedWindows {
        class BaseWindowActor {
        protected:
            BaseWindowActor();
        public:
            virtual ~BaseWindowActor();

        public:
            virtual bool sendKeySequenceToWindow(const SMBEWEmbedWindow& window,
                                                 const std::string &keySequence,
                                                 uint32_t milliSecondsDelay = 1000) const;

            virtual bool sendFocusToWindow(const SMBEWEmbedWindow &window, bool focus = true) const;

            virtual bool sendNewParent(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const;

            virtual void listen();
        };
    }
}
