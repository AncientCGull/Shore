#include "mainwindow.h"

#include <QApplication>
#include <iostream>
#include "qpp/qpp.h"

int main(int argc, char *argv[])
{
    using namespace qpp;

    std::cout << "Hello Quantum++!\nThis is the |0> state:\n";
    std::cout << disp(0_ket) << '\n';
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
