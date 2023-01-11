#include "ProcessorPrivate.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"



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

#ifdef _DEBUG
            if (res.pipeName.length()) {
                res.pipeName = std::string(PIPE_NAME);
            }
#endif

            return res;
        }
    }
}