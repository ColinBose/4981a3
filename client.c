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
#include <stdio.h>
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

#define SERVER_TCP_PORT   7000  // Default port
#define BUFFSIZE      512    // Buffer length
#define MAXNAME 30
#define MAXCLIENTS 1000
#define MAXLISTSIZE  MAXCLIENTS * MAXNAME
char * readSock(int sd, int bSize);
int sd;
void * readThread(void * arg){

  char * bp;
  while(1){
    bp = readSock(sd, BUFFSIZE);
    printf ("%s\n", bp);
    fflush(stdout);
  }
}
char * readSock(int sd, int bSize){
  int n = 0;
  char readBuff[bSize];
  char * buf = malloc(sizeof(char) * bSize);
  buf = readBuff;
  int bytesLeft = bSize;
  printf("Starting read %d", bSize);
  fflush(stdout);
  while((n = recv(sd, buf, bytesLeft, 0)) < bSize){
    buf += n;
    bytesLeft -= n;
  }
  printf("Done reading");
  fflush(stdout);

  return buf;
}
int main (int argc, char **argv)
{
  
  int port;
  struct hostent  *hp;
  struct sockaddr_in server;
  char  *host, sbuf[BUFFSIZE], **pptr;
  char str[16];
  pthread_t athread;
  char clientName[BUFFSIZE];
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
  printf("Recieved USER LIST: %s \n", readSock(sd, MAXLISTSIZE));
  write(sd, clientName, MAXNAME);
  printf("Connected:    Server Name: %s\n", hp->h_name);
  pptr = hp->h_addr_list;
  printf("\t\tIP Address: %s\n", inet_ntop(hp->h_addrtype, *pptr, str, sizeof(str)));
  printf("Transmit:\n");
  //gets(sbuf); // get user's text
  struct thread_info;
  pthread_create(&athread, NULL,readThread, (void *)0);
   int len;
     while(1){
    fgets (sbuf, BUFFSIZE, stdin);
    if((len = strlen(sbuf)) == 0){
      continue;
    }
    sbuf[len-1] = '\0';
    if(sbuf[0] == '/' && sbuf[1] == 'p' && sbuf[2] == 'm'){
        
    }
    send (sd, sbuf, BUFFSIZE, 0);

  }
  printf("closing socket");
  fflush(stdout);
  close (sd);
  return (0);
}