#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "main.h"
#include <QMutex>
#include <vector>
#include <string>
extern QMutex printAccess;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //qRegisterMetaType("QTextBlock");
    ui->setupUi(this);

}
QString getText(){
    //return ui->inputText->toPlainText();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_inputButton_clicked()
{
    QString hateQt = ui->inputText->toPlainText();
    if(hateQt.length() == 0){
        return;
    }
    showText(hateQt);
    ui->inputText->clear();
    printAccess.lock();
    ui->outputText->append(hateQt);
    printAccess.unlock();
}
void MainWindow::setText(char * hateQt){
     ui->outputText->append(hateQt);
}
void MainWindow::addUser(std::vector<std::string> hateQt){
   // ui->userList->clear();
    printf("In main window");
    fflush(stdout);
    for (std::vector<std::string>::const_iterator name = hateQt.begin(); name != hateQt.end(); ++name)
       {
        printf("Adding user: %s\n", name);
        std::string f;
        f = *name;
        ui->userList->append(f.c_str());


       }
   // for (std::string name : hateQt){

    //}


}
