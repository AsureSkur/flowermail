#include "regiclass.h"
#include "ui_regiclass.h"
#include"primclass.h"
regiclass::regiclass(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::regiclass)
{
    ui->setupUi(this);
}

regiclass::~regiclass()
{
    delete ui;
}


void regiclass::on_loginbutton_clicked()//注册切换登录
{
    emit reshow();//切换窗口2.0
    this->close();
}

void regiclass::on_pushButton_2_clicked()
{

}
