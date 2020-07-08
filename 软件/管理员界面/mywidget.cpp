#include "mywidget.h"
#include "ui_mywidget.h"



#include <QDebug>

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariantList>






MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyWidget)
{
    ui->setupUi(this);

    //数据库
    qDebug()<<QSqlDatabase::drivers();


    //打开数据库
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection")){
        db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else{
        db=QSqlDatabase::addDatabase("QSQLITE");
    }

    db.setDatabaseName("user.db");

    if(!db.open()){
        QMessageBox::warning(this,"错误",db.lastError().text());
        return;
    }
    QSqlQuery deleteall;
    deleteall.exec("delete from user;");

    //创建表
    QSqlQuery query;
    query.exec("create table user(id varchar(255),name varchar(255),passwd varchar(255));");

    query.prepare("insert into user(id,name,passwd) values(?,?,?);");
    QVariantList idlist;
    idlist<<"11"<<"22"<<"33";
    QVariantList namelist;
    namelist<<"11"<<"ming"<<"li";
    QVariantList passwd;
    passwd<<"11"<<"22"<<"33";
    query.addBindValue(idlist);
    query.addBindValue(namelist);
    query.addBindValue(passwd);
    query.execBatch();


    connect(&c,SIGNAL(zhuxiao()),this,SLOT(changepage()));
    connect(&res,SIGNAL(back_to_login()),this,SLOT(backlog()));
}

MyWidget::~MyWidget()
{
    delete ui;
}

void MyWidget::dataReceive()
{
    QString buf;
    buf=serial->readAll();
    qDebug()<<buf;
    //text->setText(buf);

    //text->clear();
}

void MyWidget::on_b1_clicked()
{
    QString id = ui->lineEdit->text();
    if(id==NULL){
        QMessageBox::warning(this,"警告","请输入用户ID");
    }
    else{
        QString passwd=ui->lineEdit_2->text();
        if(passwd==NULL){
            QMessageBox::warning(this,"警告","请输入密码");
        }
        else{
            QSqlQuery check;
            QString temp=QString("select * from user where id='%1' and passwd ='%2'").arg(id).arg(passwd);
            check.exec(temp);
            QString checkin;
            while(check.next()){
                checkin=check.value(1).toString();
            }
            if(checkin==NULL){
                QMessageBox::warning(this,"警告","用户名或密码输入错误");
            }
            else{

                this->hide();
                c.show();
            }
        }
    }
}


void MyWidget::changepage(){
    this->show();
    c.hide();
}

void MyWidget::on_pushButton_clicked()
{
    this->hide();
    res.show();
}
void MyWidget::backlog(){
    this->show();
}
