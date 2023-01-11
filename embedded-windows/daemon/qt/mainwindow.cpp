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
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::event(QEvent *event) {
    return QMainWindow::event(event);
}

