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
#ifdef Q_OS_WIN
#include <QDebug>
#include <Types/Materials.h>
#include <JournalFile.h>

using namespace Journal;

MaterialLogMFDPage::MaterialLogMFDPage(QObject *parent) : MFDPage(parent) {
    updateLines();
}

bool MaterialLogMFDPage::update(const JournalFile &journal, Journal::EventPtr ev) {
    bool didChange = false;
    switch(ev->type()) {
    case Event::MaterialCollected:
        didChange = changeMaterial(ev->string("Name"), ev->integer("Count"));
        break;
    case Event::MaterialDiscarded:
        didChange = changeMaterial(ev->string("Name"), -ev->integer("Count"));
        break;
    case Event::Synthesis:
    default:
        break;
    }

    if(didChange) {
        updateLines();
    }
    return didChange;
}

//1234567890123456
//1.82e6K M0.62 R0
bool MaterialLogMFDPage::scrollWheelclick() {
    for(auto &id: _materalDeltas.keys()) {
        _materalDeltas[id] = 0;
    }
    updateLines();
    return true;
}

bool MaterialLogMFDPage::changeMaterial(const QString &materialName, int delta) {
    if(!delta) { return false; }

    auto material = Materials::material(materialName);
    if(!material.isValid()) {
        return false;
    }
    _materalDeltas[materialName] += delta;
    _changeOrder.removeAll(materialName);
    _changeOrder.insert(0, materialName);
    return true;
}

void MaterialLogMFDPage::updateLines() {
    _currentLine = 0;
    if(_changeOrder.isEmpty()) {
        // We have nothing yet.
        _lines.append("Material Deltas");
        _lines.append("awaiting changes");
        _lines.append("click to reset");
    } else {
        _lines.clear();
        for(const auto &material: _changeOrder) {

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
                    .arg(_materalDeltas[material], 3);
            _lines.append(line);
        }
    }
}
#endif
