#include "hash.h"



User::User(){
    userName = "";
    pass = "";
    connection = NULL;
    next = NULL;
    index = -1;
    disconnectFlag = true;

}

User::User(QString user, QString pass, QWebSocket *socket){
    userName = user;
    this->pass = pass;
    connection = socket;
    next = NULL;
    index = -1;
    disconnectFlag = false;
}

User::~User(){
    if (connection != NULL){
        connection->deleteLater();
    }
}

void User::messageReceived(QString s){
    cout << QThread::currentThreadId() << userName.toStdString() << "R" <<  endl;
    cout << "The message recieved " << s.toStdString() << endl;
    if (disconnectFlag){
        return;
    }
    if (connection == NULL){
        return;
    }
    if (s == "<$GETFL$>"){
        cout << "getting list" << endl;
        emit getFriendList(this);
        return;
    }
    QString split = s.mid(0, 7);

    if (split == "<$ADD$>"){
        cout << "adding a friend" << endl;
        QString personToAdd = s.mid(7);
        emit addFriend(this, personToAdd);
        return;
    }

    emit newMessage(s);
}

void User::sendMessage(QString send){
    cout << QThread::currentThreadId() << userName.toStdString() << "S" <<  endl;
    if (disconnectFlag){
        return;
    }
    if (connection == NULL){
        cout << "YUP" << endl;
        emit errorWithUser(QString("Socket is NULL. ") + userName + QString("\n"));
        return;
    }
    qint64 result = connection->sendTextMessage(send);
    cout << "RESULT: " << result << endl;
    if (result <= 0){
        cout << "RESULT WAS LESS THAN 0" << endl;
        messageLock.lock();
        savedMessages.push_back(send);
        messageLock.unlock();
        return;
    }
    cout << "I NEVER MAKE IT HERE, SAD SAUCE" << endl;
    messageLock.lock();
    for (unsigned i = 0; i < savedMessages.size(); ++i){
        qint64 result = connection->sendTextMessage(savedMessages.at(i));
        if (result <= 0){
            cout << "RESULT WAS LESS THAN 0(3)" << endl;
            messageLock.unlock();
            return;
        }
    }
    savedMessages.resize(0);
    messageLock.unlock();
}
void User::socketDisconnected(){
    qDebug() << "SOCKET DISCONNECTED" << endl;
    disconnectFlag = true;
    emit needToDisconnect(this);
}
void User::addFriend(QString myFriend){
    bool status = friendExists(myFriend);
    if (status){
        return;
    }
    fileMutex.lock();
    ofstream out;
    out.open("friends.txt", ios::app);
    string output = userName.toStdString() + ":" + myFriend.toStdString();
    out << output << endl;
    out.close();
    fileMutex.unlock();
    friendLock.lock();
    friendList.push_back(myFriend);
    friendLock.unlock();
}
bool User::friendExists(QString myFriend){
    friendLock.lock();
    for (unsigned i = 0; i < friendList.size(); ++i){
        if (myFriend == friendList.at(i)){
            friendLock.unlock();
            return true;
        }
    }
    friendLock.unlock();
    return false;
}
void User::saveMessage(QString s){
    messageLock.lock();
    savedMessages.push_back(s);
    messageLock.unlock();
}
void User::sendSavedMessages(){
    if (disconnectFlag){
        return;
    }
    messageLock.lock();
    for (unsigned i = 0; i < savedMessages.size(); ++i){
        if (connection == NULL){
            messageLock.unlock();
            return;
        }
        qint64 result = connection->sendTextMessage(savedMessages.at(i));
        if (result <= 0){
            cout << "RESULT WAS LESS THAN 0(2)" << endl;
            messageLock.unlock();
            return;
        }
    }
    savedMessages.resize(0);
    messageLock.unlock();
}
void User::triggerMessageSignal(QString s){
    cout << QThread::currentThreadId() << " TRIGGER" << endl;
    emit sendNewMessage(s);
}


long long hashTime(string s){ //hashes the userName
  long long h = 7;
  for (unsigned i = 0; i < s.size(); ++i){
    h = h * 37 + s.at(i);
  }
  if (h < 0){
      h = abs(h);
  }
  return h;
}

Hash::Hash(MainWindow *w)
{
    userTable.assign(100, NULL);
    numOfUsers = 0;
    window = w;
}

