#pragma once
#include "BaseWindowActor.h"

namespace SMBlob {
    namespace EmbeddedWindows {
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

        class LinuxWindowActor: public BaseWindowActor {
        public:
            LinuxWindowActor();

            virtual ~LinuxWindowActor();
        };
    }
}
