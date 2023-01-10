#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "SMBlobNodeEmbeddedWindows.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool event(QEvent *event) override;
private:

    Ui::MainWindow *ui;
    std::shared_ptr<SMBlob::EmbeddedWindows::SMBlobApp> embeddedWindows;
    QAction *m_initAction;

    void releaseForeignWindowWrapper() const;
    void initForeignWindow();
};
#endif // MAINWINDOW_H
