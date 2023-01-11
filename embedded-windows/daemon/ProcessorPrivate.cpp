#include "ProcessorPrivate.h"


namespace SMBlob {
    namespace EmbeddedWindows {

        ProcessorPrivate::ProcessorPrivate(const ProcessorPrivateSetup &setup) : setup(setup) {

        }

        ProcessorPrivate::~ProcessorPrivate() {

        }

        void ProcessorPrivate::start() {

        }

        void ProcessorPrivate::stop() {

        }

        ProcessorPrivateSetup ProcessorPrivateSetup::fromArgs(const std::unique_ptr<char *[]> &args, size_t size) {
            ProcessorPrivateSetup res;

            return res;
        }
    }
}