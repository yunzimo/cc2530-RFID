#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>  //提供访问串口的功能
#include <QtSerialPort/QSerialPortInfo>   //提供系统中存在的串口信息

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QSerialPort *serial;


private slots:
     void on_pushButton_clicked();
     void dataReceive();

     void on_pushButton_2_clicked();

     void on_pushButton_3_clicked();

private:
    Ui::Widget *ui;

};
#endif // WIDGET_H
