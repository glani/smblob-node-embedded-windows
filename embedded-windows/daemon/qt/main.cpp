#include "mainwindow.h"
#include "ProcessorPrivate.h"
#include "qt/application.h"
#include "internal/SMBlobNodeEmbeddedWindowsSharedInternal.h"

#include <QStringList>
#include <memory>
#include <fstream>


int main(int argc, char *argv[]) {
    Application application(argc, argv);
    auto list = application.arguments();
    int i = 0;
    SMBlob::EmbeddedWindows::ProcessorPrivateSetup params;
    if (argv && argc > 1) {
        params = SMBlob::EmbeddedWindows::ProcessorPrivateSetup::fromArgs(argc, argv);
    }

#ifdef _DEBUG
    if (params.pipeName.length() == 0) {
        params.pipeName = std::string(SMBEW_PIPE_NAME);
    }
    if (params.logSeverity == 0) {
        params.logSeverity = 5; // DEBUG
    }
    std::ofstream fout("filename.log");
    fout << params.pipeName << std::endl;
    fout << params.logSeverity << std::endl;
    fout << params.logFileName << std::endl;
            foreach (const QString &str, list) {
            fout << str.toStdString().c_str() << std::endl;
        }
#endif

    // process parameters
    SMBlob::EmbeddedWindows::ProcessorPrivate process(params, &application);
    process.start();
#ifdef _DEBUG
    MainWindow w;
    w.show();
#endif
    process.initApplication();
    int ret = Application::exec();
    process.stop();
    return ret;
}

#include "main.moc"