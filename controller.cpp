#include "controller.h"

Controller::Controller(bigint N)
{
    shor_obj = new shor();
    shor_obj->getNum(N);
    shor_obj ->moveToThread(&shor_thread);
    connect(&shor_thread, &QThread::finished, shor_obj, &QObject::deleteLater);
    connect(&shor_thread, &QThread::started, shor_obj, &shor::calculate);
    connect(shor_obj, &shor::write, this, &Controller::getInfo);
    connect(shor_obj, &shor::done, this, &Controller::done);
}

Controller::~Controller()
{
    shor_thread.quit();
    shor_thread.wait();
}

void Controller::start()
{
    shor_thread.start();
}

QString Controller::getText(int n)
{
    return messages.value(n);
}

bool Controller::check(int n)
{
    return messages.count() > n;
}

void Controller::getInfo(QString text)
{
    messages.append(text);
    emit unlockButton();
}
