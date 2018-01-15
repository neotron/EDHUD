#pragma once

#include <QMainWindow>
#include "EventDispatch.h"

namespace Ui {
class MainWindow;
}

#ifdef Q_OS_WIN
class X52ProMFD;
#endif

class MainWindow : public Journal::EventDispatchMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow() override;

protected:
    void onEventGeneric(Journal::Event *event) override;

private:
    Ui::MainWindow *_ui;
#ifdef Q_OS_WIN
    X52ProMFD *_mfd;
#endif
};
