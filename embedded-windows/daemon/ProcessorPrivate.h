#pragma once

#include <memory>


namespace SMBlob {
    namespace EmbeddedWindows {

        struct ProcessorPrivateSetup {

            static ProcessorPrivateSetup fromArgs(const std::unique_ptr<char *[]> &args, size_t size);
        };

        class ProcessorPrivate {
        public:
            explicit ProcessorPrivate(const ProcessorPrivateSetup &setup);

            virtual ~ProcessorPrivate();

            void stop();

            void start();

        private:
        private:
            ProcessorPrivateSetup setup;
        };
    }
}