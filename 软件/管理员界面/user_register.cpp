#include "user_register.h"
#include "ui_user_register.h"

user_register::user_register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::user_register)
{
    ui->setupUi(this);


}

user_register::~user_register()
{
    delete ui;
}

void user_register::on_pushButton_2_clicked()
{
    this->hide();
    emit back_to_login();
}
