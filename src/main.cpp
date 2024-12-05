#include "mainwindow.h"

#include <QApplication>
#include <QScreen>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("Nature Werks");
    QCoreApplication::setOrganizationName("CS 1230");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    // Set OpenGL version to 4.1 and context to Core
    QSurfaceFormat fmt;
    fmt.setVersion(4, 1);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    MainWindow w;
    // w.resize(w.sizeHint());
    w.resize(2000, 1080);
    int desktopArea = QGuiApplication::primaryScreen()->size().width() *
                      QGuiApplication::primaryScreen()->size().height();
    int widgetArea = w.width() * w.height();
    if (((float)widgetArea / (float)desktopArea) < 0.75f) {
        w.show();
    } else {
        w.showMaximized();
    }

    return a.exec();
}
