#pragma once

#include <memory>

#ifdef LINUX
typedef unsigned long SMBEWEmbedWindow;
#else
typedef int SMBEWEmbedWindow;
#endif

namespace SMBlob {
    namespace EmbeddedWindows {
        struct FrameExtents {
            int32_t xl;
            int32_t xr;
            int32_t yt;
            int32_t yb;

            FrameExtents(int32_t v = 0): xl(v), xr(v), yt(v), yb(v) {}

            FrameExtents(int32_t xl, int32_t xr, int32_t yt, int32_t yb) : xl(xl), xr(xr), yt(yt), yb(yb) {}
        };

        struct OpaqueParameters {
            uint32_t items[8];
            uint32_t len;
            bool fullscreen;
            FrameExtents frameExtents;
        };
    }
}