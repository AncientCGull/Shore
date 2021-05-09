#include "mainwindow.h"
//#include <iostream>
#include <QApplication>

int main(int argc, char *argv[])
{
//    std::cout << (int)(-3/3u*3) << std::endl;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
