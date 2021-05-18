#ifndef SHOR_H
#define SHOR_H

#include <QObject>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <tuple>
#include <vector>

#include "qpp/qpp.h"

using namespace qpp;

class shor : public QObject
{
    Q_OBJECT

private:
    bigint N = 32;
    QString factors = "";
    bigint check();

public:
    shor();
    void calculate();
    void getNum(bigint num);

signals:
    void write(QString);
    void done();
};

#endif // SHOR_H
