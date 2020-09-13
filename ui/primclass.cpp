#include "ui_primclass.h"
#include "primclass.h"
#include"regiclass.h"
#include"binclass.h"
#include"homeclass.h"
#include"recvclass.h"
#include"sendclass.h"
primclass::primclass(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::primclass)
{
    ui->setupUi(this);
    connect(&regi,&regiclass::reshow,[this]()//注册界面返回登录界面功能
                {
                    regi.close();
                    this->show();
                }
           );
}

primclass::~primclass()
{
    delete ui;
}


void primclass::on_registerbutton_clicked()//登陆切换注册
{
    this->hide();
    regi.show();
}


void primclass::on_loginbutton_clicked()//注册切换登录
{
    this->hide();
    login.show();
}
