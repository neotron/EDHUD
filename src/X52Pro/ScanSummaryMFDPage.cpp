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
#include <Events/EventStartJump.h>
#include "ScanSummaryMFDPage.h"
using namespace Journal;

ScanSummaryMFDPage::ScanSummaryMFDPage(QObject *parent, DWORD pageId)
    : MFDPage(parent, pageId) {
}

bool ScanSummaryMFDPage::scrollWheelclick() {
    auto data = dataForCommander(_commander, this);
    if(!data) { return false; }
    updateLines(data);
    return true;
}


void ScanSummaryMFDPage::onEventScan(Journal::EventScan *scan) {
    auto data = dataForEvent(scan, this);
    if(!data) { return; }
    data->lastValue = scan->estimatedValue();

    switch(scan->bodyType()) {
        case Body::Planet:
            if(data->scannedBodies.contains(scan->planet()->bodyName())) {
                return;
            }
            addScan(scan->planet(), data);
            break;
        case Body::Star:
            // Only use first star for habitable zone checking.
            if(!data->hasBodies && !scan->star()->bodyId()) {
                addScan(scan->star(), data);
                data->hasBodies = true;
            }
            break;
        default:
            break; // No-op
    }
    data->systemValue += data->lastValue;
    updateLines(data);
    notifyChange();
}

void ScanSummaryMFDPage::onEventFSDJump(Journal::EventFSDJump *jump) {
    auto data = dataForEvent(jump, this);
    if(!data) { return; }
    data->hasBodies = false;
    data->tf = data->wwtf = data->ww = data->elw = data->aw = 0;
    data->systemValue = 0;
    data->scannedBodies.clear();
    _lines.clear();
    _lines.append("Arrived in:");
    _lines.append(jump->file()->system());
    _lines.append("Pending scan...");
    _currentLine = 0;
    notifyChange();
}

void ScanSummaryMFDPage::updateLines(const ScanSummaryCommanderData *data) {
    _lines.clear();
    _currentLine = 0;
    if(data->hasBodies) {
        _lines += data->hz;
        _lines += QString("System: %1").arg(format(data->systemValue));
        QString valuables;
        if(data->elw) {
            valuables += QString("E%1 ").arg(data->elw);
        }
        if(data->ww || data->wwtf) {
            valuables += "W";
            if(data->ww) {
                valuables += QString("%1").arg(data->ww);
            }
            if(data->wwtf) {
                valuables += QString("T%1").arg(data->wwtf);
            }
            valuables += " ";
        }
        if(data->aw) {
            valuables += QString("A%1 ").arg(data->aw);
        }
        if(data->tf) {
            valuables += QString("T%1").arg(data->tf);
        }
        _lines += valuables;
    } else {
        _lines += QString("Pending scan...");
    }
    qDebug() << _lines;
}

QString ScanSummaryMFDPage::format(int64_t value) {
    if(value < 1000000) {
        return QString("%1").arg(value);
    } else {
        return QString("%1M").arg(value/1000000.0, 5, 'f', 2);
    }
}

void ScanSummaryMFDPage::addScan(Journal::PlanetPtr planet, ScanSummaryCommanderData *data) {
    data->scannedBodies[planet->bodyName()] = data->lastValue;
    switch(planet->type()) {
        case Planet::Earthlike:
            data->elw ++;
            break;
        case Planet::WaterWorld:
            if(planet->isTerraformable()) {
                data->wwtf ++;
            } else {
                data->ww++;
            }
            break;
        case Planet::AmmoniaWorld:
            data->aw++;
            break;
        default:
            if(planet->isTerraformable()) {
                data->tf++;
            }
            break;
    }
}

void ScanSummaryMFDPage::addScan(Journal::StarPtr star, ScanSummaryCommanderData *data) {
    auto habZone = star->habitableZone();
    if(habZone.isValid()) {
        data->hz = QString("HZ %1~%2 ls").arg(FMTK(habZone.innerLS())).arg(FMTK(habZone.outerLS()));
    } else {
        data->hz = QString("HZ Unknown...");
    }

}

void ScanSummaryMFDPage::onEventSAAScanComplete(EventSAAScanComplete *event) {
    auto data = dataForEvent(event, this);
    if(!data) { return; }
    const auto bonusValue = data->scannedBodies[event->bodyName()] * 3;
    qDebug() << "SAA scan" << event->bodyName() << bonusValue;
    data->systemValue += bonusValue; // estimated 3x additional value.
    updateLines(data);
    notifyChange();

}

void ScanSummaryMFDPage::onEventStartJump(EventStartJump *event) {
    if(event->string("JumpType") == "Hyperspace") {
        _currentLine = 0;
        _lines.clear();
        _lines.append("Jumping to");
        _lines.append(event->string("StarSystem"));
        _lines.append("Class " + event->string("StarClass"));
        notifyChange();
    }
}

ScanSummaryCommanderData::ScanSummaryCommanderData(QObject *parent)
    : BaseCommanderData(parent) {
}
