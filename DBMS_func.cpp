#include <QCoreApplication>
#include <QSqlDatabase>
#include <QDebug>
#include <QStringList>
#include <QSqlQuery>
#include <time.h>
#include <string>

//表类型
enum TableType {USER = 0, MAIL, DRAFT};
//Mail中状态序列
enum Status {ISINJUNKBIN = 0, ISREAD};
//数据库数据分割 列
//Mail表的列名
enum MailValue { MID = 0,
                 MRECIPIENT,
                 MSENDER,
                 MSENDTIME,
                 MTITLE,
                 MTEXT,
                 MFILE,
                 MISINJUNKBIN,
                 MISREAD
               };
//User表的列名
enum UserValue { UID = 0,
                 UNAME,
                 UPASSWORD
               };
//Draft表的列名
enum DraftValue { DID = 0,
                  DTEXT
                };

//函数
    //连接操作
    QSqlDatabase connect_dbms(QString dbms, QString user,QString password);//数据库连接
    //数据插入操作
    void dbms_insert(QSqlQuery query, QString Uid, QString Uname, QString Upasswd);//数据插入表User
    void dbms_insert(QSqlQuery query, QString Mid, QString Mrecipientid,           //数据插入表Mail
                     QString Msenderid, QString Mtitle, QString Mtext,
                     QString Mfile, QString Misinjunkbox, QString Misread);
    void dbms_insert(QSqlQuery query, QString Did, QString Dtext);                 //数据插入表Draft
    //数据更新操作
        //疑似只需要进行：更新密码、更新草稿箱、更新是否在垃圾箱中、更新是否阅读
        //alter:更新内容  tabletype:表类型(0:User, 2:Draft)
    void dbms_update(QSqlQuery query, QString alter, int tabletype);               //更新数据表(User,Draft)中的内容
        //seq:状态序列（0:IsInJunkbin, 1:IsRead）status:状态(T/F)
    void dbms_update(QSqlQuery query, int seq, bool status );                      //更新邮件状态
    //数据删除
        //type:表类型(0:User,1:Mail,2:Draft) condition:删除约束条件(sql语句中Where后面的部分)
    void dbms_delete(QSqlQuery query, int tabletype, QString condition);           //删除数据表中的某个内容

//


//数据库的连接
QSqlDatabase connect_dbms(QString dbms, QString user,QString password){

    // 输出可用数据库
    qDebug() << "Available drivers:";
    QStringList drivers = QSqlDatabase::drivers();
    foreach(QString driver, drivers)
        qDebug() << driver;

    // 打开MySQL
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    //数据库的登陆
    db.setHostName("localhost");//登陆ip
    db.setDatabaseName(dbms);//登陆的数据库
    db.setUserName(user);//登陆的用户
    db.setPassword(password);//登陆密码


    if (!db.open())
        qDebug() << "Failed to connect to root mysql admin";
    else
        qDebug() << "Succussfully Open";

    return db;
}

//数据库操作
    void dbms_insert(QSqlQuery query, QString Uid, QString Uname, QString Upasswd ){
           //组合命令
           QString exec = "insert into User values("
                           + Uid + ","
                           +"'" + Uname + "',"
                           +"'" + Upasswd + "')";
           //判断是否写入成功
           if(query.exec(exec)) qDebug() << "Successfully insert data into User";
           else                 qDebug() << "Fail to insert data";

    }
    void dbms_insert(QSqlQuery query, QString Mid,//数据插入表Mail
                     QString Mrecipientid, QString Msenderid,
                     QString Mtitle, QString Mtext,
                     QString Mfile = "null",
                     QString Misinjunkbox = "0",
                     QString Misread = "0"){

        if(Mfile == "null"){        }
        else Mfile == "'" + Mfile + "'";

        //获取当前时间
        time_t timep;
        struct tm *p;
        time (&timep);
        p=gmtime(&timep);
        int sec = p->tm_sec;
        int min = p->tm_min;
        int hour = 8+p->tm_hour;
        int day = p->tm_mday;
        int month = 1+p->tm_mon;
        int year = 1900+p->tm_year;
        //将时间转为字符串类型
        QString s_year = QString::number(year,10);
        QString s_mon = month >= 10 ? QString::number(month,10) : "0" + QString::number(month,10);
        QString s_day = day >= 10 ? QString::number(day,10) : "0" + QString::number(day,10);
        QString s_hour = hour >= 10 ? QString::number(hour,10) : "0" + QString::number(hour,10);
        QString s_min = min >= 10 ? QString::number(min,10) : "0" + QString::number(min,10);
        QString s_sec = sec >= 10 ? QString::number(sec,10) : "0" + QString::number(sec,10);
        //组合时间 形成 数据库中 datetime 类型
        QString Msendtime = "'" + s_year + "-"
                            + s_mon + "-"
                            + s_day + " "
                            + s_hour + ":"
                            + s_min + ":"
                            + s_sec + "'";
        //组合命令
        QString exec = "insert into Mail values("
                        + Mid + ","
                        + Mrecipientid + ","
                        + Msenderid + ","
                        + Msendtime + ","
                        +"'" + Mtitle + "',"
                        +"'" + Mtext + "',"
                        + Mfile + ","
                        + Misinjunkbox + ","
                        + Misread + ")";
        //写入数据并返回是否成功
        if(query.exec(exec)) qDebug() << "Successfully insert data into Mail";
        else                 qDebug() << "Fail to insert data";
    }
    void dbms_insert(QSqlQuery query, QString Did, QString Dtext){
        //组合命令
        QString exec = "insert into Draft values("
                        + Did + ","
                        + "'" + Dtext + "')";
        //判断是否写入成功
        if(query.exec(exec)) qDebug() << "Successfully insert data into Draft";
        else                 qDebug() << "Fail to insert data";
    }
