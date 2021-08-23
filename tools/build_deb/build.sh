#!/bin/sh

./linuxdeployqt ../../release/out/game_box_temp -appimage
rm -rf ../../release/out/default.desktop
rm -rf ../../release/out/default.png
rm -rf ../../release/out/game_box_temp
rm -rf ../../release/out/doc
cp ./game_box.sh ../../release/out/game_box.sh

