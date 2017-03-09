

#define WELL_KNOWN_PORT_NUM 4074
#define BUFSIZE 1024
#define NAMESIZE 32
#define MAX_NUM_CLIENTS 10
#define MAX_ARGS 10

typedef struct client {
  char username[NAMESIZE];
  char password[NAMESIZE];
} Client;

int ListenTCP(int servHostPort);
int ServerSend(void *sendBuf, int len, int sock);
int ServerRecv(void *recvBuf, int len, int sock);
int StrSlicing(char *args[], char *buf, char *s);
void DieWithError(char *errorMessage);
