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


void StoI(int sockfd, char *string){
	switch(toupper(string[0])){
	case 'C':ask(sockfd, __CREATE_FILE);break;
	case 'E':ask(sockfd, __EDIT_FILE);break;
	case 'R':ask(sockfd, __REMOVE_FILE);break;
	case 'L':ask(sockfd, __LIST_FILE);break;
	case 'D':ask(sockfd, __SEND_FILE);break;
	case 'U':ask(sockfd, __DOWNLOAD_FILE);break;
	case 'H':ask(sockfd, __HELP);break;
	case 'Q':ask(sockfd, _QUIT);break;
	case 'B':ask(sockfd, __BATCH);
			printf("[C]reate\t");
			printf("[R]emove\n");
			printf("[D]ownload\t");
			printf("[U]pload\n");
			printf("[Batch-CMD] ");
			scanf("%s", string);
			switch(toupper(string[0])){
				case 'C':ask(sockfd, __AUTO_CREATE_FILE);break;
				case 'R':ask(sockfd, __AUTO_REMOVE_FILE);break;
				case 'D':ask(sockfd, __AUTO_SEND_FILE);break;
				case 'U':ask(sockfd, __AUTO_DOWNLOAD_FILE);break;
				case 'Q':printf("Quit batch mode.\n");
				default: ask(sockfd, _NO_COMMAND);
				break;
			}
			break;
	case '0':printf("Server will be shutdown.\n");
			ask(sockfd, _SHUTDOWN);
			break;
	default: ask(sockfd, _NO_COMMAND);
	}
}

void deal_with(int sockfd, int command){
	DBG(printf("server command: %d\n",command);)
	char input[MAX_FILE_NAME]="", freeFlag = 0;
	char *msg = NULL;
	char *name[MAX_BATCH_FILE] = {""};
	int i,j;

	switch(command){
		case _SHUTDOWN :
		case _QUIT :return;
		case _NO_COMMAND:break;
		case _COMMAND :
			printf("--------------------------------------------------------\n");
			printf("[CMD] ");fflush(stdout);
			scanf("%s",input);
			printf("--------------------------------------------------------\n");
			StoI(sockfd, input);
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

		case __SEND_FILE:
			printf("filename: ");fflush(stdout);
			scanf("%s",input);
			//get filesize
			if(file_size(input) == -1)
				printf("\"%s\" is not exist.\n",input);
			
			msg = input;
			freeFlag = 0;
			sendString(sockfd, msg);
		case __AUTO_SEND_FILE:
			if(!msg) msg = recvString(sockfd);
			sendFILE(sockfd, msg);
			break;

		case __DOWNLOAD_FILE:
			printf("filename: ");fflush(stdout);
			scanf("%s",input);
			sendString(sockfd, input);
			msg = input;
			freeFlag = 0;
			//get filesize
			if(recvLLONG(sockfd) == -1)
				printf("\"%s\" is not exist.\n",msg);
		case __AUTO_DOWNLOAD_FILE:
			if(!msg)msg = recvString(sockfd);
			recvFILE(sockfd, msg);
			break;

		case __BATCH:
			printf("Use ',' to split the filename\n");
			printf("(Ex) [Batch Mode] filename1,filename2,filename3 \n");
			printf("[Batch Mode] ");fflush(stdout);
			scanf("%s",input);
			msg = input;
			freeFlag = 0;
			// send how man files
			split(name,msg,",");
			for(i=0;i<MAX_BATCH_FILE;i++)if(name[i]==NULL)break;
			sendInt(sockfd, i);

			for(j=0;j<i;j++){
				sendString(sockfd, name[j]);
				deal_with(sockfd, recvInt(sockfd));
			}

			break;
		default:printf("ERROR!?\n");
	}

	if(msg && freeFlag)
		free(msg);

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
	int r = read( sockfd, &num, sizeof(int));
	DBG(printf("-----recvInt(%d/%lu): %d\n",r,sizeof(int),num);)
	return num; 
}
void sendLLONG(int sockfd, long long num){
	DBG(printf("-----sendLLONG(): %lld\n",num);)
	write( sockfd, &num, sizeof(long long));
}
long long recvLLONG(int sockfd){
	long long num=0;
	int r = read( sockfd, &num, sizeof(long long));
	DBG(printf("-----recvLLONG(%d/%lu): %lld\n",r,sizeof(long long), num);)
	return num;
}

void sendString(int sockfd, char* string){
	//send string size
	int size = strlen(string);
	sendInt( sockfd, size);
	//send string
	write( sockfd, string, size);
	DBG(printf("-----sendString():%s\nlen=%d\n", string,size);)
}
char *recvString(int sockfd){
	//recv data size
	int size = recvInt(sockfd);
	char *string = malloc(size+1);
	//recv data
	int r =read( sockfd, string, size);
	string[size] = '\0';
	DBG(printf("-----recvString(%d/%d):%s\n", r, size, string);)
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
	long long fileSize = 0;
	fileSize = recvLLONG(sockfd);


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
	DBG(printf("-----recvFILE(%s)\n",fileName);)
	sendInt(sockfd, 1);
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


void printDisappearRate(int rate){
	fputs("\033[A\033[2K",stdout);
	rewind(stdout);	
	for(int i=0;i<rate/10;i++)printf("=");
	printf("%d%%\n", rate);
}

void split(char **arr, char *str, const char *del) {
	char *s = strtok(str, del);

	while(s != NULL) {
		*arr++ = s;
		s = strtok(NULL, del);
	}
 }



