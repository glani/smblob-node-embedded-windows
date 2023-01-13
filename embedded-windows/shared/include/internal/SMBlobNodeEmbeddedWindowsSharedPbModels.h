#pragma once
#include "../../src/models/embedded-windows.scheme.pb.h"

namespace SMBlob {
    namespace EmbeddedWindows {
        typedef ::SMBlob::EmbeddedWindows::Scheme::Request SMBEWRequest;
        typedef ::SMBlob::EmbeddedWindows::Scheme::Response SMBEWResponse;

        typedef ::SMBlob::EmbeddedWindows::Scheme::EmbedWindowReq SMBEWEmbedWindowReq;
        typedef ::SMBlob::EmbeddedWindows::Scheme::ReleaseWindowReq SMBEWReleaseWindowReq;
        typedef ::SMBlob::EmbeddedWindows::Scheme::CloseWindowReq SMBEWCloseWindowReq;
    }
}