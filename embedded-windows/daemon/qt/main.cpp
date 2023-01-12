#include "mainwindow.h"
#include "ProcessorPrivate.h"
#include "qt/application.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"

#include <QStringList>
#include <memory>


int main(int argc, char *argv[]) {
    Application application(argc, argv);
    auto list = application.arguments();
    int i = 0;
    SMBlob::EmbeddedWindows::ProcessorPrivateSetup params;
    if (list.length() > 1) {
        qsizetype size = list.length() - 1;
        std::unique_ptr<char *[]> args = std::unique_ptr<char *[]>(new char *[size]);
                foreach (const QString &str, list) {
                if (i != 0) {
                    args[i - 1] = (char *) str.data();
                }
                i++;
            }
        params = SMBlob::EmbeddedWindows::ProcessorPrivateSetup::fromArgs(args, size);
    }

#ifdef _DEBUG
    if (params.pipeName.length() == 0) {
        params.pipeName = std::string(PIPE_NAME);
    }
#endif

    // process parameters
    SMBlob::EmbeddedWindows::ProcessorPrivate process(params, &application);
    process.start();
#ifdef _DEBUG
    MainWindow w;
    w.show();
#endif
    int ret = Application::exec();
    process.stop();
    return ret;
}

#include "main.moc"