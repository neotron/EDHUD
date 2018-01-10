#include "Settings/Settings.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "X52ProMFD.h"
#include "LiveJournal.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow), _mfd(new X52ProMFD(this))
{
    _ui->setupUi(this);
    _ui->mfdStatus->setText("");
    _ui->mfdOutput->setText("");

    connect(_mfd, SIGNAL(mfdStatusChanged(const QString &)), _ui->mfdStatus, SLOT(setText(const QString &)));
    connect(_mfd, SIGNAL(mfdPageChanged(const QString &)), _ui->mfdOutput, SLOT(setText(const QString &)));

    auto newerThanDate = QDateTime::currentDateTime().addDays(-1);
    Journal::LiveJournal::instance()->startWatching(newerThanDate, Settings::restoreJournalPath());
    _mfd->initializeMFD();
}

MainWindow::~MainWindow()
{
    delete _ui;
}
