#include "mainwindow.h"

#include <QApplication>
#include <QWindow>
#include <QScreen>
#include <QKeyEvent>
#include <QTimer>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QWidgetData>
#include <QToolBar>
#include <sstream>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    return a.exec();
}

#include "main.moc"