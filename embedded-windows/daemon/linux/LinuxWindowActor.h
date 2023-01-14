#pragma once
#include <memory>
#include "BaseWindowActor.h"

#include <condition_variable>
#include <queue>
#include <mutex>


namespace SMBlob {
    namespace EmbeddedWindows {

        class XcbInitializer;


        class LinuxWindowActor: public BaseWindowActor {
        public:
            LinuxWindowActor();
            virtual ~LinuxWindowActor();

            bool sendKeySequenceToWindow(const SMBEWEmbedWindow& window,
                                         const std::string &keySequence,
                                         uint32_t milliSecondsDelay = 1000) const override;

            bool sendFocusToWindow(const SMBEWEmbedWindow &window, bool focus = true) const override;

            bool sendNewParent(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const;

            void listen() override;
            void startListener() override;

            void subscribe(const SMBEWEmbedWindow& window) override;

            private:
                std::unique_ptr<XcbInitializer> xcbInitializer;
                std::mutex subscribeMutex;
                std::queue<struct SubscribeHolder> requestQueue;

            void tryToSubscribe();

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
