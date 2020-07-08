#ifndef CARINFO_H
#define CARINFO_H

#include <QWidget>


namespace Ui {
class carinfo;
}

class carinfo : public QWidget
{
    Q_OBJECT

public:
    explicit carinfo(QWidget *parent = nullptr);
    ~carinfo();
signals:
    void zhuxiao();

private slots:
    void on_pushButton_6_clicked();

    void on_insert_button_clicked();

    void on_query_a_button_clicked();

    void on_query_button_clicked();

private:
    Ui::carinfo *ui;
};

#endif // CARINFO_H
