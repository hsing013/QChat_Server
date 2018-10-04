#ifndef TEMPUSER_H
#define TEMPUSER_H
#include <QString>
#include <QObject>
#include <iostream>
#include <QWebSocket>
using namespace std;

struct TempUser : public QObject{
    Q_OBJECT
public:
    QWebSocket *conn;
    TempUser(){
        conn = NULL;
    }
    TempUser(QWebSocket *sock){
        conn = sock;
    }
private:

public slots:
    void recievedMessage(QString s){
        cout << "TempUser Message" << endl;
        emit request(s, this);
    }
    void disconnect(){
        cout << "TEMPUSER DISCONNECT" << endl;
        emit needToDisconnect(this);
    }
signals:
    void request(QString message, TempUser*);
    void needToDisconnect(TempUser*);
};

#endif // TEMPUSER_H
