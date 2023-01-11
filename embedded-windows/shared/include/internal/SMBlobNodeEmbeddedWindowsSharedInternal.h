#pragma once

#define 	CC_CALLBACK_0(__selector__, __target__,...)   std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define 	CC_CALLBACK_1(__selector__, __target__,...)   std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define 	CC_CALLBACK_2(__selector__, __target__,...)   std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define 	CC_CALLBACK_3(__selector__, __target__,...)   std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)


#define SIGNAL_TERMINATE SIGTERM

#ifdef _DEBUG
#define PIPE_NAME TEST_PIPENAME
#else
#ifdef _WIN32
        #define PIPE_NAME "\\\\?\\pipe\\snew-"
    #else
        #define PIPE_NAME "/tmp/snew-sock"
    #endif
#endif

#define LIBUV_ERR(prompt) \
    auto err = evt.code(); \
    if (err < 0) { \
        LOGE << #prompt "" << uv_err_name(err) << " str: " << uv_strerror(err);\
    }