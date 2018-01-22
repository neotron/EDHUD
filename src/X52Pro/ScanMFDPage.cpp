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

#include "ScanMFDPage.h"
#ifdef Q_OS_WIN
#include <QDebug>
#include "Events.h"
#include "JFile.h"
using namespace Journal;

#define FMTK(X) ((X) < 10000 ? QString::number(X) : QString("%1k").arg(round((X)/1000.0)))



void ScanMFDPage::onEventGeneric(Event *event) {
    auto data = dataForEvent(event, this);
    if(!data) { return; }
    switch(event->journalEvent()) {
        case Event::StartJump:
            if(event->string("JumpType") == "Hyperspace") {
                _currentLine = 0;
                _lines.clear();
                _lines.append("Jumping to");
                _lines.append(event->string("StarSystem"));
                _lines.append("Class " + event->string("StarClass"));
                notifyChange();
            }
            break;

        case Event::FSDJump:
            _lines.clear();
            _lines.append("Arrived in:");
            _lines.append(event->file()->system());
            _lines.append("Pending scan...");
            data->history.clear();
            data->currentEntry = 0;
            _currentLine = 0;
            notifyChange();
            break;
        default:
            break;
    }
}

void ScanMFDPage::onEventScan(EventScan *eventScan) {
    auto data = dataForEvent(eventScan, this);
    if(!data) { return; }
    _lines.clear();
    switch(eventScan->bodyType()) {
        case Body::Planet:
            addScan(eventScan->planet(), data);
            break;
        case Body::Star:
            addScan(eventScan->star(), data);
            break;
        default:
            _lines += eventScan->string("BodyName");
            _lines += "Simple Body";
            _lines += "No Addl. Data";
            break; // No-op
    }
    _currentLine = _lines.count() > 3 ? 1 : 0; // Hide system name if we hav more than 3 lines
    qDebug() << "Added scan for commander " << data->commander <<eventScan->file()->commander();
    data->history.append(_lines);
    data->currentEntry = data->history.size() - 1;
    notifyChange();
}

ScanMFDPage::ScanMFDPage(QObject *parent, DWORD pageId)
    : MFDPage(parent, pageId) {
    _lines.append(QString("Scan Summary\n\nPending Scan").split("\n"));
}

bool ScanMFDPage::scrollWheelclick() {
    auto data = dataForCommander(_commander, this);
    if(!data) {
        qDebug() << "No data?";
        return false;
    }
    if(data->history.size() <= 1) {
        return false;
    }
    data->currentEntry++;
    if(data->currentEntry >= data->history.size()) {
        data->currentEntry = 0;
    }
    _currentLine = 0;
    _lines = data->history[data->currentEntry];
    return true;
}

void ScanMFDPage::addScan(Journal::PlanetPtr planet, ScanCommanderData *data) {
    auto gravity = planet->surfaceGravity();
    auto numDigits = 2;
    if(gravity < 1.0) { --numDigits; }
    if(gravity > 99) { --numDigits; }

    _lines += planet->bodyName();
    QString tmp;
    QString atmStr;
    switch(planet->atmosphereType()) {
        case Atmosphere::Unknown: // Gas giants etc
        {
            _lines += planet->typeNameMedium();
            auto massEM = planet->mass();
            auto massStr = massEM < 10 ? QString::number(massEM, 'f', 1) : QString::number(massEM, 'f', 0);
            auto radius = planet->radius();
            QString radiusStr;
            if(radius < 1000) {
                radiusStr = QString("%1m").arg(static_cast<int>(radius));
            } else if(radius < 1000000) {
                radiusStr = QString("%1km").arg(static_cast<int>(radius / 1000));
            } else if(radius < 1000000000) {
                radiusStr = QString("%1Mm").arg(static_cast<int>(radius / 1000000));
            }
            // 1234567890123456
            // 9.99g M172.2 R1.2
            // 11g M155k R74Mm
            // 11g M1535 R74Mm
            _lines += QString("%1g M%2 R%3").arg(static_cast<int>(gravity)).arg(massStr).arg(radiusStr);
        }
            break;
        default: // normal planets with atmospheres.
            tmp = planet->typeNameShort();
            if(planet->isTerraformable()) {
                tmp += " TF";
            }
            tmp += QString(", %1g").arg(QString::number(gravity, 'f', numDigits));
            _lines += tmp;
            if(planet->landable()) {
                atmStr = "Landable";
            } else if(planet->type() != Planet::Earthlike) {
                // Add atmosphere, if it's not an ELW - ELW just adds "Earth Like" which is kind of pointless.
                atmStr = planet->atmosphereShort();
            }
    }
    auto temp = planet->surfaceTemperature();
    auto pressure = planet->surfacePressureBar();
    tmp = QString("%1K").arg(QString::number(temp, temp >= 1000 ? 'g' : 'f', 0));
    if(pressure > 0) {
        auto digits = pressure > 1000 ? 2 : 1;
        auto pressureStr = QString(" %1atm").arg(QString::number(pressure, pressure > 1000 ? 'g' : 'f', digits));
        tmp = QString("%1 %2").arg(tmp, -6).arg(pressureStr, -9);
    }
    _lines += tmp;
    _lines += QString("Value: %1").arg(planet->estimatedValue());
    if(!atmStr.isEmpty()) {
        _lines += atmStr;
    }
    qDebug()<<"New planet"<<_lines;
}

void ScanMFDPage::addScan(Journal::StarPtr star, ScanCommanderData *data) {
    auto lum = star->luminosity();
    auto line2 = star->typeNameMedium() + (lum.isEmpty() ? "" : " " + lum);
    if(line2.length() <= 9) {
        line2 = "Star: " + line2;
    }

    auto surfaceTemp = star->surfaceTemperature();
    auto temp = QString("%1K").arg(surfaceTemp > 999999
                                   ? QString::number(surfaceTemp * 1.0, 'g', 3).replace("+", "")
                                   : QString::number(surfaceTemp));
    auto massStr = QString::number(star->stellarMass(), 'f', 1);
    auto radius = star->radiusSol();
    auto radiusStr = radius < 0.1 ? "0" : QString::number(radius, 'f', 1);

    _lines += star->bodyName();
    _lines += line2;
    auto habZone = star->habitableZone();
    if(habZone.isValid()) {
        _lines += QString("HZ %1~%2 ls").arg(FMTK(habZone.innerLS())).arg(FMTK(habZone.outerLS()));
    }
    _lines += QString("%1 M%2 R%3").arg(temp).arg(massStr).arg(radiusStr);
    auto valueStr = QString("Value: %1").arg(star->estimatedValue());
    _lines += valueStr;
}

#endif

ScanCommanderData::ScanCommanderData(QObject *parent)
    : BaseCommanderData(parent) {
}
