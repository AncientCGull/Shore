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

public:
    shor();
    void calculate();

signals:
    void write(QString);
    void done();
};

#endif // SHOR_H
