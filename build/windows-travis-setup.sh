#!/bin/bash
set -v

if [ "$1" == "install" ]; then
  curl -Lo ~/qt-unified-windows-x86-online.exe http://download.qt.io/official_releases/online_installers/qt-unified-windows-x86-online.exe
  if ! ~/qt-unified-windows-x86-online.exe --verbose --script tools/qt-installer-windows.qs > ~/qt-installer-output.txt; then
    cat ~/qt-installer-output.txt; exit 1
  fi
fi
