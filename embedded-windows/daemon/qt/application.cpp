#include "application.h"
#include "plog/Log.h"
#include <QThread>

Application::Application(int &argc, char **argv): QApplication(argc, argv, ApplicationFlags),
    SMBlob::EmbeddedWindows::BaseProcessor()
{
    QObject::connect(&proxyObject, SIGNAL(exitRequested()), this, SLOT(exitRequested()));
}


Application::~Application() {

}

void Application::requestExit() {
    emit proxyObject.exitRequested();
}

void Application::exitRequested() {
    LOG_DEBUG << "Application::exitRequested";
    this->quit();
    this->closeAllWindows();
}

ProxyObject::ProxyObject(QObject *parent) : QObject(parent) {

}
