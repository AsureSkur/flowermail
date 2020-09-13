#include "recvclass.h"
#include "ui_recvclass.h"
#include "primclass.h"
#include"regiclass.h"
#include"binclass.h"
#include"homeclass.h"
#include"sendclass.h"
#include<QFile>
#include<QFileDialog>

recvclass::recvclass(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::recvclass)
{
    ui->setupUi(this);
}

recvclass::~recvclass()
{
    delete ui;
}

void recvclass::on_receivingbutton_clicked()//收件页面切换主页面
{
    emit reshow();
    this->close();
}

void recvclass::on_receivingcheck_clicked()
{

    QString path=QFileDialog::getOpenFileName(this,"open","../","");//打开文件夹
        if(path.isEmpty()==false)
        {
            QFile file(path);
            bool OK=file.open(QIODevice::ReadOnly);
            if(OK==true)
            {
                QByteArray array=file.readAll();//把本文传递到收件页面的本文框子里面
                ui->textEdit->setText(array);

            }
            file.close();//关闭file
        }

    /*QSqlQuery *sqlQuery = new QSqlQuery;
    QString sqlStr = "select id, name from info";
    sqlQuery->prepare( sqlStr );

    if( sqlQuery->exec() )
    {
       //读取查询到的记录
       while( sqlQuery->next() )
       {
           tableWidget->setItem( row, column, new QTableWidgetItem( sqlQuery->value(0).toString()));
           tableWidget->setItem( row, column, new QTableWidgetItem( sqkQuery->value(1).toString()))
       }
    }*/
}
