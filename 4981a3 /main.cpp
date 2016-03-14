

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
#include "mainwindow.h"
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
#define SERVER_TCP_PORT   7000  // Default port
#define BUFFSIZE      512    // Buffer length
#define MAXNAME 30
#define MAXCLIENTS 1000
#define MAXLISTSIZE MAXCLIENTS * MAXNAME
using namespace std;
char * readSock(int sd, int bSize);
void updateList();
MainWindow * mw;
std::vector<std::string> nameList;
QMutex printAccess;
int sd;
void sendNamesToList(char names[]);
void removeFromNameList(std::string remove);
void * readThread(void * arg){

    char pass[BUFFSIZE] = { 0 };
    char * bp;
    while(1){
        bp = readSock(sd, BUFFSIZE);
        strcpy(pass,bp);
        printf("PREEDIT %s:", bp);
        if(pass[0] == ':'){
            printf("Removing user");
            fflush(stdout);
            string p(pass);
            removeFromNameList(p);
        }
        else if(pass[0] == '*'){
            string p(pass);
            char * temp;
            char * hold;
            p.erase(0,1);
            hold = (char *)p.c_str();
            temp = strtok(hold, " ");
            string q(temp);
            nameList.push_back(q);
            emit mw->addUser(nameList);
        }
        printf ("First: %s %s", bp, pass);
        printAccess.lock();
        emit mw->setText(pass);
        printf (" Second: %s %s", bp, pass);
        printAccess.unlock();
        fflush(stdout);
  }
}

void removeFromNameList(std::string remove){
    char * temp;
    char * hold;
    remove.erase(0,1);
    hold = (char *)remove.c_str();
    temp = strtok(hold, " ");
    nameList.erase(std::remove(nameList.begin(), nameList.end(), temp), nameList.end());
    updateList();
}

void showText(QString text){
    QByteArray ba = text.toLatin1();
    const char * out = ba.data();
    qDebug(out);
    send (sd, out, BUFFSIZE, 0);
}

void * writeThread(void * arg){
    char sbuf[BUFFSIZE];
     QString text;
    int len;
      while(1){
    // fgets (sbuf, BUFFSIZE, stdin);
      //text = w.getText;
          if((len = strlen(sbuf)) == 0){
       continue;
     }
     sbuf[len-1] = '\0';
     send (sd, sbuf, BUFFSIZE, 0);

   }
}
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
        //tempName = strtok(NULL, "-");
        f = tempName;
        nameList.push_back(f);

    }
}
void updateList(){
    emit mw->addUser(nameList);
}

char * readSock(int sd, int bSize){
  int n = 0;
  char readBuff[bSize] = {0 };
  char * buf = readBuff;
  buf = readBuff;
  int bytesLeft = bSize;
    printf("Starting read %d: ", bSize);
    fflush(stdout);
  while((n = recv(sd, buf, bytesLeft, 0)) < bSize){
    buf += n;
    bytesLeft -= n;
  }
    printf("done read");
    fflush(stdout);
  return buf;
}
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
qDebug("Hello");
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
  strcpy(names, readSock(sd, MAXLISTSIZE));
  printf("Got Name List %s", names);
  //printf("Recieved: %s \n", readSock(sd, MAXLISTSIZE));
  sendNamesToList(names);
  updateList();
  write(sd, clientName, MAXNAME);
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
