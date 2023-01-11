#include "SMBlobNodeEmbeddedWindows.h"
#include "ConsumerPrivate.h"

namespace SMBlob {
    namespace EmbeddedWindows {

        SMBlobApp Init(const SMBlobAppInitConsumer& setup) {
            auto appPrivate = new ConsumerPrivate();

            SMBlobApp res;
            res.applicationInstance = reinterpret_cast<SMBlobEmbeddedWindowsApplicationInstance>(appPrivate);
            appPrivate->initLog(setup);
            appPrivate->start();
            return res;
        }

        void Release(SMBlobApp& app) {
            if (app.applicationInstance) {
                ConsumerPrivate *appPrivate = reinterpret_cast<ConsumerPrivate *>(app.applicationInstance);
                appPrivate->close();
            }
        }

    }
}