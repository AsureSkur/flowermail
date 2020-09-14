#include <QCoreApplication>
#include <QSqlDatabase>
#include <QDebug>
#include <QStringList>
#include <QSqlQuery>
#include <time.h>
#include <cstring>
#include <QFile>
#include <QDir>
#include <vector>
/*
 *  看一眼line 474！！  与文件传输相关
    调用函数前需要创建一个db实例与query实例
    调用函数时第一个参数为query实例
    例子:
    QSqlDatabase db = connect_dbms("flowermail", "root", "123456");
    QSqlQuery query(db);

    在完成某个操作之后需要进行数据库关闭
    例子:
    db.close();

    建议在打开信箱之后将数据保存在全局变量中

    宏、枚举、函数从 下方开始  均有注释说明功能
*/

//以下为数据库的创建
//数据库需要先手动创建:create database flowermail;
//创建用户名 完成
   /* query.exec("create table User"
           "("
           "Uid int primary key,"
           "Uname char(16) not null,"
           "Upasswd char(16) not null"
           ") ENGINE=InnoDB DEFAULT CHARSET=utf8"
           "");*/
    /*
     * 手动补:alter table User modify column Uname char(16) unique;
    */
//创建邮件表 完成
    /*query.exec("create table Mail"
           "("
           "Mid int primary key,"
           "Mrecipientid int ,"
           "Msenderid int ,"
           "Msendtime datetime not null,"
           "Mtitle varchar(40) not null,"
           "Mtext varchar(2000),"
           "Mfile mediumblob,"
           "Misinjunkbox bool,"
           "Misread bool,"
           "foreign key (Mrecipientid) references User(Uid),"
           "foreign key (Msenderid) references User(Uid)"
           ") ENGINE=InnoDB DEFAULT CHARSET=utf8"
           "");*/
    /*
     * 手动补:alter table Mail modify column Mrecipientid int not null;
     *      :alter table Mail modify column Msenderid int nut null;
            */
//创建草稿箱 完成
    /*query.exec("create table Draft"
           "("
           "Did int,"
           "Dtext varchar(2000),"
           "foreign key (Did) references User(Uid)"
           ") ENGINE=InnoDB DEFAULT CHARSET=utf8"
           "");*/


//以下为数据库相关宏、枚举、函数

//数据库相关信息
#define CONNECTION "QMYSQL"   //数据库连接类型
//#define DBMS "flowermail"   //数据库名称
//#define DBMS_USER "root"    //数据库用户
//#define DBMS_PASSWD "123456"//数据库密码

#define CHECKED 1   //返回信息
#define CANT_FIND_USER 0   //找寻不到用户
#define CANT_MATCH_PASSWORD 1 //密码不匹配
#define ALREADY_EXIST -2    //用户已存在(用于注册判断)
#define CANT_ADD_FILE_ADDR -3 //无法添加文件路径进数据库
#define CANT_FIND_FILE -4   //在查找文件时找寻不到
#define CONNECTION_FAILD 404  //连接不到数据库
//#define

//表类型
enum TableType {USER = 0, MAIL, DRAFT};
//Mail中状态序列
enum Status {ISINJUNKBOX = 0, ISREAD};
//数据库数据分割 列
//Mail表的列名
enum MailValue { MID = 0,
                 MRECIPIENTID,
                 MSENDERID,
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
    void dbms_update(QSqlQuery query, QString Uid, QString alter, int tabletype);               //更新数据表(User,Draft)中的内容
        //seq:状态序列（0:IsInJunkbin, 1:IsRead）status:状态(T/F)
    void dbms_update(QSqlQuery query, QString Mid, int seq, bool status );                      //更新邮件状态

    //数据删除
        //应该只有删邮件？？
        //type:表类型(0:User,1:Mail,2:Draft) condition:删除约束条件(sql语句中Where后面的部分)
    void dbms_delete(QSqlQuery query, QString Mid, int tabletype);           //删除数据表中的某个内容

    //数据查询
    QString dbms_get_data_from_user(QSqlQuery query, int column, QString Uname);//查询user表
    QString dbms_get_uname_by_uid(QSqlQuery query, QString Uid);//通过uid查询用户名
        //查询后返回一个数据库表，需要使用 query.next() 与 query.value(int column) 得到字符串
    QSqlQuery dbms_get_query_from_mail_recipientid(QSqlQuery query, QString recipient_id);//根据收件人获得信件信息（用于查看信箱）
    QSqlQuery dbms_get_query_from_mail_senderid(QSqlQuery query, QString sender_id);//根据发件人获得信件信息（用于查看发送信件）

