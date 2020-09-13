#include "ui_binclass.h"
#include"binclass.h"
binclass::binclass(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::binclass)
{
    ui->setupUi(this);

}

binclass::~binclass()
{
    delete ui;
}

void binclass::on_binbutton_clicked()//垃圾箱切换回主页面
{
    emit reshow();//切换窗口2.0
    this->close();
}
