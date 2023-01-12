#include "BaseProcessor.h"
#include "ProcessorPrivate.h"
#include "SMBlobNodeEmbeddedWindowsShared.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"
#include "uv.h"


namespace SMBlob {
    namespace EmbeddedWindows {

        void BaseProcessor::fillStatus(Scheme::Status* pbStatus, int status, const std::string *code,
                                       const std::string* message) {

        }

        void BaseProcessor::connectApplication() {
            auto pbResponse = SMBlob::EmbeddedWindows::Scheme::Response();
            auto app = pbResponse.mutable_connectapplication();
            auto status = app->mutable_status();
            const auto &code = std::string(SMBEW_CONNECT_DAEMON_APPLICATION);
            fillStatus(status, SMBEW_STATUS_OK, &code);
            SMBEW_PB_SERIALIZE(pbResponse, data);
            this->processorPrivate->enqueueRequest(data, sizedata);
        }


        void BaseProcessor::initApplication() {
            auto pbResponse = SMBlob::EmbeddedWindows::Scheme::Response();
            auto app = pbResponse.mutable_initapplication();
            auto status = app->mutable_status();
            const auto &code = std::string(SMBEW_INIT_DAEMON_APPLICATION);
            fillStatus(status, SMBEW_STATUS_OK, &code);
            SMBEW_PB_SERIALIZE(pbResponse, data);
            this->processorPrivate->enqueueRequest(data, sizedata);
        }
    }
}