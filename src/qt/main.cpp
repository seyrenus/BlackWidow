#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QObject::tr("BlackWidow"));
    w.setWindowIcon(QIcon(":resources/icons/spider.svg"));
    w.show();
    return a.exec();
}
