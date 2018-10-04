#include "server.h"

Server::Server()
{
    activeUsers = 0;
    window = new MainWindow();
    worker = new QThread();
    worker2 = new QThread();
    auth = new QWebSocketServer(QString("MainServer"), QWebSocketServer::NonSecureMode);
    userTable = new Hash(window);
    userTable->buildUserTable();
    messageHandler = new MessageHandler(userTable);
    messageHandler->setParent(0);
    messageHandler->moveToThread(worker);
    friendHandler = new FriendHandler(userTable);
    friendHandler->setParent(0);
    friendHandler->moveToThread(worker2);
    window->updateLCD(activeUsers);
    window->show();
    worker->start();
    worker2->start();

}

Server::~Server(){
    cout << "I was called" << endl;
    worker->quit();
    worker2->quit();
    worker->wait();
    worker2->wait();
    auth->close();
    delete auth;
    delete userTable;
    delete messageHandler;
    delete friendHandler;
}

void Server::listen(){
    auth->setMaxPendingConnections(1000);
    connect(auth, SIGNAL(newConnection()), this, SLOT(newConnection()));
    QHostAddress address(QString("192.168.1.19"));
    auth->listen(address, 12345);

}

void Server::newConnection(){
    cout << QThread::currentThreadId() << "ServerConnection"<<  endl;
    QWebSocket *qts = auth->nextPendingConnection();
    qts->setParent(0);
    TempUser *user = new TempUser();
    user->setParent(0);
    connect(user, SIGNAL(request(QString,TempUser*)), this, SLOT(loginOrSignup(QString,TempUser*)));
    connect(user, SIGNAL(needToDisconnect(TempUser*)), this, SLOT(disconnectUser(TempUser*)));
    user->conn = qts;
    connect(qts, SIGNAL(textMessageReceived(QString)), user, SLOT(recievedMessage(QString)));
    connect(qts, SIGNAL(disconnected()), user, SLOT(disconnect()));
    currentConnections.push_back(user);

}

void Server::loginOrSignup(QString qs, TempUser *tempUser){
    cout << "LOGIN OR SIGNUP IS HAPPENING" << endl;
    QWebSocket *qts = tempUser->conn;
    if (!qts->isValid()){
        removeConnection(tempUser);
        delete qts;
        return;
    }
    QString user, pass;
    int index;

    QString code = qs.mid(0, 3);
    index = qs.indexOf(" ");
    user = qs.mid(3, index - 3);
    pass = qs.mid(index + 1);
    if (code == "661"){
        myMutex.lock();
        tuple<bool, bool, User*> myTuple;
        myTuple = userTable->checkUser(user, pass);
        User *u = get<2>(myTuple);
        if (get<0>(myTuple) == false || get<1>(myTuple) == false || get<2>(myTuple) == NULL){
            //cout << myPair.first << " " << myPair.second << endl;
            qts->sendTextMessage(QString("0"));
        }
        else{
            cout << disconnect(tempUser, 0, 0, 0) << endl;
            removeConnection(tempUser);
            cout << disconnect(qts, 0, 0, 0) << endl;
            ++activeUsers;
            u->connection = qts;
            qts->sendTextMessage(QString("1"));
            window->updateLCD(activeUsers);
            connect(u, SIGNAL(newMessage(QString)), messageHandler, SLOT(newMessage(QString)));
            connect(u, SIGNAL(getFriendList(User*)), friendHandler, SLOT(sendFriendList(User*)));
            connect(u, SIGNAL(addFriend(User*,QString)), friendHandler, SLOT(addFriend(User*,QString)));
            connect(u, SIGNAL(needToDisconnect(User*)), this, SLOT(disconnectUser(User*)));
            connect(qts, SIGNAL(textMessageReceived(QString)), u, SLOT(messageReceived(QString)));
            connect(qts, SIGNAL(disconnected()), u, SLOT(socketDisconnected()));
            connect(u, SIGNAL(sendNewMessage(QString)), u, SLOT(sendMessage(QString)));

            u->disconnectFlag = false;


            cout << "ALL set" << endl;
            myMutex.unlock();

            return;
        }
        myMutex.unlock();
    }
    else if (code == "662"){
        myMutex.lock();
        User *u = userTable->getUser(user);
        if (u == NULL){
            qts->sendTextMessage(QString("1"));
            userTable->addUser(user, pass, NULL, true);
        }
        else{
            qts->sendTextMessage(QString("0"));
        }
        myMutex.unlock();
    }
}

void Server::removeConnection(TempUser *user){
    int index = -1;
    for (unsigned i = 0; i < currentConnections.size(); ++i){
        TempUser *temp = currentConnections.at(i);
        if (temp == user){
            index = i;
            break;
        }
    }
    if (index != -1){
        currentConnections.erase(currentConnections.begin() + index);
        user->deleteLater();
    }
    else{
        window->updateConsole(QString("Error: Temp User not found in vector."));
        user->deleteLater();
    }
}

QString Server::reader(QTcpSocket *qts){
    char data[4096];
    qint64 read = qts->read(data, sizeof(data));
    if(read != -1){
        return QString(data);
    }

    return "-1";
}

void Server::disconnectUser(User *u){
    cout << QThread::currentThreadId() << "Disconnect"<<  endl;
    --activeUsers;
    myMutex.lock();
    QWebSocket *conn = u->connection;
    disconnect(u, 0, 0, 0);
    disconnect(conn, 0, 0, 0);
    connect(u, SIGNAL(errorWithUser(QString)), window, SLOT(updateConsole(QString)));
    u->connection = NULL;
    conn->deleteLater();
    window->updateLCD(activeUsers);
    u->disconnectFlag = false;
    myMutex.unlock();
    cout << "DISCONNECT COMPLETE" << endl;

}

void Server::disconnectUser(TempUser *user){
    cout << "Disconnect temp user" << endl;
    disconnect(user, 0, 0, 0);
    user->conn->deleteLater();
    removeConnection(user);
}




