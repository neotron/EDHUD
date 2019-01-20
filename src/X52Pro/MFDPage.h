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
#include <EventDispatchObject.h>
#include <Events/Event.h>
#include <JFile.h>
#include <QDebug>

#define FMTK(X) ((X) < 10000 ? QString::number(X) : QString("%1k").arg(round((X)/1000.0)))

struct  BaseCommanderData: public QObject {
    QOBJECT_H
    explicit BaseCommanderData(QObject *parent = nullptr) : QObject(parent) {}

    QDateTime lastEventTimestamp{};
    QString commander;
};

template <typename CommanderData>
class CommanderState {

protected:
    QString _commander;

    CommanderData *dataForEvent(const Journal::Event *event, QObject *parent) {
        auto commander = dataForCommander(event->file()->commander(), parent);
        if(!commander || commander->lastEventTimestamp > event->timestamp()) {
            return nullptr;
        }
        commander->lastEventTimestamp = event->timestamp();
        return commander;
    }

    CommanderData *dataForCommander(const QString &cmdr, QObject *parent) {
        if(cmdr.isEmpty()) {
            return nullptr;
        }
        _commander = cmdr;
        if(!_data.contains(cmdr)) {
            qDebug() << "Creating new storage for cmdr." << cmdr;
            _data[cmdr] = new CommanderData(parent);
            _data[cmdr]->commander = cmdr;
        }
        return  _data[cmdr];
    }

private:
    QMap<QString,CommanderData*> _data;
};

class MFDPage : public Journal::EventDispatchObject {
    Q_OBJECT
public:

    MFDPage(QObject *parent, DWORD pageId);

    QString textForLine(size_t line) const;
    bool stepLine(bool up);

    void setLines(const QStringList &lines);
    DWORD numLines() const;
    virtual bool scrollWheelclick();

signals:
    void onChange(DWORD page);

protected:
    void notifyChange();


    DWORD _pageId;
    int _currentLine{};
    QStringList _lines{};


};


#endif
