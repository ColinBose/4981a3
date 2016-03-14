#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <vector>
#include "main.h"
#include <QTime>
#include <string>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Q_SLOT void setEditText(QString str){
        ui->userList->clear();
    }
    Q_SLOT void addUserList(QString str){
        ui->userList->append(str);
    }
    Q_SLOT void addMainOutput(QString str){
         char outputString[BUFFSIZE + MAXNAME + 12];
         QTime clock = clock.currentTime();
         QString time = clock.toString(Qt::TextDate);
         QByteArray ba = time.toLatin1();
         char isitc[600] = { 0 };
         char * header = ba.data();
         ba = str.toLatin1();
         char * body = ba.data();
         body++;
         sprintf(isitc, "[%s] %s",header, body );

         ui->outputText->append(isitc);
    }


   // void setText(QString hateQt);

private slots:
    void on_inputButton_clicked();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