Hash::~Hash(){
    for (unsigned i = 0; i < userTable.size(); ++i){
        User *current = userTable.at(i);
        if (current == NULL){
            continue;
        }
        User *ahead = current->next;
        delete current;
        while (ahead != NULL){
            current = ahead;
            ahead = ahead->next;
            delete current;
        }
    }
    userTable.resize(0);
}

bool Hash::buildUserTable(){
    ifstream ifs;
    ofstream ofs;
    ifs.open("users.txt");

    if (!ifs.is_open()){
        window->updateConsole(QString("Users.txt not found"));
        ofs.open("users.txt");
        int index = (hashTime("admin") % userTable.size());

        if (index >= userTable.size()){
            window->updateConsole(QString("Hash key is too big"));
            return false;
        }
        string admin = "==\nadmin\npass\n" + to_string(index) + "\n";
        ofs << admin;
        ofs.close();
    }

    int count = 0;
    long long index;
    string input, user, pass;
    while (getline(ifs, input)){
        if (input == "=="){
            count = 1;
        }
        else if (count == 1){
            user = input;
            ++count;
        }
        else if (count == 2){
            pass = input;
            ++count;
        }
        else if (count == 3){
            index = stoi(input);
            User *u = new User();
            u->setParent(0);
            u->userName = QString::fromStdString(user);
            u->pass = QString::fromStdString(pass);
            u->index = index;
            qDebug() << u->userName << endl;
            QObject *p = u->parent();
            if (p != NULL){
                cout << "HOOPLA, PARENT HOOPLA\n";
            }
            addUser(u, false);
        }
    }

    ifs.close();
    ifs.open("friends.txt");

    if (!ifs.is_open()){
        ifs.close();
        window->updateConsole(QString("friends.txt not found. Creating it now."));
        ofs.open("friends.txt");
        ofs.close();
        return true;
    }
    else{
        while (getline(ifs, input)){
            if (input == ""){
                continue;
            }
            QString qs = QString::fromStdString(input);
            int indexOf = qs.indexOf(":");
            QString firstHalf = qs.mid(0, indexOf);
            QString secondHalf = qs.mid(indexOf + 1, qs.size() - 1);
            User *f1 = getUser(firstHalf);
            User *f2 = getUser(secondHalf);
            f1->addFriend(secondHalf);
            f2->addFriend(firstHalf);
        }
    }
    //window->updateLCD(currentUsers);
    return true;
}

void Hash::addUser(QString userName, QString pass, QWebSocket *socket, bool flag){
    User *u = new User(userName, pass, socket);
    this->addUser(u, flag);
}

void Hash::addUser(User *u, bool flag){
    if (u->index == -1){
        u->index = (hashTime(u->userName.toStdString()) % userTable.size());
    }
    User *temp = userTable.at(u->index);
    User *temp2 = temp;
    while (temp != NULL){
        temp2 = temp;
        temp = temp->next;
    }

    if (temp2 != NULL){
        temp2->next = u;
    }
    else{
        userTable.at(u->index) = u;
    }
    if (flag){
        writeToFile(u);
    }
    ++numOfUsers;
    connect(u, SIGNAL(errorWithUser(QString)), window, SLOT(updateConsole(QString)));
}

User* Hash::getUser(QString userName){
    int key = (hashTime(userName.toStdString())) % userTable.size();
    User *temp = userTable.at(key);
    while (temp != NULL){
        if (temp->userName == userName){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

tuple<bool, bool, User*> Hash::checkUser(QString userName, QString pass){
    User *current= getUser(userName);
    //u = current;
    if (current == NULL){
        cout << "Oh snap!" << endl;
        return make_tuple(false, false, current);
    }
    if (current->pass != pass){
        cout << "Oh snap!2" << endl;
        return make_tuple(false, false, current);
    }
    if (current->connection != NULL){
        cout << "Oh snap3!" << endl;
        return make_tuple(true, false, current);
    }

    return make_tuple(true, true, current);
}

void Hash::writeToFile(User *u){
    ofstream out;

    out.open("users.txt", ios::app);

    out << "==\n";

    out << u->userName.toStdString() << "\n";

    out << u->pass.toStdString() << "\n";

    out << u->index << "\n";

    out.close();

}


