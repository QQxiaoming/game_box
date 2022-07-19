#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QString>
#include <iostream>

int main(int argc, char *argv[])
{
    if(argc == 2) {
        if((!strncmp(argv[1],"--version",9)) | (!strncmp(argv[1],"-v",2)) ) {
            const QString VERSION = APP_VERSION;
            const QString GIT_TAG =
            #include <git_tag.inc>
            ;
            std::cout << "game_box " << VERSION.toStdString() << "\n" << GIT_TAG.toStdString() << "\n";
            return 0;
        }
    }

    QTranslator sysTranslator;
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    if(sysTranslator.load("qt_zh_CN.qm",QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        std::cout << "sysTranslator load err\n";
    }

    QApplication app(argc, argv);
    app.installTranslator(&sysTranslator);
    MainWindow window;
    window.show();
    return app.exec();
}
