#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QWindow>
#include <QTimer>
#include <QKeyEvent>
#include <sstream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    const SMBlob::EmbeddedWindows::SMBlobApp &args = SMBlob::EmbeddedWindows::Init();
    this->embeddedWindows = std::make_shared<SMBlob::EmbeddedWindows::SMBlobApp>(args);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::event(QEvent *event) {
    return QMainWindow::event(event);
}

