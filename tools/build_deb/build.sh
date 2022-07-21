#!/bin/sh

./linuxdeployqt ../../release/out/game_box_temp -appimage
mkdir ../../release/out/plugins
cp $Qt6_DIR/lib/libQt6XcbQpa.so.6 ../../release/out/lib/libQt6XcbQpa.so.6
cp $Qt6_DIR/lib/libQt6OpenGL.so.6 ../../release/out/lib/libQt6OpenGL.so.6
mkdir ../../release/out/plugins/bearer
cp $Qt6_DIR/plugins/bearer/libqconnmanbearer.so ../../release/out/plugins/bearer/libqconnmanbearer.so
cp $Qt6_DIR/plugins/bearer/libqgenericbearer.so ../../release/out/plugins/bearer/libqgenericbearer.so
cp $Qt6_DIR/plugins/bearer/libqnmbearer.so ../../release/out/plugins/bearer/libqnmbearer.so
mkdir ../../release/out/plugins/iconengines
cp $Qt6_DIR/plugins/iconengines/libqsvgicon.so ../../release/out/plugins/iconengines/libqsvgicon.so
mkdir ../../release/out/plugins/imageformats
cp $Qt6_DIR/plugins/imageformats/libqgif.so ../../release/out/plugins/imageformats/libqgif.so
cp $Qt6_DIR/plugins/imageformats/libqico.so ../../release/out/plugins/imageformats/libqico.so
cp $Qt6_DIR/plugins/imageformats/libqjpeg.so ../../release/out/plugins/imageformats/libqjpeg.so
cp $Qt6_DIR/plugins/imageformats/libqsvg.so ../../release/out/plugins/imageformats/libqsvg.so
mkdir ../../release/out/plugins/platforminputcontexts
cp $Qt6_DIR/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so ../../release/out/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so
cp $Qt6_DIR/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so ../../release/out/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so
mkdir ../../release/out/plugins/platforms
cp $Qt6_DIR/plugins/platforms/libqxcb.so ../../release/out/plugins/platforms/libqxcb.so
mkdir ../../release/out/plugins/xcbglintegrations
cp $Qt6_DIR/plugins/xcbglintegrations/libqxcb-egl-integration.so ../../release/out/plugins/xcbglintegrations/libqxcb-egl-integration.so
cp $Qt6_DIR/plugins/xcbglintegrations/libqxcb-glx-integration.so ../../release/out/plugins/xcbglintegrations/libqxcb-glx-integration.so
rm -rf ../../release/out/default.desktop
rm -rf ../../release/out/default.png
rm -rf ../../release/out/game_box_temp
rm -rf ../../release/out/doc
cp ./game_box.sh ../../release/out/game_box.sh

