#include "regiclass.h"
#include "ui_regiclass.h"
#include"primclass.h"

#include <QMessageBox>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define SERVER_PORT 6666
#define CONNECT_CLOSE "end"
#define SERVER_ADDR "127.0.0.1"

int wait_for_recv(int client,char* recvbuf);
int socket_register(char* username,char* password);

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

void regiclass::on_regibutton_clicked()
{
    QString uname = ui->uname->text();
    QString upwd1 = ui->upwd1->text();
    QString upwd2 = ui->upwd2->text();
    char username[200],password[200];

    if(QString::localeAwareCompare(upwd1, upwd2) != 0)
    {
        QMessageBox::about(NULL, "REGISTER ERROR", "Password check error!");
    }

    std::string str;

    str= uname.toStdString();
    strcpy(username,str.c_str());

    str= upwd1.toStdString();
    strcpy(password,str.c_str());

    if(socket_register(username,password)==0)
    {
        QMessageBox::about(NULL, "REGISTER SUCCEED", "Welcome to flowermail!");
        emit reshow();//切换窗口2.0
        this->close();
    }
    else
    {
        QMessageBox::about(NULL, "REGISTER FAILED", "Something wrong occured!");
    }
}
int wait_for_recv(int client,char* recvbuf)
{
    int idatanum;
    while(1)
    {
        recvbuf[0] = '\0';
        idatanum = recv(client, recvbuf, 1024, 0);
        if (idatanum < 0)
        {
            perror("recv null");
            continue;
        }
        recvbuf[idatanum] = '\0';
        printf("recv:%s\n",recvbuf);
        break;
    }
    return 0;
}
/*
//socket used for register, send username and password,
//if username repeated show "repeated" and return 2,
//if succeed return 0,
//if something error, return -1; if connection failed halfway, return 1;
//if no input(username or password is null), return 3;
*/
int socket_register(char username[200],char password[200])
{
    if(strlen(username)<=0||strlen(password)<=0)
    {
        printf("No input!\n");
        return 3;
    }
    //客户端只需要一个套接字文件描述符，用于和服务器通信
    int client;

    //描述服务器的socket
    struct sockaddr_in serverAddr;
    char sendbuf[200];
    char recvbuf[200];

    if ((client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    //指定服务器端的ip，本地测试：127.0.0.1
    //inet_addr()函数，将点分十进制IP转换成网络字节序IP
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    if (connect(client, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("connect");
        return -1;
    }

    printf("连接到主机...\n");
    strcpy(sendbuf,"register");
    send(client, sendbuf, strlen(sendbuf), 0);

    wait_for_recv(client,recvbuf);
    if(strcmp(recvbuf,"okforregister")!=0)
    {
        strcpy(sendbuf,CONNECT_CLOSE);
        send(client, sendbuf, strlen(sendbuf), 0);
        close(client);
        printf("Register failed!\n");
        close(client);
        return 1;
    }
    //send username for register to the server
    strcpy(sendbuf,username);
    send(client, sendbuf, strlen(sendbuf), 0);

    wait_for_recv(client,recvbuf);
    if(strcmp(recvbuf,"usernameavailble")!=0)
    {
        strcpy(sendbuf,CONNECT_CLOSE);
        send(client, sendbuf, strlen(sendbuf), 0);
        close(client);
        printf("Username has existed!\n");
        close(client);
        return 2;
    }
    //send password for register to the server
    strcpy(sendbuf,password);
    send(client, sendbuf, strlen(sendbuf), 0);
    wait_for_recv(client,recvbuf);
    if(strcmp(recvbuf,"registersucceed")!=0)
    {
        strcpy(sendbuf,CONNECT_CLOSE);
        send(client, sendbuf, strlen(sendbuf), 0);
        close(client);
        printf("Register failed!\n");
        close(client);
        return 1;
    }
    printf("Register succeed!\n");
    close(client);
    return 0;
}

