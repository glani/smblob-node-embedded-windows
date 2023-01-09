#pragma once

#include <string>

namespace SMBlob {
    namespace EmbeddedWindows {
        struct LibraryVersion {
            std::string version;
            std::string lastGuid;
            std::string lastDate;
        };

        struct LibraryVersion version();
    }
}
