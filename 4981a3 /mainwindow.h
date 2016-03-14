#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <string>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
   // void setText(QString hateQt);
    ~MainWindow();

private slots:
    void on_inputButton_clicked();
public slots:
    void addUser(std::vector<std::string> hateQt);
    void setText(char * hateQt);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
