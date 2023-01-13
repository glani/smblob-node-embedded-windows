#pragma once

#include <memory>
#include "SMBlobNodeEmbeddedWindowsShared.h"
// TODO fix it with proper structure
#include "SMBlobNodeEmbeddedWindowsSharedModels.h"


namespace SMBlob {
    namespace EmbeddedWindows {
// TODO fix it with proper structure
//        class SMBEWReleaseWindowReq;
//        class SMBEWCloseWindowReq;
//        class SMBEWEmbedWindowReq;

        namespace Scheme {
            class Status;
        }

        class ProcessorPrivate;
        class BaseWindowActor;

        class BaseProcessor {
        public:
            virtual void requestExit() = 0;

            virtual void embedWindow(const SMBEWEmbedWindowReq &req) = 0;

            virtual void releaseWindow(const SMBEWReleaseWindowReq &req) = 0;

            virtual void closeWindow(const SMBEWCloseWindowReq &req) = 0;

        protected:
            BaseProcessor();

            virtual ~BaseProcessor();

        private:
            ProcessorPrivate *processorPrivate;
            std::unique_ptr<BaseWindowActor> windowActor;

            void fillStatus(Scheme::Status *pbStatus,
                                      int status,
                                      const std::string *code = nullptr,
                                      const std::string *message = nullptr);

            void initApplication();

            void connectApplication();
            void request(SMBlob::EmbeddedWindows::IODataHolder holder);

            friend class ProcessorPrivate;

        };
    }
}
