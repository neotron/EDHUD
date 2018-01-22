#include <QDebug>
#include "Settings/Settings.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#ifdef Q_OS_WIN
#include "X52Pro/X52ProMFD.h"
#endif

MainWindow::MainWindow(QWidget *parent) 
    : EventDispatchMainWindow(parent), _ui(new Ui::MainWindow)
#ifdef Q_OS_WIN
        , _mfd(new X52ProMFD(this))
#endif
{
    _ui->setupUi(this);

    // Only care about reasonable recent files, but since windows doesn't update dates while Elite is open, go back a lot.
    auto newerThanDate = QDateTime::currentDateTime().addDays(-1);

    auto liveJournal = Journal::LiveJournal::instance();
    liveJournal->registerHandler(this);
    liveJournal->startWatching(newerThanDate, Settings::restoreJournalPath(), 5);
#ifdef Q_OS_WIN
    _ui->mfdStatus->setText("");
    _ui->mfdOutput->setText("");
    connect(_mfd, SIGNAL(mfdStatusChanged(const QString &)), _ui->mfdStatus, SLOT(setText(const QString &)));
    connect(_mfd, SIGNAL(mfdPageChanged(const QString &)), _ui->mfdOutput, SLOT(setText(const QString &)));
    _mfd->initializeMFD();
#else
    _ui->mfdStatus->setText("Not Supported on Mac");
    _ui->mfdOutput->setText("");
#endif
}

MainWindow::~MainWindow()
{
    delete _ui;
}

void MainWindow::onEventGeneric(Journal::Event *event) {
//    qDebug() << "Got event"<<event;
//    qDebug() <<event->file();
  //  qDebug() << event->obj();
}
