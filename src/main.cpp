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


#include <QApplication>
#include <QDebug>
#include <src/Utility/Version.h>
#include "UI/MainWindow.h"
#include "Settings/Theme.h"
Q_DECLARE_METATYPE(Version);
#ifdef Q_OS_WIN
#include <windows.h>
#endif

// namespace operations_research

//#define USE_SPLASH
int main(int argc, char *argv[]) {
    // Enable console debug output on Windows, if started from a console window.
#ifdef Q_OS_WIN
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE *fpo, *fpe;
        freopen_s(&fpo, "CONOUT$", "w", stdout);
        freopen_s(&fpe, "CONOUT$", "w", stderr);
    }
#endif
   // qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QCoreApplication::setOrganizationName("NeoTron Software");
    QCoreApplication::setApplicationName("EDHUD");

    qRegisterMetaType<Version>();
    QApplication a(argc, argv);

    Theme::s_defaultPalette = a.palette();
    Theme::applyTheme();

    QPixmap pixmap(":/icon128.png");
    MainWindow mainWindow;
    QIcon icon(pixmap);
    mainWindow.setWindowIcon(icon);
    mainWindow.show();
    return a.exec();
}


