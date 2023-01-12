#pragma once

#include <memory>

namespace SMBlob {
    namespace EmbeddedWindows {
        namespace Scheme {
            class Status;
        }

        class ProcessorPrivate;

        class BaseProcessor {
        public:
            virtual void requestExit() = 0;

//            std::shared_ptr<struct RequestDataHolder> getInitApplication();

            void connectApplication();

        protected:
            BaseProcessor() {}

            virtual ~BaseProcessor() {}


            ProcessorPrivate *processorPrivate;

            friend class ProcessorPrivate;

            void fillStatus(Scheme::Status *pbStatus,
                                      int status,
                                      const std::string *code = nullptr,
                                      const std::string *message = nullptr);

            void initApplication();
        };
    }
}
