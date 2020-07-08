#ifndef USER_REGISTER_H
#define USER_REGISTER_H

#include <QWidget>

namespace Ui {
class user_register;
}

class user_register : public QWidget
{
    Q_OBJECT

public:
    explicit user_register(QWidget *parent = nullptr);
    ~user_register();
signals:
    void back_to_login();

private slots:
    void on_pushButton_2_clicked();

private:
    Ui::user_register *ui;
};

#endif // USER_REGISTER_H
