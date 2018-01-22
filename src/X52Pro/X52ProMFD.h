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
#pragma once
#include <QMap>
#ifdef Q_OS_WIN
#include <QTimer>
#include "MFDPage.h"
#include "deps/SaitekDirectOutput/DirectOutput.h"
#include "LiveJournal.h"
#include "EventDispatchObject.h"

class X52ProMFD: public QObject {
    Q_OBJECT
public:
    explicit X52ProMFD(QObject *parent = nullptr);

    ~X52ProMFD() override;

    // callbacks from the library
    void __stdcall onDeviceChanged(void *device, bool isAdded);
    void onEnumerate(void *device);
    void onPageChange(void *device, DWORD page, bool isSetActive);
    void onSoftButtonChange(void *device, DWORD buttonMask);
    void initializeMFD();

signals:
    void mfdStatusChanged(const QString &status);
    void mfdPageChanged(const QString &page);

public slots:
    void updateCurrentPage();
    void pageChanged(DWORD page);

private:

    void updateActiveDevice(void *device);

    void configureOutputDevice(void *device);

    void addDevicePages();


    void createPages();

    void *_device{};
    QMap<DWORD,MFDPage*> _pages{};
    DWORD _currentPage = 0xffffffff;
    QTimer _mfdUpdateTimer;
    QString _commander{};

    void addPage(DWORD pageId, bool forceShow) const;
};


#endif
