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
#include <QObject>

#ifdef Q_OS_WIN
#include "windows.h"
#include "Events.h"
namespace Journal {
    class JournalFile;
}


class MFDPage : public QObject {
    Q_OBJECT
public:

    MFDPage(QObject *parent);

    QString textForLine(size_t line) const;
    bool stepLine(bool up);

    void setLines(const QStringList &lines);
    DWORD numLines() const;

    virtual bool update(const Journal::JournalFile &journal, Journal::EventPtr ev) { return false; };
    virtual bool scrollWheelclick() {
        if(_currentLine > 0) {
            _currentLine = 0;
            return true;
        }
        return false;
    }

protected:
    int _currentLine{};
    QStringList _lines{};
};


#endif
