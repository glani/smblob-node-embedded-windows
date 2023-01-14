#pragma once

#define 	SMBEW_CC_CALLBACK_0(__selector__, __target__,...)   std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define 	SMBEW_CC_CALLBACK_1(__selector__, __target__,...)   std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define 	SMBEW_CC_CALLBACK_2(__selector__, __target__,...)   std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define 	SMBEW_CC_CALLBACK_3(__selector__, __target__,...)   std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)


#define SMBEW_SIGNAL_TERMINATE SIGTERM

#ifdef _DEBUG
#define SMBEW_PIPE_NAME TEST_PIPENAME
#else
#ifdef _WIN32
        #define SMBEW_PIPE_NAME "\\\\?\\pipe\\snew-"
    #else
        #define SMBEW_PIPE_NAME "/tmp/snew-sock"
    #endif
#endif

#define SMBEW_LIBUV_ERR(prompt) \
    auto err = evt.code(); \
    if (err < 0) { \
        LOGE << #prompt "" << uv_err_name(err) << " str: " << uv_strerror(err);\
    }


#define SMBEW_PB_SERIALIZE(PB, VAR) \
    size_t size##VAR = PB.ByteSizeLong(); \
    std::unique_ptr<char[]> VAR = std::make_unique<char[]>(size##VAR); \
    PB.SerializeWithCachedSizesToArray((uint8_t *) VAR.get());