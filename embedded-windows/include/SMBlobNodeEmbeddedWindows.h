#pragma once
#include <assert.h>
#include <chrono>
#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

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


        SMBLOB_NODE_EMBEDDED_WINDOWS_EXTERN SMBlobApp Init();
        SMBLOB_NODE_EMBEDDED_WINDOWS_EXTERN void Release(SMBlobApp& app);

    }
}