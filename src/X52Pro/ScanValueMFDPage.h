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

#include "MFDPage.h"
struct ScanValueCommanderData : public BaseCommanderData {
    explicit ScanValueCommanderData(QObject *parent);

    int64_t totalValue{};
    int64_t systemValue{};
    int64_t lastValue{};
    QMap<QString,int64_t> scannedBodies;
};

class ScanValueMFDPage: public MFDPage, public CommanderState<ScanValueCommanderData> {
    Q_OBJECT
public:

    ScanValueMFDPage(QObject *parent, DWORD pageId);

    bool scrollWheelclick() override;
protected:

    void onEventScan(Journal::EventScan *scan) override;

    void onEventFSDJump(Journal::EventFSDJump *jump) override;

    void onEventSAAScanComplete(Journal::EventSAAScanComplete *complete) override;

    void updateLines(const ScanValueCommanderData *data);

    QString format(int64_t value);
};

