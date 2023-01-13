#include "SMBlobNodeEmbeddedWindows.h"
#include "SMBlobNodeEmbeddedWindowsShared.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedPbModels.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"
#include "ConsumerPrivate.h"

namespace SMBlob {
    namespace EmbeddedWindows {
        void fillWindow(const SMBlobWindow &window, Scheme::Window *pbWindow);


        SMBlobApp Init(const SMBlobAppInitConsumer &setup) {
            auto appPrivate = new ConsumerPrivate();

            SMBlobApp res;
            res.applicationInstance = reinterpret_cast<SMBlobEmbeddedWindowsApplicationInstance>(appPrivate);
            appPrivate->start(setup);
            return res;
        }

        void Release(SMBlobApp &app) {
            if (app.applicationInstance) {
                ConsumerPrivate *appPrivate = reinterpret_cast<ConsumerPrivate *>(app.applicationInstance);
                appPrivate->close();
            }
        }

        void EmbedWindow(SMBlobApp &app, const SMBlobWindow &window) {
            if (app.applicationInstance) {
                ConsumerPrivate *appPrivate = reinterpret_cast<ConsumerPrivate *>(app.applicationInstance);

                auto pbRequest = SMBlob::EmbeddedWindows::Scheme::Request();
                auto pbEmbedWindowReq = pbRequest.mutable_embedwindow();
                SMBlob::EmbeddedWindows::Scheme::Window *pbWindow = pbEmbedWindowReq->mutable_window();
                fillWindow(window, pbWindow);

                SMBEW_PB_SERIALIZE(pbRequest, data);
                appPrivate->enqueueRequest(data, sizedata);
            }
        }

        // private
        void fillWindow(const SMBlobWindow &window, Scheme::Window *pbWindow) {
            pbWindow->set_windowid(window.windowId);
            pbWindow->set_nativewindowid(window.nativeWindowId);
            pbWindow->set_processid(window.processId);
            pbWindow->set_windowuuid(window.windowUuid);
        }


        SMBlobAppInitConsumer::SMBlobAppInitConsumer() :
                logFileName(""), debug(false), logSeverity(-1),
                logMaxFilesize(-1), logMaxFiles(-1),
                daemonExec(""), logDaemonFilename("") {

        }
    }
}