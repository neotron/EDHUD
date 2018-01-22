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
#ifdef Q_OS_WIN
struct ScanCommanderData : public BaseCommanderData {
    explicit ScanCommanderData(QObject *parent);

    int currentEntry{};
    QList<QStringList> history{};
};

class ScanMFDPage: public MFDPage, public CommanderState<ScanCommanderData> {
public:

    ScanMFDPage(QObject *parent, DWORD pageId);

    bool scrollWheelclick() override;

protected:
    void onEventScan(Journal::EventScan *eventScan) override;

    void onEventGeneric(Journal::Event *event) override;

private:

    void addScan(Journal::PlanetPtr planet, ScanCommanderData *data);
    void addScan(Journal::StarPtr star, ScanCommanderData *data);
};

#endif
