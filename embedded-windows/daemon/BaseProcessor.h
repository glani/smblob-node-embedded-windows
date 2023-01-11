//
// Created by denis on 11.01.23.
//

#ifndef SMBLOB_NODE_EMBEDDED_WINDOWS_BASEPROCESSOR_H
#define SMBLOB_NODE_EMBEDDED_WINDOWS_BASEPROCESSOR_H


namespace SMBlob {
    namespace EmbeddedWindows {

        class BaseProcessor {
        public:
            virtual void requestExit() = 0;
        protected:
            BaseProcessor() {}
            virtual ~BaseProcessor() {}




        };
    }
}


#endif //SMBLOB_NODE_EMBEDDED_WINDOWS_BASEPROCESSOR_H
