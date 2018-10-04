#ifndef FRIENDHANDLER_H
#define FRIENDHANDLER_H
#include <QObject>
#include <QString>
#include <iostream>
#include "hash.h"
#include "include.h"

using namespace std;

struct User;
class Hash;

class FriendHandler : public QObject
{
    Q_OBJECT
public:
    FriendHandler();
    FriendHandler(Hash *table);
public slots:
    void sendFriendList(User*);
    void addFriend(User*, QString);
signals:

private:
    Hash *userTable;
};

#endif // FRIENDHANDLER_H
