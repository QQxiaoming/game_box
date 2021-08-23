#!/bin/sh

./linuxdeployqt ../../release/out/game_box_temp -appimage
rm -rf ../../release/out/game_box_temp
