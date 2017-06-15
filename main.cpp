#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /*  Qt5以下：
        QTextCodec *tc = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForTr(tc);
        QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
        QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    */

    MainWindow w;
    w.show();
    return app.exec();
}
