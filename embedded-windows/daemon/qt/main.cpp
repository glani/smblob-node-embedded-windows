#include "mainwindow.h"
#include "ProcessorPrivate.h"

#include <QApplication>
#include <QWindow>
#include <QScreen>
#include <QKeyEvent>
#include <QTimer>
#include <QStringList>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <memory>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    auto list = a.arguments();
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

    // process parameters
    SMBlob::EmbeddedWindows::ProcessorPrivate process(params);
    process.start();
#ifdef _DEBUG
    MainWindow w;
    w.show();
#endif
    int ret = a.exec();
    process.stop();
    return ret;
}

#include "main.moc"