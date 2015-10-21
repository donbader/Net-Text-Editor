#include "NTE_Client.h"

#ifndef DEBUG
#define DEBUG 0
#define DBG( ... ) \
		if (DEBUG) {  __VA_ARGS__; } 
#endif 

/*MAIN DEALING FUNCTION*/
/*MAIN DEALING FUNCTION*/
/*MAIN DEALING FUNCTION*/
/*MAIN DEALING FUNCTION*/
/*MAIN DEALING FUNCTION*/

void deal_with(int sockfd, int command){
	DBG(printf("server command: %d\n",command);)
	char input[MAX_FILE_NAME]="";
	char *msg = NULL;
	switch(command){
		case _SHUTDOWN :
		case _QUIT :return;
		case _COMMAND :
			printf("--------------------------------------------------------\n");
			printf("[CMD] ");fflush(stdout);
			scanf("%s",input);
			printf("--------------------------------------------------------\n");
			ask(sockfd, StoI(input));
			break;
		case _PRINTF :
			msg = recvString(sockfd);
			printf("%s",msg);
			fflush(stdout);
			break;
		case _SCANF :
			scanf("%s",input);
			sendString(sockfd, input);
			break;
		case _SYSTEM :
			msg = recvString(sockfd);
			system(msg);
			break;

		case __REMOVE_FILE:
			msg = recvString(sockfd);
			remove(msg);
			break;

		case __DOWNLOAD_FILE:
			msg = recvString(sockfd);
			recvFILE(sockfd, msg);
			break;
		case __SEND_FILE:
			msg = recvString(sockfd);
			sendFILE(sockfd, msg);
			break;

		case __UPLOAD_FILE:
			//give filename
			printf("filename: ");fflush(stdout);
			scanf("%s",input);
			if(file_size(input) == -1)
				printf("\"%s\"is not exist.\n",input);
			sendString(sockfd, input);
			sendFILE(sockfd, input);
			break;

		default:
			ask(sockfd, command);
	}

	if(msg)free(msg);
}



/*COMMUNICATION*/
/*COMMUNICATION*/
/*COMMUNICATION*/
/*COMMUNICATION*/
/*COMMUNICATION*/


int connect_to(char ip[],unsigned int port){
	
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);

	/*server address struct*/
	struct sockaddr_in dest;

	/* initialize value in dest */
	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	dest.sin_addr.s_addr = inet_addr(ip);
	/* Connecting to server */
	connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));

	return sockfd;
}

void sendInt(int sockfd, int num){
	DBG(printf("-----sendInt(): %d\n",num);)
	write( sockfd, &num, sizeof(int));
}
int recvInt(int sockfd){
	int num;
	read( sockfd, &num, sizeof(int));
	DBG(printf("-----recvInt(): %d\n",num);)
	return num; 
}
void sendLLONG(int sockfd, long long num){
	DBG(printf("-----sendLLONG(): %lld\n",num);)
	write( sockfd, &num, sizeof(long long));
}
long long recvLLONG(int sockfd){
	long long num;
	read( sockfd, &num, sizeof(long long));
	DBG(printf("-----recvLLONG(): %lld\n", num);)
	return num;
}

void sendString(int sockfd, char* string){
	//send string size
	int size = strlen(string);
	sendInt( sockfd, size);
	//send string
	write( sockfd, string, size);
	DBG(printf("-----sendString():%s\n", string);)
}
char *recvString(int sockfd){
	//recv data size
	int size = recvInt(sockfd);
	char *string = malloc(size+1);
	//recv data
	read( sockfd, string, size);
	string[size] = '\0';
	DBG(printf("-----recvString():%s.\n", string);)
	return string;
}

char sendFILE(int sockfd, char *fileName){
	if(!fileName)return 0; //ask for retype fileName
	//open file
	FILE* fptr = fopen(fileName, "rb");
	if(!fptr){				// file isn't exist
		sendLLONG(sockfd, -1);
		return -1;
	}

	//get file stat
	sendLLONG(sockfd, file_size(fileName));// send file size
	
	/*start sending file*/
	char buffer[BUFFER_SIZE] = "";
	//
	unsigned int len;
	while(!feof(fptr)){
		len = fread(buffer, sizeof(char), BUFFER_SIZE, fptr);
		len = write(sockfd, buffer, len);
	}

	fclose(fptr);
	//check if file is finished
	if(recvInt(sockfd)!=1)return 0;
	DBG(printf("-----sendFILE(%s)\n",fileName);)
	return 1;
}

char recvFILE(int sockfd, char* fileName){
	if(!fileName)return 0; //ask for retype fileName

	//get file size
	long long fileSize = recvLLONG(sockfd);
	if(fileSize == -1)return -1;  //file isn't exist
	//
	/*open an empty file*/
	FILE* fptr = fopen(fileName, "wb");
	/*start downloading file*/
	char buffer[BUFFER_SIZE]="";
	int temp_rate = 0;
	long long recvSize = 0, len = BUFFER_SIZE;
	while(fileSize - recvSize>0){
		len = read(sockfd, buffer, len);
		recvSize += len;
		fwrite(buffer, sizeof(char), len, fptr);
		fflush(fptr);
		if(fileSize - recvSize < len)
			len = fileSize - recvSize;
		if(recvSize*100/fileSize != temp_rate)
			printDisappearRate(temp_rate++);
	}
	printDisappearRate(temp_rate);
	fputs("\033[A\033[2K",stdout);
	rewind(stdout);	
	//tell server file is finished
	sendInt(sockfd, 1);
	DBG(printf("-----recvFILE(%s)\n",fileName);)
	return 1;
}

//



/*TOOLS*/
/*TOOLS*/
/*TOOLS*/
/*TOOLS*/
/*TOOLS*/
long long file_size(char *fileName){
	struct stat fileStat;
	if(lstat(fileName, &fileStat))return -1;
	else return fileStat.st_size;
}


char *va_strcat(char *first, ...){
  	char *str, *retbuf;
    va_list argp;
	//get length
  	int len = strlen(first) ;
	va_start(argp, first);
    while((str = va_arg(argp, char *)) != NULL)
    	len += strlen(str);
    va_end(argp);
    printf("len=%d\n",len);

    //malloc
    retbuf = malloc(len+1);
    
    strcpy(retbuf, first);

	va_start(argp, first);
    while((str = va_arg(argp, char *)) != NULL)
    	strcat(retbuf, str);
    va_end(argp);

    return retbuf;
}


int StoI(char *string){
	switch(toupper(string[0])){
	case 'C':return __CREATE_FILE;
	case 'E':return __EDIT_FILE;
	case 'R':return __REMOVE_FILE;
	case 'L':return __LIST_FILE;
	case 'D':return __SEND_FILE;
	case 'U':return __DOWNLOAD_FILE;
	case 'H':return __HELP;
	case 'Q':return _QUIT;
	case '0':printf("Server will be shutdown.\n");
			return _SHUTDOWN;
	default: return 90;
	}
}

void printDisappearRate(int rate){
	fputs("\033[A\033[2K",stdout);
	rewind(stdout);	
	for(int i=0;i<rate/10;i++)printf("=");
	printf("%d%%\n", rate);
}



