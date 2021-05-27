#include "controller.h"

Controller::Controller(bigint N)
{
    shor_obj = new shor(); // создаем объект shor
    shor_obj->getNum(N); // подаем туда факторизуемое число
    shor_obj->moveToThread(&shor_thread); // перемещаем в поток
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

QString Controller::getText(int n) // возврат текста шага
{
    return messages.value(n);
}

bool Controller::check(int n) // проверка на выполнение шага
{
    return messages.count() > n;
}

void Controller::getInfo(QString text) // получение шага от shore_obj
{
    messages.append(text);
    emit unlockButton();
}
