#include "BaseProcessor.h"
#include "SMBlobNodeEmbeddedWindowsSharedModels.h"

#include "ProcessorPrivate.h"
#include "SMBlobNodeEmbeddedWindowsShared.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedPbModels.h"
#include "uv.h"
#include "plog/Log.h"

#ifdef LINUX
#include "linux/LinuxWindowActor.h"
#endif

typedef SMBlob::EmbeddedWindows::Scheme::Request SMBEWRequest;

namespace SMBlob {
    namespace EmbeddedWindows {

        BaseProcessor::BaseProcessor() {
#ifdef LINUX
            auto *pLinuxWindowActor = new LinuxWindowActor();
            this->windowActor = std::move(std::unique_ptr<BaseWindowActor>(pLinuxWindowActor));
#endif
        }

        BaseProcessor::~BaseProcessor() {

        }

        void BaseProcessor::fillStatus(Scheme::Status *pbStatus, int status, const std::string *code,
                                       const std::string *message) {
//            holder->lastTime = uv_hrtime()
        }

        void BaseProcessor::connectApplication() {
            auto pbResponse = SMBEWResponse();
            auto app = pbResponse.mutable_connectapplication();
            auto status = app->mutable_status();
            const auto &code = std::string(SMBEW_CONNECT_DAEMON_APPLICATION);
            fillStatus(status, SMBEW_STATUS_OK, &code);
            SMBEW_PB_SERIALIZE(pbResponse, data);
            this->processorPrivate->enqueueRequest(data, sizedata);
        }


        void BaseProcessor::initApplication() {
            auto pbResponse = SMBEWResponse();
            auto app = pbResponse.mutable_initapplication();
            auto status = app->mutable_status();
            const auto &code = std::string(SMBEW_INIT_DAEMON_APPLICATION);
            fillStatus(status, SMBEW_STATUS_OK, &code);
            SMBEW_PB_SERIALIZE(pbResponse, data);
            this->processorPrivate->enqueueRequest(data, sizedata);
        }

        void BaseProcessor::request(SMBlob::EmbeddedWindows::IODataHolder holder) {
            SMBEWRequest pbRequest;
            if (!pbRequest.ParseFromArray(holder.data.get(), holder.size)) {
                LOGE << "Failed to parse request";
                return;
            }

            Scheme::Request::MessageCase messageCase = pbRequest.Message_case();
            if (messageCase == Scheme::Request::MessageCase::kEmbedWindow) {
                LOGD << "Request parsed kEmbedWindow";
                this->embedWindow(pbRequest.embedwindow());
            } else if (messageCase == Scheme::Request::MessageCase::kCloseWindow) {
                LOGD << "Request parsed kCloseWindow";
                this->closeWindow(pbRequest.closewindow());
            } else if (messageCase == Scheme::Request::MessageCase::kReleaseWindow) {
                LOGD << "Request parsed kReleaseWindow";
                this->releaseWindow(pbRequest.releasewindow());
            } else if (messageCase == Scheme::Request::MessageCase::kCloseApplication) {
                LOGD << "Request parsed kCloseApplication";
                this->requestExit();
            } else {
                LOGW << "Request parsed but not supported yet: " << messageCase;
            }
        }


    }
}