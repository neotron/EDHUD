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
#include "MFDPage.h"
#ifdef Q_OS_WIN
#include <QDebug>
#include <LiveJournal.h>

QString MFDPage::textForLine(size_t line) const {
    // Calculate actual line using current offset.
    auto actualLine = _currentLine + line;
    if(actualLine >= _lines.size()) {
        return "";
    }
    return _lines[static_cast<int>(actualLine)];
}

bool MFDPage::stepLine(bool up) {
    int newLine = up ? _currentLine - 1 : _currentLine + 1;
    if(newLine < 0 || newLine == _lines.size()) {
        return false;
    }
    _currentLine = newLine;
    return true;
}

void MFDPage::setLines(const QStringList &lines) {
    _currentLine = 0;
    _lines = lines;
}

DWORD MFDPage::numLines() const {
    return static_cast<DWORD>(_lines.count());
}


MFDPage::MFDPage(QObject *parent, DWORD pageId)
    : EventDispatch(parent), _pageId(pageId) {
    Journal::LiveJournal::instance()->registerHandler(this);
}

bool MFDPage::scrollWheelclick() {
    if(_currentLine > 0) {
        _currentLine = 0;
        return true;
    }
    return false;
}

void MFDPage::notifyChange() { emit onChange(_pageId); }

#endif

