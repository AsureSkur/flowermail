#include "smtp.h"

/**
  *Class constructor
  *@param from QString containing the email sender address
  *@param to QString containing email recipient address
  *@param subject QString containing email subject
  *@param body Qstring containing email content
  */
Smtp::Smtp(mainwindow *w,const QString &from, const QString &to,const QString &subject,const QString &body)
{
    widget = w;
    //create a socket
    socket = new QTcpSocket(this);

    //Connect signal & slot pairs
    connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()));
    connect(socket,SIGNAL(connected()),this,SLOT(connected()));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(errorReceived(QAbstractSocket::SocketError)));
    connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));

    //Create the message
    message = QString::fromLatin1("From: ") + from
              + QString::fromLatin1("\nto: ") + to +
                QString::fromLatin1("\nSubject: ") + subject +
                QString::fromLatin1("\n\n") + body + "\n";
    message.replace(QString::fromLatin1("\r\n.\r\n"),
                    QString::fromLatin1("\r\n..\r\n"));
    this->from = from;
    rcpt = to;
    socket->connectToHost("smtp.netplaza.fi",25);
    state = Init;
    textStream = new QTextStream(socket);
}
/**
  *Class destructor. 
  */
Smtp::~Smtp()
{
    delete socket;
    socket = NULL;
    delete textStream;
    textStream = NULL;
}

/**
  *This slot function is called when client is connected to SMTP server
  */
void Smtp::connected()
{
    widget->statusBar()->setWindowTitle("Connected");
}

/**
  *This slot function is called when connection to server is disposed
  */
void Smtp::disconnected()
{
    widget->statusBar()->setWindowTitle("Disconnected");
}

/**
  *This slot function is called if somethig goes wrong
  *@param socketErrot enum value defining the error
  */
void Smtp::errorReceived(QAbstractSocket::SocketError socketErrot)
{
}
/**
  *This slot function is called when ever socket state changes
  *@param socketState enum value defining the socket state
  */
void Smtp::stateChanged(QAbstractSocket::SocketState socketState)
{
    //Here we trace our socket state...
    //Possible states
    //QAbstractSocket::UnconnectedState
    //QAbstractSocket::HostLookupState
    //QAbstractSocket::ConnectingState
    //QAbstractSocket::ConnectedState
    //QAbstractSocket::BoundState
    //QAbstractSocket::ClosingState
    //QAbstractSocket::ListeningState (for internal use only)
}
/**
  *This slot function is called every time when QTcpSocekt readyRead() signal
  *is emitted.
  *This signal is emitted once every time new data is available for reading
  *from the device. It will only be emitted again once new data is available,
  *such as when a new payload of network data has arrived on your network socket,
  *or when a new block of data has been appended to your device.
  */
void Smtp::readyRead()
{
    //First lets check that whole line of data can be read from socket
    if(!socket->canReadLine())
    {
        //Data not ready, return to wait next readReady() signal
        return;
    }


    QString lineOfData;
    //Data ready, read the response from server
    lineOfData = socket->readLine();
    response += lineOfData;
    //As you can see form below codes 250,354 (for data) and 220 (for banner) are Ok responses from
    //server. So we check if response from server contains some other code we show
    //warning message and return -> no need to continue.
    if(!lineOfData.contains("250",Qt::CaseInsensitive) &&
       !lineOfData.contains("354",Qt::CaseInsensitive) &&
       !lineOfData.contains("220",Qt::CaseInsensitive))
    {
        QMessageBox::warning(widget,"Error in email sending",
                             "Unexpected reposne form server:\n" + response);
        /*
          Schedules this object for deletion.
          The object will be deleted when control returns to the event loop.
          If the event loop is not running when this function is called
          (e.g. deleteLater() is called on an object before QCoreApplication::exec()),
          the object will be deleted once the event loop is started.*/
        deleteLater();
        return;
    }

    //Switch the state in beginning we are in state init
    switch(state)
    {
        case Init:
        {
            //banner okay write to socket HELO to make a handshake with serevr
            *textStream << "HELO there\r\n";
            //Flush any buffered data waiting to be written to the device.
            textStream->flush();
            state = Mail;
        }
        break;
        case Mail:
        {
            //Hello was ok, write from data
            *textStream << "MAIL FROM: <" << from << ">\r\n";
            textStream->flush();
            state = Rcpt;
        }
        break;
        case Rcpt:
        {
            //Write recipient data
            *textStream << "RCPT TO: <" << rcpt << ">\r\n";
            textStream->flush();
            state = Data;
        }
        break;
        case Data:
        {
            //Write data
            *textStream << "DATA\r\n";
            textStream->flush();
            state = Body;
        }
        break;
        case Body:
        {
            //Write message
            *textStream << message << ".\r\n";
            textStream->flush();
            state = Quit;
        }
        break;
        case Quit:
        {
            //Close connection to server
            *textStream << "QUIT\r\n";
            state = Close;
        }
        break;
        case Close:
        {
            deleteLater();
            return;
        }
        default:
        {
            QMessageBox::warning(widget,"Error in email sending",
                             "Unknown state:\n" + state);
        }
    }
    response = "";
}

