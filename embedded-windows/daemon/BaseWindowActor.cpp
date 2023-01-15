#include "BaseWindowActor.h"

namespace SMBlob {
    namespace EmbeddedWindows {

        BaseWindowActor::BaseWindowActor() {

        }

        BaseWindowActor::~BaseWindowActor() {

        }


        bool BaseWindowActor::forceUpdateSize(const SMBEWEmbedWindow &window, int width, int height) const {
            return true;
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

        void BaseWindowActor::setOnEmbeddedWindowDestroyedCallback(
                const std::function<void(const SMBEWEmbedWindow &)> &) {

        }

        void BaseWindowActor::setOnEmbeddedWindowFocusedCallback(
                const std::function<void(const SMBEWEmbedWindow &, bool)> &) {

        }

        void BaseWindowActor::setOnEmbeddedWindowSubscribedCallback(
                const std::function<void(const SMBEWEmbedWindow &, bool)> &) {

        }

        void BaseWindowActor::setOnEmbeddedWindowReparentedCallback(
                const std::function<void(const SMBEWEmbedWindow &, int)> &) {

        }

        void BaseWindowActor::setOnEmbeddedWindowCustomOpaqueRequestedCallback(
                const std::function<void(const SMBEWEmbedWindow &, FrameExtents,
                                         OpaqueParameters)> &) {

        }

        void BaseWindowActor::setOnEmbeddedWindowCustomOpaqueRequestedCallback(
                const std::function<void(const SMBEWEmbedWindow &)> &onEmbeddedWindowCustomOpaqueRequestedCallback) {

        }

    }
}
