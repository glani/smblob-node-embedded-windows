#pragma once

#define SMBEW_STATUS_OK 0
#define SMBEW_STATUS_FAIL 1
#define SMBEW_CONNECT_DAEMON_APPLICATION "connect_daemon_application"
#define SMBEW_INIT_DAEMON_APPLICATION "init_daemon_application"

namespace SMBlob {
    namespace EmbeddedWindows {

        struct IODataHolder {
            std::unique_ptr<char[]> data;
            size_t size;
        };
    }
}