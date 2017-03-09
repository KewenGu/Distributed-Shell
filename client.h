

#define WELL_KNOWN_PORT_NUM 4074
#define BUFSIZE 1024
#define NAMESIZE 32
#define USERNAME "kgu"
#define PASSWORD "407485831"

int TalkTCP(char *servHostAddr, int servHostPort);
int ClientSend(void *sendBuf, int len, int sock);
int ClientRecv(void *recvBuf, int len, int sock);
void DieWithError(char *errorMessage);
