#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QThread>
#include "shor.h"

class Controller : public QObject
{
    Q_OBJECT

public:
    Controller(bigint N);
    ~Controller();
    void start();
    QString getText(int n);
    bool check(int n);

public slots:
    void getInfo(QString text);

private:
    QThread shor_thread;
    shor* shor_obj;
    QStringList messages;

signals:
    void unlockButton();
    void done();
};

#endif // CONTROLLER_H
