#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile f(":/qss.txt");
    f.open(QFile::ReadOnly);
    QString qss = f.readAll();
    a.setStyleSheet(qss);
    f.close();

    MainWindow w;
    w.show();

    return a.exec();
}
