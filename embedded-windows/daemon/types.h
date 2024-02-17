#pragma once



#ifdef LINUX
#include <stdint.h>
typedef unsigned long SMBEWEmbedWindow;
#define SMBEWEmbedWindowNull (SMBEWEmbedWindow)0

#else
typedef int SMBEWEmbedWindow;
#endif


namespace SMBlob {
    namespace EmbeddedWindows {
#ifdef LINUX
        struct MotifHints {
            uint32_t flags;
            uint32_t functions;
            uint32_t decorations;
            int32_t inputMode;
            uint32_t status;

            MotifHints(): flags(0L), functions(0L), decorations(0), inputMode(0L), status(0L) {}
        };

        struct SMBEWEmbedWindowDecorationsStruct {
            MotifHints hints;

            SMBEWEmbedWindowDecorationsStruct(): hints({})
            {}
        };
#endif

        typedef SMBEWEmbedWindowDecorationsStruct SMBEWEmbedWindowDecorations;
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