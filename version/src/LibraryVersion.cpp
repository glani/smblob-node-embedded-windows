#include "LibraryVersion.h"
#include "version_generated_header.h"


namespace SMBlob {
    namespace EmbeddedWindows {
        struct LibraryVersion version() {
            return {
                    std::string(LIB_VERSION),
                    std::string(LIB_VERSION_LAST_GUID),
                    std::string(LIB_VERSION_LAST_DATE)
            };
        }
    }
}
