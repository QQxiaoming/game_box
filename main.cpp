#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QTranslator sysTranslator;
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    sysTranslator.load("qt_zh_CN.qm",QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    QApplication app(argc, argv);
    app.installTranslator(&sysTranslator);
    MainWindow window;
    window.show();
    return app.exec();
}
