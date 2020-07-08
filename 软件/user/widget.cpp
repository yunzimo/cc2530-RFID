#include "widget.h"
#include "ui_widget.h"

#include <QDebug>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);


    serial=new QSerialPort;
    //串口初始化
    serial->setPortName("COM10");
    serial->open(QIODevice::ReadWrite);
    serial->setBaudRate(9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    connect(serial,SIGNAL(readyRead()),this,SLOT(dataReceive()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::dataReceive()
{
    QString buf;
    buf=serial->readAll();
    qDebug()<<buf;
    ui->textEdit->append(buf);
}

void Widget::on_pushButton_clicked()
{

    QString sendme="1"+ui->lineEdit->text()+"#";
    QByteArray sendbytes=(sendme).toLatin1();
    //QByteArray sendbytes=ui->lineEdit->text().toLatin1();
    //qDebug()<<sendbytes;
    serial->write(sendbytes);
}

void Widget::on_pushButton_2_clicked()
{
    //QString sendme="0"+ui->lineEdit_2->text()+"#";
    QString sendme="0"+ui->spinBox->text()+"#";
    QByteArray sendbytes=(sendme).toLatin1();
    //QByteArray sendbytes=ui->lineEdit->text().toLatin1();
    serial->write(sendbytes);
}

void Widget::on_pushButton_3_clicked()
{
    QString sendme="c#";
    QByteArray sendbytes=sendme.toLatin1();
    serial->write(sendbytes);
}
