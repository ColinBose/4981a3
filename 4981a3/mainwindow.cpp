#include "mainwindow.h"

#include "main.h"
#include <vector>
#include <string>
#include <QTime>
#include <QDateTime>
extern char userName[MAXNAME];
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
    QString sendString = ui->inputText->toPlainText();
    if(sendString.length() == 0){
        printf("Pm doesnt work");
                fflush(stdout);
        return;
    }
    QTime clock = clock.currentTime();
    QString time = clock.toString(Qt::TextDate);
    QByteArray ba = time.toLatin1();
    char isitc[600] = { 0 };
    char * header = ba.data();
    ba = sendString.toLatin1();
    char * body = ba.data();
    sprintf(isitc, "[%s] %s: %s",header, userName, body );
    showText(sendString);
    ui->inputText->clear();
//    printAccess.lock();
    printf("String to append2: %s\n", isitc);
    ui->outputText->append(isitc);
  //  printAccess.unlock();
    fflush(stdout);
}
/*void MainWindow::setText(char * hateQt){
     ui->outputText->append(hateQt);
}
*/
/*void MainWindow::addUser(std::vector<std::string> hateQt){
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
*/
