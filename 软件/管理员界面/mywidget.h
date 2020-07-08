#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QTextEdit>

#include <QtSerialPort/QSerialPort>  //提供访问串口的功能
#include <QtSerialPort/QSerialPortInfo>   //提供系统中存在的串口信息

#include "carinfo.h"
//#include "ui_carinfo.h"
#include "user_register.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MyWidget; }
QT_END_NAMESPACE

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    MyWidget(QWidget *parent = nullptr);
    ~MyWidget();
    QTextEdit *text;
    QSerialPort *serial;
    carinfo c;
    user_register res;

public slots:
    void dataReceive();
    void changepage();
    void backlog();
private slots:


    void on_b1_clicked();

    void on_pushButton_clicked();

private:
    Ui::MyWidget *ui;

};
#endif // MYWIDGET_H
