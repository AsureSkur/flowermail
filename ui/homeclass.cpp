#include "homeclass.h"
#include "ui_homeclass.h"
#include "primclass.h"
#include"regiclass.h"
#include"binclass.h"
#include"recvclass.h"
#include"sendclass.h"
homeclass::homeclass(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::homeclass)
{
    ui->setupUi(this);
    connect(&send,&sendclass::reshow,[this]()//接受reshow信号并回馈，发送页面切换主页面
                {
                    send.close();
                    this->show();
                }
           );
    connect(&rece,&recvclass::reshow,[this]()//收信页面切换主页面
                {
                    rece.close();
                    this->show();
                }
           );
    connect(&bin,&binclass::reshow,[this]()//垃圾箱页面切换主页面
                {
                   bin.close();
                    this->show();
                }
           );
}

homeclass::~homeclass()
{
    delete ui;
}

void homeclass::on_sendbutton_clicked()//主页面切换发送页面
{
    this->hide();
    send.show();
}

void homeclass::on_reveivingbutton_clicked()//主页面切换接受页面
{
    this->hide();
    rece.show();
}

void homeclass::on_binbutton_clicked()//主页面切换垃圾箱页面
{
    this->hide();
    bin.show();
}
