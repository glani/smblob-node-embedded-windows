#include "BaseWindowActor.h"

namespace SMBlob {
    namespace EmbeddedWindows {

        BaseWindowActor::BaseWindowActor() {

        }

        BaseWindowActor::~BaseWindowActor() {

        }
        
        bool BaseWindowActor::sendKeySequenceToWindow(const SMBEWEmbedWindow &window, const std::string &keySequence,
                                                      uint32_t milliSecondsDelay) const {
            return true;
        }

        bool BaseWindowActor::sendFocusToWindow(const SMBEWEmbedWindow &window, bool focus) const {
            return true;
        }

        bool BaseWindowActor::sendNewParent(const SMBEWEmbedWindow &window, const SMBEWEmbedWindow &parent) const {
            return true;
        }

        bool BaseWindowActor::setSize(const SMBEWEmbedWindow &window, int width, int height) const {
            return false;
        }

        void BaseWindowActor::listen() {

        }

        void BaseWindowActor::subscribe(const SMBEWEmbedWindow &window) {

        }

        void BaseWindowActor::startListener() {

        }

        void BaseWindowActor::setOnEmbeddedWindowDestroyed(
                const std::function<void(const SMBEWEmbedWindow &)> &onEmbeddedWindowDestroyedCallback) {

        }

        void BaseWindowActor::setOnEmbeddedWindowFocused(
                const std::function<void(const SMBEWEmbedWindow &, bool)> &onEmbeddedWindowFocusedCallback) {

        }

        void BaseWindowActor::setOnEmbeddedWindowSubscribedCallback(
                const std::function<void(const SMBEWEmbedWindow &, bool)> &onEmbeddedWindowSubscribedCallback) {

        }

        void BaseWindowActor::setOnEmbeddedWindowReparentedCallback(
                const std::function<void(const SMBEWEmbedWindow &, int)> &onEmbeddedWindowReparentedCallback) {

        }


    }
}
