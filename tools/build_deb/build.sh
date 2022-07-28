#!/bin/sh

export QMAKE=$Qt6_DIR/bin/qmake
./linuxdeploy-x86_64.AppImage --executable=../../release/out/game_box_temp --appdir=../../release --plugin=qt
rm -rf ../../release/apprun-hooks
mv ../../release/out/game_box ../../release/usr/game_box
rm -rf ../../release/out
mv ../../release/usr ../../release/out
mv ../../release/out/bin/qt.conf ../../release/out/qt.conf
rm -rf ../../release/out/bin
sed -i "s/Prefix = ..\//Prefix = .\//g" ../../release/out/qt.conf
chrpath -r "\$ORIGIN/./lib" ../../release/out/game_box
rm -rf ../../release/out/share
cp ./game_box.sh ../../release/out/game_box.sh
