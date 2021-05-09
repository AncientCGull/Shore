#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "shor.h"

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

private slots:
    void on_pushButton_factor_clicked();

private:
    Ui::MainWindow *ui;
    QThread shor_thread;

signals:
    void start(bigint N);

};
#endif // MAINWINDOW_H
