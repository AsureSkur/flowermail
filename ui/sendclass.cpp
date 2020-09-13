#include "sendclass.h"
#include "ui_sendclass.h"
#include "primclass.h"
#include"regiclass.h"
#include"binclass.h"
#include"homeclass.h"
#include"recvclass.h"
sendclass::sendclass(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sendclass)
{
    ui->setupUi(this);
}

sendclass::~sendclass()
{
    delete ui;
}

void sendclass::on_sendbutton_clicked()//发送页面切换主页面
{
    emit reshow();
    this->close();
}
