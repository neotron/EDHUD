QT_MA=5
QT_MI=11
QT_PA=2
QT=${QT_MA}.${QT_MI}.${QT_PA}
QTUS=${QT_MA}_${QT_MI}_${QT_PA}
all:
	@echo mac-package or win-package

OUT := $(shell echo "output/EDPathFinder-`/usr/libexec/PlistBuddy -c 'Print CFBundleVersion' cmake-build-minsizerel/EDPathFinder.app/Contents/Info.plist`.dmg")

mac-package:  
	/workspace/Qt/${QT}/clang_64/bin/macdeployqt cmake-build-minsizerel/EDPathFinder.app   -always-overwrite
	rm -rf cmake-build-minsizerel/Contents/Frameworks/QtSql.framework
	rm -f "${OUT}" ; appdmg  cmake/appdmg.json "${OUT}"

win-package: 
	rm -rf output/EDHUD
	mkdir -p output/EDHUD
	cp cmake-build-minsizerel/EDHUD.exe output/EDHUD
	cp cmake-build-minsizerel/DirectOutput.dll output/EDHUD
	rm -f wininstall/*.exe
	env VCINSTALLDIR="C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC" /cygdrive/c/User\ Programs/Qt/${QT}/msvc2017_64/bin/windeployqt.exe output/EDHUD/EDHUD.exe
	(cd wininstall && /cygdrive/c/Program\ Files\ \(x86\)/NSIS/makensis.exe installer.nsi)

win-package-bash: 
	rm -rf output/EDPathFinder
	mkdir -p output/EDPathFinder
	cp ../build-EliteTSP-Desktop_Qt_${QTUS}_MSVC2015_64bit-Minimum\ Size\ Release/EDPathFinder.exe output/EDPathFinder
	rm -f wininstall/*.exe
	(export VCINSTALLDIR="C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC"; /mnt/c/User\ Programs/Qt/${QT}/msvc2017_64/bin/windeployqt.exe output/EDPathFinder/EDPathFinder.exe)
	(cd wininstall && /mnt/c/Program\ Files\ \(x86\)/NSIS/makensis.exe installer.nsi)
