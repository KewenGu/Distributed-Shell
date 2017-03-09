/*
* Kewen Gu
*
* listen.tcp.c - Illustrate simple TCP connection
*/

#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server.h"

int main(int argc, char *argv[]) {

  if (argc != 1) {
    fprintf(stderr, "Usage: %s - starting a server on port 4074\n", argv[0]);
    exit(1);
  }

  int servSock, clntSock, servSockservHostPort;
	unsigned int clntLen;
	struct sockaddr_in servAddr, clntAddr;
  char username[NAMESIZE], password[NAMESIZE];
  int childpid, bytes, randInt;
  char salt[3];
  char encrypt[BUFSIZE], readBuf[BUFSIZE], command[BUFSIZE];
  char *args[MAX_ARGS];
  FILE *file;
  Client clients[MAX_NUM_CLIENTS];
  Client currentClient;
  int i = 0, j = 0, k;

  if ((file = fopen("clients.txt", "r")) == NULL)
    DieWithError("open() failed");

  while (fgets(readBuf, NAMESIZE + 1, file) != NULL) {
    if (strlen(readBuf) > 1) {
      readBuf[strlen(readBuf) - 1] = '\0';
      if (i % 2 == 0) {
        strcpy(clients[j].username, readBuf);
      }
      else if (i % 2 == 1) {
        strcpy(clients[j].password, readBuf);
        j++;
      }
      i++;
    }
  }

  servSockservHostPort = WELL_KNOWN_PORT_NUM;

	/* Create the socket */
	if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed");
	/* Configure server address */
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family      = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port        = htons(servSockservHostPort);
	/* Bind to the address */
	if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)))
		DieWithError("bind() failed");
	/* Listen to the socket */
	if (listen(servSock, MAX_NUM_CLIENTS) < 0)
		DieWithError("listen() failed");

  printf("Socket ready to go! Accepting connections....\n\n");

  while(1) {
    /* wait here (block) for connection */
    clntLen = sizeof(clntAddr);
    clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntLen);
    if (clntSock < 0)
      DieWithError("accept() failed");

    if ((childpid = fork()) == 0) {
      /* close the original socket */
      close(servSock);
      /* read data until no more */
      ServerRecv(username, NAMESIZE, clntSock);

      for (k = 0; k < MAX_NUM_CLIENTS; k++) {
        if (!strcmp(clients[k].username, username)) {
          currentClient = clients[k];
          ServerSend("USERNAME FOUND", NAMESIZE, clntSock);
          break;
        }
        else
          ServerSend("USERNAME NOT FOUND", NAMESIZE, clntSock);
      }

      srand(time(NULL));
      randInt = rand() % 90 + 10;
      sprintf(salt, "%d", randInt);
      salt[2] = '\0';

      ServerSend(salt, 3, clntSock);

      ServerRecv(encrypt, NAMESIZE, clntSock);

      if (!strcmp(encrypt, crypt(currentClient.password, salt)))
        ServerSend("PASSWORD CORRECT", NAMESIZE, clntSock);
      else
        ServerSend("PASSWORD INCORRECT", NAMESIZE, clntSock);

      ServerRecv(command, BUFSIZE, clntSock);
      printf("Received command from client %s: %s\n", currentClient.username, command);

      StrSlicing(args, command, " ");

      dup2(clntSock, 1);
      dup2(clntSock, 2);

			if(execvp(args[0], args) < 0)
				DieWithError("execvp() failed");

      /* close the client socket */
      close(clntSock);
      exit(0);
    }
    else {
      /* close the client socket */
      close(clntSock);
    }
  }
  /* close the original socket */
  close(servSock);

  return 0;
}


/* Send operation for both client and server */
int ServerSend(void *sendBuf, int len, int servSock)
{
  int bytesSend;
	if ((bytesSend = send(servSock, sendBuf, len, 0)) != len)
		DieWithError("send() failed");

  return bytesSend;
}


/* Receive operation for both client and server */
int ServerRecv(void *recvBuf, int len, int servSock)
{
	int n, bytesRcvd = 0;

	while (bytesRcvd < len)
	{
		if ((n = recv(servSock, recvBuf + bytesRcvd, len - bytesRcvd, 0)) < 0)
			DieWithError("recv() failed");

		bytesRcvd += n;
	}

	return bytesRcvd;
}


/* Slice the long string into tokens */
int StrSlicing(char *args[], char *buf, char *s)
{
	int i = 0;
	/* get the first token */
	char *token = strtok(buf, s);
	args[i] = token;

	/* walk through other tokens */
	while(token != NULL)
	{
	    token = strtok(NULL, s);
	    args[++i] = token;
	}
	return i;
}


/* If error generated, print error message and die */
void DieWithError(char *errorMessage)
{
	perror(errorMessage);
	exit(1);
}
