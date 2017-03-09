/*
* Kewen Gu
*
* talk.tcp.c - Illustrate simple TCP connection
*
* Calls a server named as the first argument
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
#include <sys/time.h>
#include <fcntl.h>

#include "client.h"

int main(int argc, char *argv[]) {

  if (argc != 5) {
    fprintf(stderr, "usage: %s -c <command> -s <server address>\n", argv[0]);
    fprintf(stderr, "\t<command>\t- command you want the server to execute\n");
    fprintf(stderr, "\t<server address>\t- the IP address of the server\n");
    exit(1);
  }

  int sock;
  char servHostPort[NAMESIZE], *servHostAddr;
  struct addrinfo hints, *servinfo, *res;
  int opt, file, bytes;
  char *command;
  char response[BUFSIZE];
  char salt[3];
  char *encrypt;
  int fd1, fd2, fd3;
  char time[NAMESIZE];
  double start, end;
  struct timeval tv;

  while ((opt = getopt(argc, argv, "c:s:h")) != -1) {
    switch (opt) {
      case 'c':
        command = argv[optind-1];
        break;
      case 's':
        servHostAddr = argv[optind-1];
        break;
      case 'h':

        break;
    }
  }

  gettimeofday(&tv, NULL);
  start = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;

  sprintf(servHostPort, "%d", WELL_KNOWN_PORT_NUM);

  /* Configure the server address */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/* Obtain the server address info */
	if(getaddrinfo(servHostAddr, servHostPort, &hints, &servinfo) != 0)
	{
		fprintf(stderr, "Getting IP address failed\n");
		exit(1);
	}

	/* Try the obtained address one by one until connected */
	for (res = servinfo; res; res = res->ai_next)
	{
		/* Create a relaible, stream socket using TCP */
		if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
			DieWithError("socket() failed");

		/* Establish the connection to the echo server */
		if (connect(sock, res->ai_addr, res->ai_addrlen) < 0)
			DieWithError("connect() failed");
	}

  gettimeofday(&tv, NULL);
  end = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;

  if ((fd1 = open("setup.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
    DieWithError("open() failed");

  sprintf(time, "%.3f\n", end - start);
  if (write(fd1, time, strlen(time)) < 0)
    DieWithError("write() failed");

  close(fd1);

  gettimeofday(&tv, NULL);
  start = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;

  /* send username to the server */
  ClientSend(USERNAME, NAMESIZE, sock);

  /* read message from server */
  ClientRecv(response, NAMESIZE, sock);

  if (!strcmp(response, "USERNAME FOUND")) {
    /* receive salt from  */
    ClientRecv(salt, 3, sock);
  }
  else if (!strcmp(response, "USERNAME NOT FOUND")) {
    fprintf(stderr, "Username or password is not correct\n");
    exit(1);
  }

  encrypt = crypt(PASSWORD, salt);

  /* send password to the server */
  ClientSend(encrypt, NAMESIZE, sock);

  /* read message from server */
  ClientRecv(response, NAMESIZE, sock);

  if (!strcmp(response, "PASSWORD CORRECT")) {
    gettimeofday(&tv, NULL);
    end = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;

    if ((fd2 = open("authentication.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
      DieWithError("open() failed");

    sprintf(time, "%.3f\n", end - start);
    if (write(fd2, time, strlen(time)) < 0)
      DieWithError("write() failed");

    close(fd2);

    gettimeofday(&tv, NULL);
    start = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;

    ClientSend(command, BUFSIZE, sock);
  }
  else if (!strcmp(response, "PASSWORD INCORRECT")) {
    fprintf(stderr, "Username or password is not correct\n");
    exit(1);
  }

  /* receivce result from server */
	while((bytes = recv(sock, response, BUFSIZE, 0)) > 0) {
		response[bytes] = '\0';
		printf("%s", response);
	}
	if (bytes == -1)
		DieWithError("recv() failed");

  /* close socket */
  close(sock);

  gettimeofday(&tv, NULL);
  end = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;

  if ((fd3 = open("teardown.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
    DieWithError("open() failed");

  sprintf(time, "%.3f\n", end - start);
  if (write(fd3, time, strlen(time)) < 0)
    DieWithError("write() failed");

  close(fd3);

  return 0;
}


/* Send operation for both client and server */
int ClientSend(void *sendBuf, int len, int sock)
{
  int bytesSend;
	if ((bytesSend = send(sock, sendBuf, len, 0)) != len)
		DieWithError("send() failed");

  return bytesSend;
}

/* Receive operation for both client and server */
int ClientRecv(void *recvBuf, int len, int sock)
{
	int n, bytesRcvd = 0;

	while (bytesRcvd < len)
	{
		if ((n = recv(sock, recvBuf + bytesRcvd, len - bytesRcvd, 0)) < 0)
			DieWithError("recv() failed");
		bytesRcvd += n;
	}

	return bytesRcvd;
}

/* If error generated, print error message and die */
void DieWithError(char *errorMessage)
{
	perror(errorMessage);
	exit(1);
}
