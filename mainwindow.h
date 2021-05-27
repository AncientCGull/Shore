#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QLabel>
#include <QScrollArea>
#include "shor.h"
#include "controller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Shore(bigint N);

public slots:
    void writeMsg(int, QString);
    void unlockButton();
    void reload();

private slots:
    void on_pushButton_factor_clicked();

    void on_pushButton_step_clicked();

    void on_action_help_triggered();

private:
    Ui::MainWindow *ui;
    Controller* contr = nullptr;
    int step = 0;
    bool isReload = false;
    QList <QLabel*> labels;
    QList <QScrollArea*> areas;
    QFont* font;
    QString help = "1. Введите число, множители которого хотите узнать. Оно должно быть больше или равно 15.\n\n"
                   "2. Нажмите кнопку \"Начать факторизацию\". Запустится алгоритм, и пройдет первый шаг.\n\n"
                   "3. Нажимайте кнопку \"Следующий шаг\", пока не появится окно, сообщающее об окончании алгоритма.\n\n"
                   "4. Для детализации каждого шага нажмите на область с кратким текстом соответсвующего шага.\n\n";

//signals:
//    void start(bigint N);

};
#endif // MAINWINDOW_H
