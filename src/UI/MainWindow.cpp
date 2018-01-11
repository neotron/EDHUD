#include "Settings/Settings.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#ifdef Q_OS_WIN
#include "X52ProMFD.h"
#endif
#include "LiveJournal.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
#ifdef Q_OS_WIN
        , _mfd(new X52ProMFD(this))
#endif
{
    _ui->setupUi(this);


    auto newerThanDate = QDateTime::currentDateTime().addDays(-1);
    Journal::LiveJournal::instance()->startWatching(newerThanDate, Settings::restoreJournalPath());
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
