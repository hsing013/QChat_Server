#ifndef HASH_H
#define HASH_H

#include "include.h"
#include <vector>
#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <string>
#include <tuple>
#include <fstream>
#include <utility>
#include "mainwindow.h"
#include <fstream>
#include <QMutex>
#include <QThread>
#include <iostream>
#include <QWebSocket>
#include "friendhandler.h"
#include "messagehandler.h"
#include <QAbstractSocket>
#include <QMetaType>
#include <QHash>
#include <QTimer>
using namespace std;




struct User : public QObject{
    Q_OBJECT
public:
    QString userName;
    QString pass;
    QWebSocket *connection;
    User *next;
    vector<QString> savedMessages;
    int index;
    QMutex friendLock;
    vector<QString> friendList;
    QMutex messageLock;
    bool disconnectFlag;
    User();
    User(QString user, QString pass, QWebSocket *socket);
    ~User();
public slots:
    void messageReceived(QString s);
    void sendMessage(QString send);
    void socketDisconnected();
    void addFriend(QString myFriend);
    bool friendExists(QString myFriend);
    void saveMessage(QString s);
    void sendSavedMessages();
    void triggerMessageSignal(QString s);
signals:
    void needToDisconnect(User*);
    void errorWithUser(QString);
    void newMessage(QString);
    void getFriendList(User*);
    void addFriend(User*, QString);
    void sendNewMessage(QString);
};



//<$Goldenfiber:Lawrence$><%Hello world%> // The message structure.

class Hash : public QObject
{
    Q_OBJECT
public:
    Hash() {}
    Hash(MainWindow *w);
    ~Hash();
    bool buildUserTable();
    User* getUser(QString userName);
    tuple<bool, bool, User*> checkUser(QString userName, QString pass); //double role check if user exists and/or if the user is logged in
    void addUser(QString userName, QString pass, QWebSocket *socket, bool flag);
    void addUser(User *u, bool flag);
    void writeToFile(User *);
private:
    vector<User*> userTable;
    int numOfUsers;
    MainWindow *window;
public slots:

signals:

};

#endif

