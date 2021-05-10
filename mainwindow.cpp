#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtConcurrent>
#include <QFuture>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::writeMsg(QString text)
{
    ui->textEdit->append(text);
}

void MainWindow::unlockButton()
{
    ui->pushButton_step->setEnabled(true);
    if (ui->checkBox_auto->isChecked())
    {
        on_pushButton_step_clicked();
    }
}


void MainWindow::on_pushButton_factor_clicked()
{
    ui->textEdit->clear();
    step = 0;

    if (contr != nullptr)
    {
        delete contr;
    }
    contr = new Controller(ui->lineEdit_num->text().toLongLong());
    connect(contr, &Controller::write, this, &MainWindow::writeMsg);
    connect(contr, &Controller::unlockButton, this, &MainWindow::unlockButton);
    contr->start();
}

void MainWindow::on_pushButton_step_clicked()
{
    if (contr != nullptr)
    {
        writeMsg(contr->getText(step));
    }
    step++;

    if (not contr->check(step))
    {
        ui->pushButton_step->setDisabled(true);
        return;
    }
    if (ui->checkBox_auto->isChecked())
    {
        on_pushButton_step_clicked();
    }
}
