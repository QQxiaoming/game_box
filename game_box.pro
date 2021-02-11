QT       += core gui
QT       += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APP_VERSION="\\\"V0.0.1\\\""

INCLUDEPATH += \
        -I ./nes/port \
        -I ./nes/src \
        -I ./md/port \
        -I ./md/src/musa \
        -I ./md/src

SOURCES += \
    nes/port/InfoNES_System.cpp \
    nes/src/InfoNES_Mapper.cpp \
    nes/src/InfoNES_pAPU.cpp \
    nes/src/InfoNES.cpp \
    nes/src/K6502.cpp \
    md/src/musa/m68kcpu.cpp \
    md/src/musa/m68kdasm.cpp \
    md/src/musa/m68kops.cpp \
    md/src/md.cpp \
    md/src/mem.cpp \
    md/src/fm.cpp \
    md/src/vdp.cpp \
    md/src/myfm.cpp \
    md/src/mdfr.cpp \
    md/src/sn76496.cpp \
    md/src/ras.cpp \
    md/src/graph.cpp \
    md/src/save.cpp \
    md/src/decode.cpp \
    md/src/romload.cpp \
    md/port/md_port.cpp \
    main.cpp \
    mainwindow.cpp \
    keysetting.cpp
    
HEADERS += \
    nes/port/InfoNES_System.h \
    nes/src/InfoNES_Mapper.h \
    nes/src/InfoNES_pAPU.h \
    nes/src/InfoNES.h \
    nes/src/K6502.h \
    md/src/md.h \
    md/src/fm.h \
    mainwindow.h \
    keysetting.h

FORMS += \
    mainwindow.ui \
    keysetting.ui

RESOURCES += \
        res/resources.qrc

# 输出配置
build_type =
CONFIG(debug, debug|release) {
    build_type = debug
} else {
    build_type = release
}

DESTDIR     = $$build_type/out
OBJECTS_DIR = $$build_type/obj
MOC_DIR     = $$build_type/moc
RCC_DIR     = $$build_type/rcc
UI_DIR      = $$build_type/ui

win32:{
    QMAKE_LFLAGS += -Wl,--large-address-aware

    VERSION = 0.0.1.0
    RC_LANG = 0x0004
    RC_ICONS = "img\icon.ico"

    CONFIG(release, debug|release) {
        AFTER_LINK_CMD_LINE = $$PWD/tools/upx307w/upx.exe --best -f $$DESTDIR/$${TARGET}.exe
        QMAKE_POST_LINK += $$quote($$AFTER_LINK_CMD_LINE)
    }
}

unix:{
    QMAKE_RPATHDIR=$ORIGIN
    QMAKE_LFLAGS += -no-pie

    CONFIG(release, debug|release) {
        AFTER_LINK_CMD_LINE = $$PWD/tools/upx-3.91-amd64_linux/upx --best -f $$DESTDIR/$$TARGET
        QMAKE_POST_LINK += $$quote($$AFTER_LINK_CMD_LINE)
    }
}
