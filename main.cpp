#include "mainwindow.h"
#include <QApplication>
#include "include.h"
#include <iostream>
#include "server.h"
using namespace std;

QMutex myMutex;
QMutex fileMutex;



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QString s = "Look,hoopla,";
//    if (s.at(s.size()-1) == QChar(',')){
//        s.remove(s.size()-1, 1);
//        cout << s.toStdString() << endl;
//    }
    Server *s = new Server();
    s->listen();
//    MainWindow w;
//    w.show();


//    //<$Goldenfiber:Lawrence$><%Hello world%> // The message structure.

//    QString qs = "<$Goldenfiber:Lawrence$>Hello world";

//    int index = qs.indexOf("$>");
//    QString fromTo = qs.mid(2, index - 2);
//    QString message = qs.mid(index + 2, qs.size() - 1);
//    cout << message.toStdString() << endl;
//    cout << qs.mid(2, index - 2).toStdString() << endl;

//    index = fromTo.indexOf(":");

//    cout << fromTo.mid(index + 1, fromTo.size() - 1).toStdString() << endl;

//    cout << fromTo.mid(0, index).toStdString() << endl;

    //QString myGod = "<$ADD$>Lawrence";

    //QString split = myGod.mid(0, 7);
    //QString split2 = myGod.mid(7);
    //cout << split.toStdString() << endl;
    //cout << split2.toStdString() << endl;


    return a.exec();
}
