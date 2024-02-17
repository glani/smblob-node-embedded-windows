#pragma once
#include <assert.h>
#include <chrono>
#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <string>

#if defined(WINDOWS) || defined(WIN32)
#if defined(SMBLOB_NODE_EMBEDDED_WINDOWS_DLL)
#define SMBLOB_NODE_EMBEDDED_WINDOWS_EXTERN __declspec(dllexport)
#else
#define SMBLOB_NODE_EMBEDDED_WINDOWS_EXTERN
#endif
#else
#define SMBLOB_NODE_EMBEDDED_WINDOWS_EXTERN
#endif

typedef void* SMBlobEmbeddedWindowsApplicationInstance;

namespace SMBlob {
    namespace EmbeddedWindows {

        struct SMBLOB_NODE_EMBEDDED_WINDOWS_EXTERN SMBlobApp {
            SMBlobEmbeddedWindowsApplicationInstance applicationInstance;
        };

        struct SMBLOB_NODE_EMBEDDED_WINDOWS_EXTERN SMBlobWindow {
            unsigned long nativeWindowId;
            unsigned long windowId;
            std::string windowUuid;
            unsigned long processId;
        };


        struct SMBLOB_NODE_EMBEDDED_WINDOWS_EXTERN SMBlobAppInitConsumer {
            SMBlobEmbeddedWindowsApplicationInstance applicationInstance;
            std::string logFileName;
            int logSeverity;
            int logMaxFilesize;
            int logMaxFiles;
            bool debug;
            std::string daemonExec;
            std::string logDaemonFilename;

            SMBlobAppInitConsumer();
        };




        SMBLOB_NODE_EMBEDDED_WINDOWS_EXTERN SMBlobApp Init(const SMBlobAppInitConsumer& setup);
        SMBLOB_NODE_EMBEDDED_WINDOWS_EXTERN void Release(SMBlobApp& app);
        SMBLOB_NODE_EMBEDDED_WINDOWS_EXTERN void EmbedWindow(SMBlobApp& app, const SMBlobWindow& window);

    }
}