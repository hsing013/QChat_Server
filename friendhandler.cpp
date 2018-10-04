#include "friendhandler.h"

FriendHandler::FriendHandler()
{
    userTable = NULL;
}

FriendHandler::FriendHandler(Hash *table){
    userTable = table;
}

void FriendHandler::sendFriendList(User *u){
    cout << QThread::currentThreadId() << "Friend Handler"<<  endl;
    QString fList = "<$FRIENDLIST$>";
    for (unsigned i = 0; i < u->friendList.size(); ++i){
        QString current = u->friendList.at(i);
        if (i == u->friendList.size() - 1){
            fList = fList + current;
            continue;
        }
        fList = fList + current + ",";
    }
    u->triggerMessageSignal(fList);

    cout << "I came out of that FL" << endl;
}


void FriendHandler::addFriend(User *u, QString personToAdd){
    cout << QThread::currentThreadId() << "Friend 2 Handler"<<  endl;
    myMutex.lock();
    User *toAdd = userTable->getUser(personToAdd);
    myMutex.unlock();
    if (toAdd == NULL || u->friendExists(personToAdd)){
        u->triggerMessageSignal(QString("<$ADDRESULT$>0"));
    }
    else{
        toAdd->addFriend(u->userName);
        u->addFriend(personToAdd);
        if (toAdd->connection != NULL){
            toAdd->triggerMessageSignal(QString("<$NEWFRIEND$>") + u->userName);
        }
        u->triggerMessageSignal(QString("<$ADDRESULT$>1"));
    }

}
