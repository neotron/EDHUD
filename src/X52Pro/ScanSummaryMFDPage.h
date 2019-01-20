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
#include "Types.h"

struct ScanSummaryCommanderData : public BaseCommanderData {
    explicit ScanSummaryCommanderData(QObject *parent);
    bool hasBodies{};
    int64_t systemValue{};
    uint8_t tf{};
    uint8_t wwtf{};
    uint8_t ww{};
    uint8_t elw{};
    uint8_t aw{};
    int64_t lastValue{};
    QString hz;
    QMap<QString,int64_t> scannedBodies;
};

class ScanSummaryMFDPage: public MFDPage, public CommanderState<ScanSummaryCommanderData> {
    Q_OBJECT
public:

    ScanSummaryMFDPage(QObject *parent, DWORD pageId);

    bool scrollWheelclick() override;
protected:

    void onEventScan(Journal::EventScan *scan) override;

    void onEventFSDJump(Journal::EventFSDJump *jump) override;

    void onEventSAAScanComplete(Journal::EventSAAScanComplete *event) override;

    void onEventStartJump(Journal::EventStartJump *jump) override;

    void updateLines(const ScanSummaryCommanderData *data);

    void addScan(Journal::PlanetPtr planet, ScanSummaryCommanderData *data);

    void addScan(Journal::StarPtr star, ScanSummaryCommanderData *data);

    QString format(int64_t value);
};

