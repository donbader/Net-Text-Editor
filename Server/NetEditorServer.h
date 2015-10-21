#ifndef NESERVER
#define NESERVER
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


//server command
#define _SHUTDOWN -2
#define _QUIT -1
#define _COMMAND 0
#define _PRINTF 1
#define _SCANF 2
#define _SYSTEM 3
#define _HELP 4


//client command
#define __CREATE_FILE 10
#define __EDIT_FILE 11
#define __REMOVE_FILE 12
#define __LIST_FILE 13
#define __SEND_FILE 14
#define __DOWNLOAD_FILE 15
#define __UPLOAD_FILE 16


/*COMMUNICATION*/

void deal_with(int sockfd, int command);
int init_socket(unsigned int port);
void ask(int sockfd, int command);
void sendMsg(int sockfd, char *msg);
int s_recvData(int sockfd, char *buffer);
//

/*TOOLS*/

void clientPrintf(int sockfd, char *string);
void clientScanf(int sockfd, char *string);
void clientSystem(int sockfd, char *string);
void clientPrologue(int sockfd);
void clientHelp(int sockfd);
char *clientGetFilename(int sockfd);

char file_exist(char *filename);
unsigned long data_volume(char *data);
char *va_strcat(char *first, ...);
//

/*FILE OPERATION*/

char* list_file(char *path);
void create_file(int sockfd);
void remove_file(int sockfd, char *filename);
void edit_file(int sockfd, char *filename);
char send_file(int sockfd, char *filename);
void download_file(int sockfd, char *filename);
//

#endif