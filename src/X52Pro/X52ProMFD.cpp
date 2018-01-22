//
//  Copyright (C) 2018-  David Hedbor <neotron@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "X52ProMFD.h"
#ifdef Q_OS_WIN
#include <QDebug>
#include "deps/EDJournalQT/src/LiveJournal.h"
#include "DirectOutputCallbacks.h"
#include "ScanMFDPage.h"
#include "MaterialLogMFDPage.h"
#include "ScanValueMFDPage.h"

#define MASK(X, Y) (((X)&(Y)) == Y)
#define ROK(X) ((res = X) == S_OK)

using namespace Journal;

const DWORD kScanSummaryPage = 0;
const DWORD kMaterialLogPage = 1;
const DWORD kScanValuePage = 2;
const DWORD kHelpPage = 3;

void X52ProMFD::initializeMFD() {

    HRESULT res;
    qDebug() << "Initializing MDF";
    if(!ROK(DirectOutput_Initialize(L"EDHUD"))) {
        qDebug() << "Failed to initialize DirectOutput:" << res;
        emit mfdStatusChanged("DirectOutput init failed.");
        return;
    }

    if(!ROK(DirectOutput_RegisterDeviceCallback(DirectOutput::onDeviceChanged, this))) {
        qDebug() << "Failed to register device callback:" << res;
        emit mfdStatusChanged("Failed to register callback.");
        return;
    }
    emit mfdStatusChanged("Looking for device");
    if(!ROK(DirectOutput_Enumerate(DirectOutput::onEnumerate, this))) {
        qDebug() << "Failed to enumerate devices:"<<res;
        emit mfdStatusChanged("Enumeration failed.");
        return;
    }
}

X52ProMFD::X52ProMFD(QObject *parent) : QObject(parent) {
    createPages();
    _mfdUpdateTimer.setSingleShot(true);
    connect(&_mfdUpdateTimer, SIGNAL(timeout()), this, SLOT(updateCurrentPage()));
}

X52ProMFD::~X52ProMFD() {
    DirectOutput_Deinitialize();
}

void X52ProMFD::onEnumerate(void *device) {
    qDebug() << "Enumerate device" <<device;
    updateActiveDevice(device);
}

void X52ProMFD::updateActiveDevice(void *device) {
    if(_device) { return; }
    GUID guid = {};
    HRESULT res;
    if(!ROK(DirectOutput_GetDeviceType(device, &guid))) {
        qDebug() << "Failed to get device guid:" << res;
        return;
    }
    if(guid == DeviceType_X52Pro) {
        qDebug() << "Found active device";
        emit mfdStatusChanged("Found Device");
        configureOutputDevice(device);
    }
}

void X52ProMFD::configureOutputDevice(void *device) {
    HRESULT res;
    _device = device;
    if(!ROK(DirectOutput_RegisterPageCallback(_device, DirectOutput::onPageChange, this))) {
        qDebug() << "Failed to register page callback:"<<res;
        emit mfdStatusChanged("Failed to register page callback.");
        return;
    }
    if(!ROK(DirectOutput_RegisterSoftButtonCallback(_device, DirectOutput::onSoftButtonChange, this))) {
        qDebug() << "Failed to register soft button callback:"<<res;
        emit mfdStatusChanged("Failed to register button callback.");
        return;
    }
    addDevicePages();
    emit mfdStatusChanged("Device Active");
}

void X52ProMFD::onDeviceChanged(void *device, bool isAdded) {
    qDebug() << "Device" <<device<<"was"<<(isAdded?"added":"removed");
    if(!isAdded && device == _device) {
        _device = nullptr;
    } else {
        updateActiveDevice(device);
    }
}

void X52ProMFD::onPageChange(void *device, DWORD page, bool isSetActive) {
    qDebug() << "Page changed" << page <<"to active"<<isSetActive;
    if(isSetActive) {
        _currentPage = page;
        updateCurrentPage();
    }
}

void X52ProMFD::updateCurrentPage() {
    HRESULT res;
    if(!_pages.contains(_currentPage)) {
        return;
    }
    QStringList lines;
    for(DWORD line = 0; line < 3; line++) {
        auto str = line < _pages[_currentPage]->numLines()
                   ? _pages[_currentPage]->textForLine(line)
                   : "";
        lines.append(str);
        if(!ROK(DirectOutput_SetString(_device, _currentPage, line, static_cast<DWORD>(str.length()), str.toStdWString().c_str()))){
            qDebug() << "Failed to set line"<<line<<"on page"<<_currentPage<<"with error:"<<res;
        }
    }
    emit mfdPageChanged(lines.join('\n'));
}

void X52ProMFD::onSoftButtonChange(void *device, DWORD buttonMask) {
    if(!_pages.contains(_currentPage)) {
        return;
    }
    bool didPage = false;
    if(MASK(buttonMask, SoftButton_Up)) {
        didPage = _pages[_currentPage]->stepLine(true);
    } else if(MASK(buttonMask, SoftButton_Down)) {
        didPage = _pages[_currentPage]->stepLine(false);
    } else if(MASK(buttonMask, SoftButton_Select)) {
        if(_pages[_currentPage]->scrollWheelclick()) {
            updateCurrentPage();
        }
    }
    if(didPage) {
        updateCurrentPage();
    }
}

void X52ProMFD::addDevicePages() {
    for(auto pageId: _pages.keys()) {
        addPage(pageId, pageId == kHelpPage);
    }
    _currentPage = kHelpPage;
    updateCurrentPage();
}

void X52ProMFD::addPage(DWORD pageId, bool forceShow) const {
    HRESULT res;
    auto name = QString("%1").arg(pageId).toStdWString().c_str();
    if(!ROK(DirectOutput_AddPage(_device, pageId, name, forceShow ? FLAG_SET_AS_ACTIVE : 0))) {
        qDebug() << "Failed to add page" << pageId << ":" << res;
    }
}

void X52ProMFD::createPages() {
    _pages[kScanSummaryPage] = new ScanMFDPage(this, kScanSummaryPage);
    _pages[kScanValuePage] = new ScanValueMFDPage(this, kScanValuePage);
    _pages[kMaterialLogPage] = new MaterialLogMFDPage(this, kMaterialLogPage);
    QFile help(":/MFDHelp.txt");
    if(help.open(QIODevice::ReadOnly | QIODevice::Text)) {
        auto helpPage = new MFDPage(this, kHelpPage);
        QStringList lines(QString(help.readAll()).split("\n"));
        helpPage->setLines(lines);
        _pages[kHelpPage] = helpPage;
    }
    for(auto page: _pages.values()) {
        connect(page, SIGNAL(onChange(DWORD)), this, SLOT(pageChanged(DWORD)));
    }
}

void X52ProMFD::pageChanged(DWORD page) {
    qDebug() << "Page"<<page<<"changed. Current ="<<_currentPage;
    if(page == _currentPage) {
        _mfdUpdateTimer.start(100);
    }
}

#endif
