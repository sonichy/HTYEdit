#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("HTY");
    app.setApplicationName("HTYEdit");
    qSetMessagePattern("[ %{file}: %{line} ] %{message}");
    MainWindow w;
    w.show();
    return app.exec();
}