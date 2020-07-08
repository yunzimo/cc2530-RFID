#include "carinfo.h"
#include "ui_carinfo.h"


#include <QDebug>

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariantList>



carinfo::carinfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::carinfo)
{
    ui->setupUi(this);
    qDebug() << QSqlDatabase::drivers();//打印qt支持的数据库类型
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

    QSqlQuery query;
    query.exec("create table carinfo(id varchar(255),location varchar(255),bx varchar(255),bro_info varchar(255));");
    query.exec("delete from carinfo;");

}

carinfo::~carinfo()
{
    delete ui;
}

void carinfo::on_pushButton_6_clicked()
{
    this->hide();
    emit zhuxiao();
}

void carinfo::on_insert_button_clicked()
{
    QString id=ui->lineEdit->text();
    QString location=ui->lineEdit_2->text();
    QString bx=ui->lineEdit_3->text();
    QString bro_info=ui->lineEdit_4->text();
    if(id==NULL||location==NULL||bx==NULL||bro_info==NULL){
        QMessageBox::warning(this,"警告","请输入完整信息");
    }
    else{
        QSqlQuery query;
        QString str=QString("select * from carinfo where id = '%1'").arg(id);
        query.exec(str);
        QString checkin;
        while(query.next()){
            checkin=query.value(1).toString();
        }
        if(checkin==NULL){

            query.prepare("insert into carinfo(id,location,bx,bro_info) values(?,?,?,?);");
            QVariantList idlist;
            idlist<<id;
            QVariantList locatlist;
            locatlist<<location;
            QVariantList bxlist;
            bxlist<<bx;
            QVariantList brolist;
            brolist<<bro_info;
            query.addBindValue(idlist);
            query.addBindValue(locatlist);
            query.addBindValue(bxlist);
            query.addBindValue(brolist);
            query.execBatch();

            QMessageBox::warning(this,"SUCESS","插入成功");
            ui->lineEdit->clear();
            ui->lineEdit_2->clear();
            ui->lineEdit_3->clear();
            ui->lineEdit_4->clear();

        }
        else{
            QMessageBox::warning(this,"警告","已存在相同信息");
        }

    }

}






void carinfo::on_query_a_button_clicked()
{
    QString id[100];
    QString location[100];
    QString bx[100];
    QString bro_info[100];
    int i=0;
    QSqlQuery query;
    query.exec("select * from carinfo;");
    while(query.next()){
        id[i]=query.value(0).toString();
        //qDebug()<<query.value(0).toString();
        location[i]=query.value(1).toString();
        bx[i]=query.value(2).toString();
        bro_info[i]=query.value(3).toString();
        i++;
    }


    ui->textEdit->clear();
    int j=0;
    for(j=0;j<i;j++){
        QString str=QString("车辆ID:%1  位置:%2   保修情况:%3  借出情况:%4").arg(id[j]).arg(location[j]).arg(bx[j]).arg(bro_info[j]);
        ui->textEdit->append(str);
    }

}





void carinfo::on_query_button_clicked()
{
    QString location,bx,bor;
    QString id=ui->lineEdit->text();
    if(id==NULL){
        QMessageBox::warning(this,"警告","请输入车辆ID");
    }
    else{
        QSqlQuery query;
        QString str=QString("select * from carinfo where id=%1").arg(id);
        query.exec(str);
        QString checkin;
        while(query.next()){
            checkin=query.value(1).toString();
        }
        if(checkin==NULL){
            QMessageBox::warning(this,"警告","没有此车信息，请检查输入的信息");
        }
        else{
            str=QString("select * from carinfo where id=%1").arg(id);
            query.exec(str);
            while(query.next()){
            location=query.value(1).toString();
            bx=query.value(2).toString();
            bor=query.value(3).toString();
            str=QString("车辆ID:%1  位置:%2   保修情况:%3  借出情况:%4").arg(id).arg(location).arg(bx).arg(bor);
            ui->textEdit->setText(str);
            }
        }
    }
}
