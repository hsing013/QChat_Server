#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    cout << "Window deconstructor called." << endl;
    delete ui;
}

void MainWindow::updateConsole(QString qs){
    if (qs.at(qs.size() - 1) != '\n'){
        qs += '\n';
    }
    ui->console->insertPlainText(qs);
}

void MainWindow::updateLCD(int usersOnline){
    cout << QThread::currentThreadId() << "LCD"<<  endl;
    ui->activeUsers->display(usersOnline);
}
