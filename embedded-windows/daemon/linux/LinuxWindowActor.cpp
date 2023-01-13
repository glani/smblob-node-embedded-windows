#include "LinuxWindowActor.h"
#include "xdo.h"
#include "xdo_util.h"
#include "xdo_version.h"

int self_fprintf(FILE *__restrict __stream,
                      const char *__restrict __format, ...) {
    return 0;
}
int self_vfprintf(FILE *__restrict __s, const char *__restrict __format,
                       __gnuc_va_list __arg) {

}


namespace SMBlob {
    namespace EmbeddedWindows {

        LinuxWindowActor::LinuxWindowActor(): BaseWindowActor()  {
            LinuxWindowActorPrivate::getInstance();
        }

        LinuxWindowActor::~LinuxWindowActor() {

        }



        LinuxWindowActorPrivate::LinuxWindowActorPrivate() {
            ::main_logger_fprintf = &self_fprintf;
            ::main_logger_vfprintf = &self_vfprintf;
        }

    }
}
