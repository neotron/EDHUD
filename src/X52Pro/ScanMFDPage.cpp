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
#include "JournalFile.h"
using namespace Journal;

#define FMTK(X) ((X) < 10000 ? QString::number(X) : QString("%1k").arg(round((X)/1000.0)))



void ScanMFDPage::onEventGeneric(Event *event) {
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
            _history.clear();
            _currentEntry = 0;
            _currentLine = 0;
            notifyChange();
            break;
        default:
            break;
    }
}

void ScanMFDPage::onEventScan(EventScan *eventScan) {
    _lines.clear();
    switch(eventScan->bodyType()) {
        case Body::Planet:
            addScan(eventScan->planet());
            break;
        case Body::Star:
            addScan(eventScan->star());
            break;
        default:
            _lines += eventScan->string("BodyName");
            _lines += "Simple Body";
            _lines += "No Addl. Data";
            break; // No-op
    }
    _currentLine = _lines.count() > 3 ? 1 : 0; // Hide system name if we hav more than 3 lines

    _history.append(_lines);
    _currentEntry = _history.size() - 1;
    notifyChange();
}

ScanMFDPage::ScanMFDPage(QObject *parent, DWORD pageId)
    : MFDPage(parent, pageId) {
    _lines.append(QString("Scan Summary\n\nPending Scan").split("\n"));
}

bool ScanMFDPage::scrollWheelclick() {
    if(_history.size() <= 1) {
        return false;
    }
    _currentEntry++;
    if(_currentEntry >= _history.size()) {
        _currentEntry = 0;
    }
    _currentLine = 0;
    _lines = _history[_currentEntry];
    qDebug() << "Loaded page"<<_currentEntry<<endl<<_lines;
    return true;
}

void ScanMFDPage::addScan(Journal::PlanetPtr planet) {
    QString line2, line3;

    auto gravity = planet->surfaceGravity();
    auto numDigits = 2;
    if(gravity < 1.0) { --numDigits; }
    if(gravity > 99) { --numDigits; }
    switch(planet->atmosphereType()) {
        case Atmosphere::Unknown: // Gas giants etc
        {
            line2 = planet->typeNameMedium();

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
            line3 = QString("%1g M%2 R%3").arg(static_cast<int>(gravity)).arg(massStr).arg(radiusStr);
        }
            break;
        default: // normal planets with atmospheres.
            if(planet->landable()) {
                line3 = "Landable";
            } else {
                line3 = planet->atmosphereShort();
            }
            line2 = planet->typeNameShort();
            if(planet->terraformState() != TerraformState::None) {
                line2 += " TF";
            }
            line2 += QString(", %1g").arg(QString::number(gravity, 'f', numDigits));
    }
    auto temp = planet->surfaceTemperature();
    auto pressure = planet->surfacePressureBar();
    auto line4 = QString("%1K").arg(QString::number(temp, temp >= 1000 ? 'g' : 'f', 0));
    if(pressure > 0) {
        auto digits = pressure > 1000 ? 2 : 1;
        auto pressureStr = QString(" %1atm").arg(QString::number(pressure, pressure > 1000 ? 'g' : 'f', digits));
        line4 = QString("%1 %2").arg(line4, -6).arg(pressureStr, -9);
    }
    _lines += planet->bodyName();
    _lines += line2;
    _lines += line3;
    _lines += line4;

}

void ScanMFDPage::addScan(Journal::StarPtr star) {
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
}

#endif
