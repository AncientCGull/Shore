#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtConcurrent>
#include <QFuture>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    shor* shor_obj = new shor();
    shor_obj ->moveToThread(&shor_thread);
    connect(&shor_thread, &QThread::finished, shor_obj, &QObject::deleteLater);
    connect(&shor_thread, &QThread::started, shor_obj, &shor::calculate);
    connect(shor_obj, &shor::write, this, &MainWindow::writeMsg);
}

MainWindow::~MainWindow()
{
    shor_thread.quit();
    shor_thread.wait();
    delete ui;
}

void MainWindow::writeMsg(QString text)
{
    ui->textEdit->append(text);
}


void MainWindow::on_pushButton_factor_clicked()
{
    shor_thread.start();
}