    QString dbms_get_file_addr(QSqlQuery query, QString Mid);//获取文件储存地址

    //获取注册人数（用于自动生成UID）
    int dbms_get_user_number(QSqlQuery query);
    //各种实际功能
    int check_username(QSqlQuery query, QString username);                  //判断用户名是否存在
    int check_password(QSqlQuery query, QString username, QString password);//判断密码与用户名是否匹配
    int user_register (QSqlQuery query, QString username, QString password);//注册相关
    int send_mail(QSqlQuery query, QString receiverid, QString senderid,    //发送邮件
                  QString title, QString text);
    bool check_mail_in_junkbox(QSqlQuery query, QString Mid);               //根据邮件id判断是否在垃圾箱中
    bool check_mail_read(QSqlQuery query, QString Mid);                     //根据邮件id判断是否被阅读

    int  update_file_address(QSqlQuery query, QString Mid);                  //根据邮件id更改文件地址（插入时文件地址为null，如果有文件进行更新）
    void create_new_folder(QString storage, QString Mid);                   //传入存储地址与Mid，在该目录下创建一个文件夹

    std::vector<std::string> get_mail_and_sender_by_receiver        //获取收件人下 邮件id 邮件名 发件人名称
        (QSqlQuery query,QSqlQuery tempquery, QString receiverid);
    QString get_mail_text_by_mid(QSqlQuery query, QString Mid); //根据Mid获取信件正文
//


//数据库的连接
QSqlDatabase connect_dbms(QString dbms, QString user,QString password){

    // 输出可用数据库
    qDebug() << "Available drivers:";
    QStringList drivers = QSqlDatabase::drivers();
    foreach(QString driver, drivers)
        qDebug() << driver;

    // 打开MySQL
    QSqlDatabase db = QSqlDatabase::addDatabase(CONNECTION);

    //数据库的登陆
    db.setHostName("localhost");//登陆ip
    db.setDatabaseName(dbms);//登陆的数据库
    db.setUserName(user);//登陆的用户
    db.setPassword(password);//登陆密码


    if (!db.open())
        qDebug() << "Failed to connect to root mysql admin";
    else
        qDebug() << "DBMS Succussfully Open";

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
           qDebug() << "insert command:" << exec;
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

    //alter:更新内容  tabletype:表类型(0:User, 2:Draft)
    void dbms_update(QSqlQuery query, QString Uid, QString alter, int tabletype){               //更新数据表(User,Draft)中的内容

        QString exec;

        switch (tabletype){
            case USER:
                //组合命令 更新密码
                exec = "update User set Upasswd = '"
                     + alter + "' where Uid = " + Uid;
                break;
            case MAIL:
                break;
            case DRAFT:
                //组合命令 更新草稿
                exec = "update Draft set Dtext = '"
                        + alter + "' where Did = " + Uid;
                break;
            default : break;
        }

        //log输出
        if(query.exec(exec)) {
            qDebug() << "Successfully update data";
        }
        else {
            qDebug() << "Fail to update data";
        }

    }

    //seq:状态序列（0:IsInJunkbox, 1:IsRead）status:状态(T/F)
    void dbms_update(QSqlQuery query, QString Mid, int seq, bool status ){                    //更新邮件状态
        QString exec;
        QString sta;
        if(status){
            sta = "true";
        }
        else {
            sta = "false";
        }

        switch (seq){
            case ISINJUNKBOX:
                exec = "update Mail set Misinjunkbox = "
                     + sta + " where Mid = " + Mid;
                if(status) {
                    dbms_update(query, Mid, ISREAD, true);  //如果处于垃圾箱中, 则设置成 已读
                }
                break;
            case ISREAD:
                exec = "update Mail set Misread = "
                     + sta + " where Mid = " + Mid;
                break;
            default: break;
        }
        //qDebug() << exec;
        if(query.exec(exec)) qDebug() << "Successfully change mail status";
        else                 qDebug() << "Fail to change mail status";
    }

    //数据删除
        //应该只有删邮件？？
        //type:表类型(0:User,1:Mail,2:Draft) condition:删除约束条件(sql语句中Where后面的部分)(应该只有Mid为条件)
    void dbms_delete(QSqlQuery query, QString Mid, int tabletype){           //删除数据表中的某个内容
            QString exec;
            switch (tabletype) {
                case USER:
                    break;
                case MAIL:
                    exec = "delete from Mail where Mid = " + Mid;
                    break;
                case DRAFT:
                    break;
                default: break;
            }

            if(query.exec(exec)) qDebug() << "Successfully delete mail";
            else                 qDebug() << "Fail to delete mail";
        }

    QString dbms_get_data_from_user(QSqlQuery query, int column, QString Uname){       //查询user表

        QString exec;
        switch (column){
            case UID:
                exec = "select Uid from User where Uname = '" + Uname + "'"; //查询uid匹配
                break;
            case UNAME:
                exec = "select Uname from User where Uname = '" + Uname + "'"; //查询用户名匹配
                break;
            case UPASSWORD:
                exec = "select Upasswd from User where Uname = '" + Uname + "'"; //查询密码匹配
                break;
            default:
                break;
        }
        if(!query.exec(exec)) {
            qDebug() << "Fail to get data";
            return NULL;
        }
        else {
            query.next();
            QString data = query.value(0).toString();
            qDebug() << "Successfully get data from Uname : " + data;
            return data;
        }
    }

    //查询Mail表
    QSqlQuery dbms_get_query_from_mail_recipientid(QSqlQuery query, QString recipient_id){//根据收件人获得信件信息（用于查看信箱）
        QString exec = "select * from Mail where Mrecipientid = " + recipient_id;
        query.exec(exec);
        return query;
    }
    QSqlQuery dbms_get_query_from_mail_senderid(QSqlQuery query, QString sender_id){    //根据发件人获得信件信息（用于查看发送信件）
        QString exec = "select * from Mail where Msenderid = " + sender_id;
        query.exec(exec);
        return query;
    }


    //获取注册人数(以分配uid)
    int dbms_get_user_number(QSqlQuery query){
        QString exec = "select Uid from User";
        query.exec(exec);
        //人数
        int num = 0;
        while(query.next()){
            num++;
        }
        qDebug() << "user number" << num;
        return num;
    }

    //检测是否存在用户名
    int check_username(QSqlQuery query, QString username){
        QString data = dbms_get_data_from_user(query, UNAME, username);
        if(data != NULL){
            return CHECKED;
        } else {
            return CANT_FIND_USER;
        }

    }
    //检测用户名是否和密码匹配(先检测用户名是否存在再检测密码是否匹配,因此不用判断是否存在账户)
    int check_password(QSqlQuery query, QString username, QString password){
        QString Upasswd = dbms_get_data_from_user(query, UPASSWORD, username);
        if(Upasswd == password){
            return CHECKED;
        }
        else {
            return CANT_MATCH_PASSWORD;
        }
    }

    int user_register(QSqlQuery query, QString username, QString password){//注册相关
        int power_to_register = check_username(query, username);//是否能够注册
        //不重名 则分配uid 并创建表
        if (power_to_register == CANT_FIND_USER) {
            int number = dbms_get_user_number(query);
            dbms_insert(query, QString::number(number,10), username, password);
            return CHECKED;
        }
        //重名  则返回已存在
        else if (power_to_register == CHECKED){
            return ALREADY_EXIST;
        }
    }

    bool check_mail_in_junkbox(QSqlQuery query, QString Mid){//根据邮件id判断邮件是否位于垃圾箱
        QString exec = "select Misinjunkbox from Mail where Mid = " + Mid;

        if(!query.exec(exec)) return false;//如果无法处理
        query.next();
        if(query.value(0).toString() == "1"){
                return true;
        }
        else if(query.value(0).toString() == "0"){
            return false;
        }
        //无Mid会自动报error不用管
    }

    bool check_mail_read(QSqlQuery query, QString Mid){ //根据邮件id判断邮件是否被阅读
        QString exec = "select Misread from Mail where Mid = " + Mid;

        if(!query.exec(exec)) return false;//如果无法处理
        query.next();
        if(query.value(0).toString() == "1"){
            return true;
        }
        else if (query.value(0).toString() == "0"){
            return false;
        }
        //无Mid会自动报error不用管
    }

    //获取文件储存地址
    QString dbms_get_file_addr(QSqlQuery query, QString Mid){
        QString exec = "select Mfile from Mail where Mid = " + Mid;
        query.exec(exec);
        query.next();
        if(query.value(0).toString() == NULL){
            return NULL;
        } else {
            return query.value(0).toString();
        }
    }
    //将邮件的地址传入数据库并创建信件文件文件夹（将文件保存到该文件夹需要自己写！）
    int update_file_address(QSqlQuery query,QString content,QString Mid, QString filename){
        create_new_folder(content, Mid);
        QString exec = "update Mail set Mfile = '"
                     + content + "/" + Mid + "/" + filename
                     + "' where Mid = " + Mid;

        if(query.exec(exec)){
            return CHECKED;
        }
        else {
            return CANT_ADD_FILE_ADDR;
        }
    }
    void create_new_folder(QString content, QString Mid){                  //传入存储目录与Mid，在该目录下创建一个名字为Mid的文件夹
        QDir dir;
        QString dir_str = content + "/" + Mid;
        if(!dir.exists(dir_str)) dir.mkpath(dir_str);
    }

    int send_mail(QSqlQuery query, QString receiver, QString sender,    //发送邮件
                  QString title, QString text){
        //获取Uid
        if(!check_username(query, receiver)) return CANT_FIND_USER;     //无法找到收件人
        //检测完成 获取收件人与发件人id
        QString senderid = dbms_get_data_from_user(query, UID, sender);
        QString receiverid = dbms_get_data_from_user(query, UID, receiver);
        //自动生成Mid
        QString exec = "select Mid from Mail";
        query.exec(exec);
        int num = 0;
        while(query.next()){
            num++;
        }
        //插入数据库
        dbms_insert(query, QString::number(num,10), receiverid, senderid, title, text);
        return CHECKED;
    }

    QString dbms_get_uname_by_uid(QSqlQuery query, QString Uid){        //通过uid查询用户名
        QString exec = "select Uname from User where Uid = " + Uid;
        query.exec(exec);
        query.next();
        return query.value(0).toString();
    }

    //获取收件人下 邮件id 邮件名 发件人名称
    std::vector<std::string> get_mail_and_sender_by_receiver(QSqlQuery query,QSqlQuery tempquery, QString receivername){

        query.exec("select Uid from User where Uname = '" + receivername + "'");
        query.next();
        QString receiverid = query.value(0).toString();
        query = dbms_get_query_from_mail_recipientid(query, receiverid);

        std::vector<std::string> vec;
        while(query.next()){
            QString returnback;//插入vector的总语句
            QString temp1 = query.value(MID).toString();        //MID
            QString temp2 = query.value(MTITLE).toString();     //MTITLE
            QString temp3 = query.value(MSENDERID).toString();  //SENDERID

            temp3 = dbms_get_uname_by_uid(tempquery, temp3);
            returnback = temp1 + ";" + temp2 + ";" + temp3;
            //传入
            vec.push_back(returnback.toStdString());
        }
        return vec;
    }

    QString get_mail_text_by_mid(QSqlQuery query, QString Mid){ //根据Mid获取信件正文
        QString exec = "select Mtext from Mail where Mid = " + Mid;
        query.exec(exec);

        query.next();
        return query.value(0).toString();
    }

    /*
     * 数据库各表列类型
     *
     * mysql> desc User;
    +---------+----------+------+-----+---------+-------+
    | Field   | Type     | Null | Key | Default | Extra |
    +---------+----------+------+-----+---------+-------+
    | Uid     | int(11)  | NO   | PRI | NULL    |       |
    | Uname   | char(16) | NO   | UNI | NULL    |       |
    | Upasswd | char(16) | NO   |     | NULL    |       |
    +---------+----------+------+-----+---------+-------+
    mysql> desc Mail;
    +--------------+---------------+------+-----+---------+-------+
    | Field        | Type          | Null | Key | Default | Extra |
    +--------------+---------------+------+-----+---------+-------+
    | Mid          | int(11)       | NO   | PRI | NULL    |       |
    | Mrecipientid | int(11)       | NO   | MUL | NULL    |       |
    | Msenderid    | int(11)       | NO   | MUL | NULL    |       |
    | Msendtime    | datetime      | NO   |     | NULL    |       |
    | Mtitle       | varchar(40)   | NO   |     | NULL    |       |
    | Mtext        | varchar(2000) | YES  |     | NULL    |       |
    | Mfile        | varchar(50)   | YES  |     | NULL    |       |
    | Misinjunkbox | tinyint(1)    | YES  |     | NULL    |       |
    | Misread      | tinyint(1)    | YES  |     | NULL    |       |
    +--------------+---------------+------+-----+---------+-------+
    mysql> desc Draft;
    +-------+---------------+------+-----+---------+-------+
    | Field | Type          | Null | Key | Default | Extra |
    +-------+---------------+------+-----+---------+-------+
    | Did   | int(11)       | NO   | PRI | NULL    |       |
    | Dtext | varchar(2000) | YES  |     | NULL    |       |
    +-------+---------------+------+-----+---------+-------+
    */
