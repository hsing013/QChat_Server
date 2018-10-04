#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
#include "mainwindow.h"
#include <QMainWindow>
#include <QThread>
#include <vector>
#include "include.h"
#include "hash.h"
#include <QHostAddress>
#include <tuple>
#include <utility>
#include "messagehandler.h"
#include <iostream>
#include <QWebSocket>
#include <QWebSocketServer>
#include "tempuser.h"
#include "friendhandler.h"
#include <QHash>
using namespace std;




class Server : public QObject
{
    Q_OBJECT
public:
    Server();
    ~Server();
private:
    QWebSocketServer *auth;
    MainWindow *window;
    QThread *worker;
    QThread *worker2;
    int activeUsers;
    Hash *userTable;
    MessageHandler *messageHandler;
    vector <TempUser*> currentConnections;
    FriendHandler *friendHandler;
public slots:
    void listen();
    void newConnection();
    void loginOrSignup(QString s, TempUser*);
    void removeConnection(TempUser*);
    QString reader(QTcpSocket *qts);
    void disconnectUser(User*);
    void disconnectUser(TempUser*);
signals:
};

#endif // SERVER_H
