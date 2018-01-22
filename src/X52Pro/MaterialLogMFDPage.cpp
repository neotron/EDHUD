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

#include "MaterialLogMFDPage.h"
#include <QDebug>
#ifdef Q_OS_WIN
#include <Types/Materials.h>
#include <JFile.h>
#include <Events/EventEngineerCraft.h>

using namespace Journal;

MaterialLogMFDPage::MaterialLogMFDPage(QObject *parent, DWORD pageId)
    : MFDPage(parent, pageId) {
    updateLines(nullptr);
}

//1234567890123456
//1.82e6K M0.62 R0
bool MaterialLogMFDPage::scrollWheelclick() {
    auto data = dataForCommander(_commander, this);
    if(!data) {
        return false;
    }
    for(auto &id: data->materalDeltas.keys()) {
        data->materalDeltas[id] = 0;
    }
    updateLines(data);
    return true;
}

void MaterialLogMFDPage::changeMaterial(const QString &materialName, int64_t delta, MaterialCommanderData *data) {
    if(!delta) { return; }

    auto material = Materials::material(materialName);
    if(material.isValid()) {
        data->materalDeltas[materialName] += delta;
        data->changeOrder.removeAll(materialName);
        data->changeOrder.insert(0, materialName);
        updateLines(data);
        notifyChange();
    }
}

void MaterialLogMFDPage::updateLines(const MaterialCommanderData *data) {
    _currentLine = 0;
    if(!data || data->changeOrder.isEmpty()) {
        // We have nothing yet.
        _lines.append("Material Deltas");
        _lines.append("awaiting changes");
        _lines.append("Click to reset");
    } else {
        _lines.clear();
        for(const auto &material: data->changeOrder) {

            auto mat = Materials::material(material);
            // 1234567890123456
            // 123456789012 XXX
            // BICO VR ENC XXXX
            // BICO R  RAW XXXX
            // BANN VC MAN XXXX
            // 1234567890123456
            // Raw++++ BnNb 293
            // Man++   Ir     8

            QString line = QString("%1 %2%3 %4")
                    .arg(mat.abbreviation(), -4)
                    .arg(mat.typeName())
                    .arg(mat.rarityName())
                    .arg(data->materalDeltas[material], 3);
            _lines.append(line);
        }
    }
}

void MaterialLogMFDPage::onEventGeneric(Event *event) {
    auto data = dataForEvent(event, this);
    if(!data) { return; }
    switch(event->journalEvent()) {
        case Event::MaterialCollected:
            changeMaterial(event->string("Name"), event->integer("Count"), data);
            break;
        case Event::MaterialDiscarded:
            changeMaterial(event->string("Name"), -event->integer("Count"), data);
            break;
        case Event::Synthesis:
        default:
            break;
    }
}

void MaterialLogMFDPage::onEventEngineerCraft(EventEngineerCraft *event) {
    auto data = dataForEvent(event, this);
    if(!data) { return; }
    for(const auto &mat: event->ingredients()) {
        changeMaterial(mat.id(), -mat.quantity(), data);
    }
}

#endif

MaterialCommanderData::MaterialCommanderData(QObject *parent)
    : BaseCommanderData(parent) {
}
