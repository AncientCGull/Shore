#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    labels.append(ui->label_1);
    labels.append(ui->label_2);
    labels.append(ui->label_3);
    labels.append(ui->label_4);
    labels.append(ui->label_5);

    areas.append(ui->scrollArea);
    areas.append(ui->scrollArea_2);
    areas.append(ui->scrollArea_3);
    areas.append(ui->scrollArea_4);
    areas.append(ui->scrollArea_5);

    for (auto a : areas)
    {
        a->setVisible(false);
    }
    QFont font ("Apple Garamond", 20);
    for (auto a : labels)
    {
        a->setFont(font);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::writeMsg(int step, QString text)
{
    areas[step]->setVisible(true);
    labels[step]->setText(text);
//    labels[step]->setPalette(QPalette);
}

void MainWindow::unlockButton()
{
    ui->pushButton_step->setEnabled(true);
    if (ui->checkBox_auto->isChecked())
    {
        on_pushButton_step_clicked();
    }
}

void MainWindow::reload()
{
    ui->pushButton_factor->setEnabled(true);
    ui->lineEdit_num->setEnabled(true);
}


void MainWindow::on_pushButton_factor_clicked()
{
    for (auto a : areas)
    {
        a->setVisible(false);
    }

    for (auto a : labels)
    {
        a->setText("");
    }

    ui->pushButton_factor->setEnabled(true);
    ui->lineEdit_num->setDisabled(true);

    if (ui->lineEdit_num->text().toLongLong() < 15)
    {
        QMessageBox::critical(this, "Ошибка", "Минимальное подходящее число для факторизации – 15.");
        return;
    }

    step = 0;

    if (contr != nullptr)
    {
        delete contr;
        contr = nullptr;
    }
    contr = new Controller(ui->lineEdit_num->text().toLongLong());
    connect(contr, &Controller::unlockButton, this, &MainWindow::unlockButton);
    connect(contr, &Controller::done, this, &MainWindow::reload);
    contr->start();
}

void MainWindow::on_pushButton_step_clicked()
{
    if (contr != nullptr)
    {
        writeMsg(step, contr->getText(step));
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

void MainWindow::on_action_help_triggered()
{
    if (contr != nullptr)
    {
        help += "В данный момент программа выполняет вычисления. Пожалуйста, дождитесь результата.";
    }
    QMessageBox::about(this, "Помощь", help);
}
