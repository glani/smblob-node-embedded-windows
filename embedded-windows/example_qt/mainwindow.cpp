#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QWindow>
#include <QToolBar>
#include <QTimer>
#include <QKeyEvent>
#include <sstream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMenu *fileMenu = menuBar()->addMenu("File");
    fileMenu->setObjectName("FileMenu");
    QToolBar *toolbar = new QToolBar;
    addToolBar(Qt::TopToolBarArea, toolbar);

    m_initAction = fileMenu->addAction("Init external System", this, &MainWindow::initForeignWindow);
    toolbar->addAction(m_initAction);
}


void MainWindow::initForeignWindow() {
    if (this->embeddedWindows) {
        SMBlob::EmbeddedWindows::Release(*this->embeddedWindows);
    }

    const SMBlob::EmbeddedWindows::SMBlobApp &args = SMBlob::EmbeddedWindows::Init();
    this->embeddedWindows = std::make_shared<SMBlob::EmbeddedWindows::SMBlobApp>(args);
}

MainWindow::~MainWindow()
{
    releaseForeignWindowWrapper();
    delete ui;
}

void MainWindow::releaseForeignWindowWrapper() const {
    if (embeddedWindows) {
        SMBlob::EmbeddedWindows::Release(*embeddedWindows);
    }
}

bool MainWindow::event(QEvent *event) {
    return QMainWindow::event(event);
}

