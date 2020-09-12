void login::on_pushButton_sign_in_clicked()
{
    QString username = ui->lineEdit_login_login->text();
    QString password = ui->lineEdit_password_login->text();

    QSqlQuery query;
    query.prepare("SELECT userID, name from users WHERE 
                    username = :username AND 
                    password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    if (query.exec())
    {
        if (query.size() > 0)
        {
            //  login a user here
            QString name = query.value(1).toString();
            qDebug() << name << "is logged in";
        }
        else
        {
            qDebug() << "Login failed. Invalid username or password.";
        }
    }
}