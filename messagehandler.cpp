#include "messagehandler.h"
#include "hash.h"
MessageHandler::MessageHandler(Hash *userTable){
    this->userTable = userTable;
}

//<$Goldenfiber:Lawrence$>Hello world // The message structure.void Hash::newMessage(User *u)
void MessageHandler::newMessage(QString qs){
        cout << QThread::currentThreadId() << "Message Handler"<<  endl;


            int index = qs.indexOf("$>");
            QString fromTo = qs.mid(2, index - 2);
            QString message = qs.mid(index + 2, qs.size() - 1);
            int index2 = fromTo.indexOf(":");
            QString to = fromTo.mid(index2 + 1, fromTo.size() - 1);
            QString from = fromTo.mid(0, index2);
            myMutex.lock();
            User *sendTo = userTable->getUser(to);
            //User *messageFrom = userTable->getUser(from);
            myMutex.unlock();
            if (sendTo != NULL){
                QString finalMessage = "<" + from + ">" + message;
                if (sendTo->connection == NULL){
                    sendTo->saveMessage(finalMessage);
                }
                else{
                    sendTo->triggerMessageSignal(finalMessage);
                }
            }


}




