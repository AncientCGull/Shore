#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QTest>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    labels.append(ui->label_1); // объединяем QLabel'ы в QList
    labels.append(ui->label_2); // для удобства использования
    labels.append(ui->label_3);
    labels.append(ui->label_4);
    labels.append(ui->label_5);

    areas.append(ui->scrollArea); // с ScrollArrea так же
    areas.append(ui->scrollArea_2);
    areas.append(ui->scrollArea_3);
    areas.append(ui->scrollArea_4);
    areas.append(ui->scrollArea_5);

    for (auto a : areas)
    {
        a->setVisible(false); // делаем изначально все невидимыми
    }

    font = new QFont("Charter", 25);
//    QFont font ("Times New Roman", 27); // ставим шрифт и размер побольше, чем родной
    for (auto a : labels)
    {
        a->setFont(*font); // применяем шрифт к надписям
    }
    ui->label->setFont(*font);
    ui->pushButton_factor->setFont(*font);
    ui->pushButton_step->setFont(*font);
    ui->lineEdit_num->setFont(*font);
    ui->checkBox_auto->setFont(*font);

#ifdef QT_NO_DEBUG // в release версии выключаем автошаг
    ui->checkBox_auto->setChecked(false);
    ui->checkBox_auto->setVisible(false);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::writeMsg(int step, QString text) // написать очередной шаг
{
    areas[step]->setVisible(true);
    labels[step]->setText(text);
//    labels[step]->setTextFormat(Qt::RichText);
    labels[step]->setStyleSheet("color: rgb(50, 200, 70)"); // выделяем цветом последний показанный шаг
    if (step > 0)
    {
        labels[step-1]->setStyleSheet("color: rgb(255, 255, 255)"); // окрашиваем предыдщуий белым
    }
//    labels[step]->setToolTip(QString::number(step));
}

void MainWindow::unlockButton() // слот – разблокировка кнопки шага, когда этот шаг доступен
{
    ui->pushButton_step->setEnabled(true);
    if (ui->checkBox_auto->isChecked()) // проверка автошага (включено только в отладке)
    {
        on_pushButton_step_clicked();
    }
}

void MainWindow::reload() // перезапуск для следующего алгоритма
{
    ui->pushButton_factor->setEnabled(true);
    ui->lineEdit_num->setEnabled(true);
    int last = 4;
    for (auto a : labels) // цикл для определения последней показанной надписи
    {
        if (labels[last]->isVisible())
        {
            break;
        }
        last--;
    }
    QTest::qWait (5000); // ждем 5 секунд
    labels[last]->setStyleSheet("color: rgb(255, 255, 255)"); // красим в белый последний шаг
}


void MainWindow::on_pushButton_factor_clicked() // основная ф-ия, запускаем алгоритм
{
    ui->label->setVisible(false);

    for (auto a : areas)
    {
        a->setVisible(false); // прячем все, что было неспрятано
    }

    for (auto a : labels)
    {
        a->setText("");
    }

    ui->pushButton_factor->setEnabled(false); // блочим кнопку и LineEdit
    ui->lineEdit_num->setDisabled(true); // "защита от дурака"

    if (ui->lineEdit_num->text().toLongLong() < 15) //  проверяем число
    {
        QMessageBox::critical(this, "Ошибка", "Минимальное подходящее число для факторизации – 15.");
        return;
    }

    step = 0;

    if (contr != nullptr) // удаляем объект Controller, если он есть
    {
        delete contr;
        contr = nullptr;
    }

    /*
     * В объекте Controller contr создается поток, в котором выполняется весь алгоритм;
     * этот поток существует только тогда, когда существует contr;
     * оттуда идут сигналы:
     *      1. о готовности нового шага – unlockButton,
     *      2. о конце работы алгоритма – done;
     * а так же есть метод getText, возвращающий текст шага.
     */

    contr = new Controller(ui->lineEdit_num->text().toLongLong());
    connect(contr, &Controller::unlockButton, this, &MainWindow::unlockButton);
    connect(contr, &Controller::done, this, &MainWindow::reload);
    contr->start();
}

void MainWindow::on_pushButton_step_clicked() // очередной шаг
{
    if (contr != nullptr)
    {
        writeMsg(step, contr->getText(step)); // пишем текст шана
    }
    step++;


    if (not contr->check(step)) // проверяем готовность следующего шага
    {
        ui->pushButton_step->setDisabled(true);
        return;
    }
    if (ui->checkBox_auto->isChecked()) // проверяем автошаг (включено только в отладке)
    {
        on_pushButton_step_clicked();
    }
}

void MainWindow::on_action_help_triggered() // вывод справки
{
    if (contr != nullptr)
    {
        // если алгоритм выполняется прямо сейчас
        help += "В данный момент программа выполняет вычисления. Пожалуйста, дождитесь результата.";
    }
    QMessageBox* box = new QMessageBox(QMessageBox::NoIcon, "Помощь", help, QMessageBox::Close, this);
    box->setFont(*font);
    box->show();
}
