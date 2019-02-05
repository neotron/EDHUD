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

#include <Events/EventScan.h>
#include <Events/EventFSDJump.h>
#include <Events/EventSAAScanComplete.h>
#include "ScanValueMFDPage.h"

ScanValueMFDPage::ScanValueMFDPage(QObject *parent, DWORD pageId)
    : MFDPage(parent, pageId) {
}

bool ScanValueMFDPage::scrollWheelclick() {
    auto data = dataForCommander(_commander, this);
    if(!data) { return false; }
    data->totalValue = 0;
    updateLines(data);
    return true;
}

void ScanValueMFDPage::onEventScan(Journal::EventScan *scan) {
    auto data = dataForEvent(scan, this);
    if(!data) { return; }
    auto value(scan->estimatedValue());
    switch(scan->bodyType()) {
        case Journal::Body::Planet:
            if(data->scannedBodies.contains(scan->planet()->bodyName())) {
                return;
            }
            data->scannedBodies[scan->planet()->bodyName()] = value;
            break;
        default:
            break;

    }
    data->lastValue = value;
    data->systemValue += data->lastValue;
    data->totalValue += data->lastValue;
    updateLines(data);
    notifyChange();
}

void ScanValueMFDPage::onEventFSDJump(Journal::EventFSDJump *jump) {
    auto data = dataForEvent(jump, this);
    if(!data) { return; }
    data->scannedBodies.clear();
    data->systemValue = 0;
    updateLines(data);
    notifyChange();
}

void ScanValueMFDPage::updateLines(const ScanValueCommanderData *data) {
    _lines.clear();
    _currentLine = 0;
    _lines += QString("Total: %1").arg(format(data->totalValue));
    _lines += QString("System: %1").arg(format(data->systemValue));
    _lines += QString("Last: %1").arg(data->lastValue);
    qDebug() << _lines;
}

QString ScanValueMFDPage::format(int64_t value) {
    if(value < 1000000) {
        return QString("%1").arg(value);
    } else {
        return QString("%1M").arg(value/1000000.0, 5, 'f', 2);
    }
}

void ScanValueMFDPage::onEventSAAScanComplete(Journal::EventSAAScanComplete *event) {
    auto data = dataForEvent(event, this);
    if(!data) { return; }
    const auto bonusValue = data->scannedBodies[event->bodyName()] * 3;
    data->systemValue += bonusValue; // estimated 3x additional value.
    updateLines(data);
    notifyChange();

}

ScanValueCommanderData::ScanValueCommanderData(QObject *parent)
    : BaseCommanderData(parent) {
}
