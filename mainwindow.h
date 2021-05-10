#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
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
    void writeMsg(QString);
    void unlockButton();

private slots:
    void on_pushButton_factor_clicked();

    void on_pushButton_step_clicked();

private:
    Ui::MainWindow *ui;
    Controller* contr;
    int step = 0;

//signals:
//    void start(bigint N);

};
#endif // MAINWINDOW_H
