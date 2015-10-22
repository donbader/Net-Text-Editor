#ifndef NTECLIENT
#define NTECLIENT
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>

//directory
#include <dirent.h>

//socket
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/*CONSTANTS*/
#define KB 1024
#define BUFFER_SIZE 10*KB
#define MAX_FILE_NAME 256
#define MAX_CONNECTION 20
#define MAX_BATCH_FILE 30



#define _SHUTDOWN -1
#define _QUIT 0
#define _COMMAND 1
#define _DEALING 2
#define _ASK 3
#define _PRINTF 4
#define _SCANF 5
#define _SYSTEM 6


#define __HELP 10
#define __CREATE_FILE 11
#define __AUTO_CREATE_FILE 12
#define __EDIT_FILE 13
#define __REMOVE_FILE 14
#define __AUTO_REMOVE_FILE 15
#define __LIST_FILE 16

#define __SEND_FILE 17
#define __AUTO_SEND_FILE 18
#define __DOWNLOAD_FILE 19
#define __AUTO_DOWNLOAD_FILE 20

#define __BATCH 21


#define _NO_COMMAND 90


#define ask(FD, CMD) sendInt(FD, CMD);

/*MAIN DEALING*/
void StoI(int sockfd,char *string);
void deal_with(int sockfd, int command);

/*COMMUNICATION*/
int connect_to(char *ip, unsigned int port);
void sendInt(int sockfd, int num);
int recvInt(int sockfd);
void sendLLONG(int sockfd, long long num);
long long recvLLONG(int sockfd);
void sendString(int sockfd, char* data);
char *recvString(int sockfd);
char sendFILE(int sockfd, char *fileName);
char recvFILE(int sockfd, char* fileName);
//

/*TOOLS*/
long long file_size(char* fileName);
char *va_strcat(char *first, ...);
void printDisappearRate(int rate);
void split(char **arr, char *str, const char *del);


#endif