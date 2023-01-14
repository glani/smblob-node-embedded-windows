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
    }
}
