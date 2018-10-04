#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H
#include "include.h"
#include <QTcpSocket>
#include <QObject>
#include <iostream>
#include <QThread>
#include <QMutex>
#include <tuple>
#include <utility>
#include <algorithm>
#include <queue>
using namespace std;

class Hash;

class MessageHandler : public QObject
{
    Q_OBJECT
public:
    MessageHandler() {}
    MessageHandler(Hash *userTable);
    ~MessageHandler() {}

signals:

public slots:
    void newMessage(QString);
private:
    Hash *userTable;
};

#endif // MESSAGEHANDLER_H
