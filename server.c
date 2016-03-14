#include "shared.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 7000  
#define BUFFSIZE 512   
#define MAXCLIENTS 1000
#define MAXLISTEN 1000
#define MAXLINE 512
#define MAXNAME 30
#define MAXLISTSIZE MAXCLIENTS * MAXNAME
static void SystemFatal(const char* );
void doPmRequest(char * ipaddresses[], char * names[], int sockfd, char * name, char * message);
void sendUserList(char * names[], int sd, int maxi);
void sendSock(int client[], int maxi,  char * readBuff , int i);


char * readSock(int sd, int buffSize){
    int n;
    char readBuff[buffSize];
    char * buf = malloc(sizeof(char) * buffSize);
    buf = readBuff;
    int bytesLeft = buffSize;
    fflush(stdout);
    while((n = recv(sd, buf, bytesLeft, 0)) < buffSize){
        if(n == 0){
        printf("Client Disconecting");
        fflush(stdout);
        return NULL;
    }
    buf += n;
    bytesLeft -= n;
  }
  fflush(stdout);
  return buf;
}

void doPmRequest(char * ipaddresses[], char * names[], int sockfd, char * name, char * message){
  char header[BUFFSIZE + MAXNAME + 10];
  int x = 0;
    for(int i = 0; i < MAXCLIENTS; i++){
      
       if(strcmp(name, names[i]) == 0){
            sprintf(header, "&PM From %s: ", names[sockfd]);
            strcat(header, message);
            x = 1;
            char * out = header;
            write(i, out, BUFFSIZE);
       }
       else{
        
       }

    }
    if(!x){
      strcpy(header, "&User not found");
      write(sockfd, header, BUFFSIZE);
    }
}
void sendUserList(char * names[], int sd, int maxi){
      char temp[MAXLISTSIZE] = "";
      for(int q = 4; q < maxi + 4; q++){

            if(strcmp(names[q], "")){
                strcat(temp, names[q]);
                strcat(temp, "-");
              
                fflush(stdout);
        }
      }
      int len;
      if((len = strlen(temp)) > 0){
            temp[len] = '\0';

      }
      write(sd, temp, MAXLISTSIZE);

  
printf("%s\n", temp);
}
void sendSock(int client[], int maxi, char * readBuff , int i){
  for(int g = 0; g <= maxi; g++)
       {
          
          if(g == i){
            continue;
          }
          if(client[g] == -1){
              continue;
          }
         write(client[g], readBuff, BUFFSIZE); 
        
        }
}
int main (int argc, char **argv)
{
  int i, maxi, nready,  arg;
  int listen_sd, new_sd, sockfd, maxfd, client[MAXCLIENTS];
  unsigned int client_len;
  struct sockaddr_in server, client_addr;
  char *bp;
  fd_set rset, allset;
  char * names[MAXCLIENTS], * ipaddresses[MAXCLIENTS];
  for(int z = 0; z < MAXCLIENTS; z++){
     names[z] = malloc(sizeof(char) * MAXNAME);
     ipaddresses[z] = malloc(sizeof(char) * MAXNAME);
  }
  
 
  if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    SystemFatal("Cannot Create Socket!");
  
  arg = 1;
  if (setsockopt (listen_sd, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1)
                SystemFatal("setsockopt");

  bzero((char *)&server, sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = htonl(INADDR_ANY); 

  if (bind(listen_sd, (struct sockaddr *)&server, sizeof(server)) == -1)
    SystemFatal("bind error");
  
  listen(listen_sd, MAXLISTEN);

  maxfd = listen_sd;  // initialize
  maxi  = -1;   // index into client[] array

  for (i = 0; i < MAXCLIENTS; i++)
    client[i] = -1;             // -1 indicates available entry
  FD_ZERO(&allset);
  FD_SET(listen_sd, &allset);
    

  while (1)
  {
    rset = allset;               // structure assignment
    nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
    
    if (FD_ISSET(listen_sd, &rset)) // new client connection
    {
      client_len = sizeof(client_addr);
      if ((new_sd = accept(listen_sd, (struct sockaddr *) &client_addr, &client_len)) == -1)
        SystemFatal("accept error");
      
        printf(" Remote Address:  %s\n", inet_ntoa(client_addr.sin_addr));

      for (i = 0; i < MAXCLIENTS; i++)
        if (client[i] < 0)
        {

          client[i] = new_sd; // save descriptor
          FD_SET(i, &rset);
          char name[BUFFSIZE];
          strcpy(name, "GETNAME");
          printf("NEW SD: %d\n", new_sd);
          fflush(stdout);
         // write(new_sd, name, BUFFSIZE);
          sendUserList(names, new_sd, new_sd);

          strcpy(names[new_sd], readSock(new_sd, MAXNAME));
          strcpy(ipaddresses[new_sd],inet_ntoa(client_addr.sin_addr));
          char connectMessage[50] = "*";
          strcat(connectMessage, names[new_sd]);
          strcat(connectMessage, " Connected");
          sendSock(client, maxi, connectMessage, i);
          fflush(stdout);
          break;
        }
      if (i == MAXCLIENTS)
      {
        printf ("Too many clients\n");
        exit(1);
      }
      
      FD_SET (new_sd, &allset);     // add new descriptor to set
      if (new_sd > maxfd)
        maxfd = new_sd; // for select

      if (i > maxi)
        maxi = i; // new max index in client[] array


      if (--nready <= 0)
        continue; // no more readable descriptors
  }

      for (i = 0; i <= maxi; i++) // check all clients for data
      {
      if ((sockfd = client[i]) < 0)
        continue;

      if (FD_ISSET(sockfd, &rset))
            {
           fflush(stdout);
           bp = readSock(sockfd, BUFFSIZE);
        if(bp == NULL){

          FD_CLR(sockfd, &allset);
          
          char disconectMessage[BUFFSIZE] = ":";
          strcat(disconectMessage, names[sockfd]);
          strcat(disconectMessage, " Disconnected");
          bp = disconectMessage;
          strcpy(names[client[i]], "");
          client[i] = -1;
          sendSock(client, maxi, bp, i);
          
        }
        else{
          if(bp[0] == '/' && bp[1] == 'p' && bp[2] == 'm'){
            printf("Doing a pm");
            char * requestName;
            char * message;
            char nameHold[MAXNAME];
            strcpy(nameHold, bp);
            requestName = strtok (nameHold," ,.-");
            requestName = strtok (NULL, " ,.-");
            message = strtok(NULL, "\0");
            printf("PM REQUEST FOR: %s ", requestName);
            doPmRequest(ipaddresses, names, sockfd, requestName, message);
            fflush(stdout);
            break;
          }
          char readBuff[BUFFSIZE + MAXNAME] = "-";
          strcat(readBuff, names[sockfd]);
          strcat(readBuff, ": ");
          char why[BUFFSIZE];
          strcpy(why, bp);
          strcat(readBuff, why);   
          fflush(stdout);
          printf("Sending addr: %s\n", ipaddresses[sockfd]);
          fflush(stdout);
          sendSock(client, maxi, readBuff, i);
        }
       

       
        
        if (--nready <= 0)
          break;        // no more readable descriptors
      }
    }
  }
  return(0);
}

// Prints the error stored in errno and aborts the program.
static void SystemFatal(const char* message)
{
    perror (message);
    exit (EXIT_FAILURE);
}
/*
int signalHandler(int signal){
  for(int z = 0; z < MAXCLIENTS; z++){
     free(names[z]);
     free(ipaddresses[z]);
  }
}
*/