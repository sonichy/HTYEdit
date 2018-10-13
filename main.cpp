#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qSetMessagePattern("[ %{file}: %{line} ] %{message}");
    /*  Qt5以下：
        QTextCodec *tc = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForTr(tc);
        QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
        QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    */
    MainWindow w;
    w.show();

    //在新标签里打开文件
    //https://github.com/linuxdeepin/deepin-editor/blob/209eac26c54fe6d2562dd8e00608b8d62b328529/src/main.cpp#L69
    //QDBusInterface.callWithArgumentList
    return app.exec();
}