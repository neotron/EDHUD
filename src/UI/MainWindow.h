#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

#ifdef Q_OS_WIN
class X52ProMFD;
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *_ui;
#ifdef Q_OS_WIN
    X52ProMFD *_mfd;
#endif
};

#endif // MAINWINDOW_H
