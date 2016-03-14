

/*---------------------------------------------------------------------------------------
--  SOURCE FILE:    tcp_clnt.c - A simple TCP client program.
--
--  PROGRAM:    tclnt.exe
--
--  FUNCTIONS:    Berkeley Socket API
--
--  DATE:     February 2, 2008
--
--  REVISIONS:    (Date and Description)
--        January 2005
--        Modified the read loop to use fgets.
--        While loop is based on the buffer length
--
--
--  DESIGNERS:    Aman Abdulla
--
--  PROGRAMMERS:    Aman Abdulla
--
--  NOTES:
--  The program will establish a TCP connection to a user specifed server.
-- The server can be specified using a fully qualified domain name or and
--  IP address. After the connection has been established the user will be
-- prompted for date. The date string is then sent to the server and the
-- response (echo) back from the server is displayed.
---------------------------------------------------------------------------------------*/

#include <QApplication>
#include <stdio.h>
#include "signalobject.h"
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <string>
#include <QString>
#include "main.h"
#include <QMutex>
#include <vector>
#include "mainwindow.h"

using namespace std;
char userName[MAXNAME] = { 0 };
char * readSock(int sd, int bSize);
void updateList();
MainWindow * mw;
std::vector<std::string> nameList;
int sd;
void sendNamesToList(char names[]);
void removeFromNameList(std::string remove);

//The read thread, called after initial list setup
//Reads data and prints to main window
void * readThread(void * arg){

    char pass[BUFFSIZE] = { 0 };
    char * bp;

    while(1){
        printf("Entering read");
        fflush(stdout);
        bp = readSock(sd, BUFFSIZE);
        printf("BG in read: %s Pass before copy: %s\n", bp, pass);
        strcpy(pass,bp);
       // printf("Pass after copy: %s\n", pass );

        //Checks user disconnect
        if(pass[0] == ':'){
            printf("Removing user");
            fflush(stdout);
            string p(pass);
            removeFromNameList(p);
        }

        //Check user connect
        else if(pass[0] == '*'){
            char * temp;
            char tempArray[MAXNAME + 15];
            temp = tempArray;
            strcpy(temp, pass);
            temp++;
            temp = strtok(temp, " ");
            nameList.push_back(temp);
            printf("BP: %s pass: %s\n", temp, pass);
            QMetaObject::invokeMethod(mw, "addUserList", Q_ARG(QString,temp));

        }
        //Print it to window
        printf("pass before print: %s BP before print: %s\n", pass, bp);
        //sprintf(outputString, "" )
        QMetaObject::invokeMethod(mw, "addMainOutput", Q_ARG(QString,pass));
  }
}
//Clears user list then repopulates
void popUserList(){
    QMetaObject::invokeMethod(mw, "setEditText", Q_ARG(QString, "Hi"));
    for (std::vector<std::string>::const_iterator name = nameList.begin(); name != nameList.end(); ++name)
       {
        QString qstr = QString::fromStdString(*name);
        QMetaObject::invokeMethod(mw, "addUserList", Q_ARG(QString,qstr));
       }
}
//Removes name from vector
void removeFromNameList(std::string remove){
    char * temp;
    char * hold;
    remove.erase(0,1);
    hold = (char *)remove.c_str();
    temp = strtok(hold, " ");
    nameList.erase(std::remove(nameList.begin(), nameList.end(), temp), nameList.end());
    popUserList();
}
//Write to socket
void showText(QString text){
    QByteArray ba = text.toLatin1();
    const char * out = ba.data();
    send (sd, out, BUFFSIZE, 0);
}
/*
void * writeThread(void * arg){
    char sbuf[BUFFSIZE];
     QString text;
    int len;
      while(1){
          if((len = strlen(sbuf)) == 0){
       continue;
     }
     sbuf[len-1] = '\0';
     send (sd, sbuf, BUFFSIZE, 0);

   }
}
*/

//Splits name list on initial startup and populate vector
void sendNamesToList(char names[]){
    char * tempName;
    std::string f;
    if(strlen(names) == 0){
        return;
    }
    tempName = strtok(names, "-");
    f = tempName;
    nameList.push_back(f);
    while((tempName = strtok(NULL, "-")) != NULL){
        f = tempName;
        nameList.push_back(f);

    }
}
//Called in read loop/startup to read bSize bytes from socket
char * readSock(int sd, int bSize){
  int n = 0;
  char readBuff[bSize] = {0 };
  char * buf = readBuff;
  buf = readBuff;
  int bytesLeft = bSize;
  while((n = recv(sd, buf, bytesLeft, 0)) < bSize){
    buf += n;
    bytesLeft -= n;
  }
  return buf;
}
//Main, sets up socket, does user list stuff, sends name to server then starts read thread.
int main (int argc, char **argv)
{

  int port;
  struct hostent  *hp;
  struct sockaddr_in server;
  char  *host,  **pptr;
  char str[16];
  pthread_t athread, wThread;
  char clientName[BUFFSIZE];
  QApplication a(argc, argv);
  MainWindow w;
  mw = &w;

  w.show();

  switch(argc)
  {
    case 2:
      host =  argv[1];  // Host name
      port =  SERVER_TCP_PORT;
    break;
    case 3:
      host =  argv[1];
      port =  atoi(argv[2]);  // User specified port
    break;
    case 4:
      host =  argv[1];
      port =  atoi(argv[2]);  // User specified port
      strcpy(clientName, argv[3]);

      break;
    default:
      fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
      exit(1);
  }
  // Create the socket
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Cannot create socket");
    exit(1);
  }
  bzero((char *)&server, sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  if ((hp = gethostbyname(host)) == NULL)
  {
    fprintf(stderr, "Unknown server address\n");
    exit(1);
  }
  bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);

  // Connecting to the server
  if (connect (sd, (struct sockaddr *)&server, sizeof(server)) == -1)
  {
    fprintf(stderr, "Can't connect to server\n");
    perror("connect");
    exit(1);
  }
  char names[MAXLISTSIZE];
  printf("before readsock");
  fflush(stdout);
  strcpy(names, readSock(sd, MAXLISTSIZE));
  printf("Got Name List %s", names);
  //printf("Recieved: %s \n", readSock(sd, MAXLISTSIZE));
  printf("before sendnames");
  fflush(stdout);
  sendNamesToList(names);
  printf("before popuser");
  fflush(stdout);
  popUserList();
  printf("before write");
  fflush(stdout);
  write(sd, clientName, MAXNAME);
  printf("Using name %s: ", clientName);
  printf("after write");
  fflush(stdout);
  strcpy(userName, clientName);
  fflush(stdout);
  printf("Connected:    Server Name: %s\n", hp->h_name);
  pptr = hp->h_addr_list;
  printf("\t\tIP Address: %s\n", inet_ntop(hp->h_addrtype, *pptr, str, sizeof(str)));
  printf("Transmit:\n");
  //gets(sbuf); // get user's text
  struct thread_info;
  pthread_create(&athread, NULL,readThread, (void *)0);
  //pthread_create(&wThread, NULL, writeThread, (void *)0);

  //printf("closing socket");
  //fflush(stdout);
  //close (sd);

  return a.exec();
}
