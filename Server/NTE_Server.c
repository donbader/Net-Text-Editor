#include "NTE_Server.h"

#ifndef DEBUG
#define DEBUG 1
#define DBG( ... ) \
		if (DEBUG) {  __VA_ARGS__; } 
#endif 

/*MAIN DEALING FUNCTION*/
/*MAIN DEALING FUNCTION*/
/*MAIN DEALING FUNCTION*/
/*MAIN DEALING FUNCTION*/
/*MAIN DEALING FUNCTION*/

void deal_with(int sockfd, int command){
	DBG(printf("client command: %d\n",command);)
	char *msg=NULL;
	int i,num_of_files ;

	switch(command){
		case _SHUTDOWN:
		case _QUIT:return;
		case __HELP:clientHelp(sockfd);break;
		case __CREATE_FILE:
			clientPrintf(sockfd, "filename: ");
			msg = clientScanf(sockfd);
			auto_command(sockfd, command, msg);
			break;
		case __EDIT_FILE:
			clientPrintf(sockfd, "filename: ");
			msg = clientScanf(sockfd);
			if(file_size(msg) == -1){
				char *str = va_strcat("\"",msg,"\" is not exist.\n", 0);
				clientPrintf(sockfd, str);
				free(str);
				break;
			}
			//send file
			ask(sockfd, __AUTO_DOWNLOAD_FILE);
			char *tempName = va_strcat(".temp.",msg,0);
			sendString(sockfd, tempName);
			sendFILE(sockfd, msg);
			//call edit
			ask(sockfd, _SYSTEM);
			char *cmd = va_strcat("vi ",tempName);
			sendString(sockfd, cmd);
			//download file
			ask(sockfd, __AUTO_SEND_FILE);
			sendString(sockfd, tempName);
			recvFILE(sockfd, msg);

			//remove file
			ask(sockfd, __REMOVE_FILE);
			sendString(sockfd, tempName);
			//

			free(tempName);
			break;

		case __REMOVE_FILE:
			clientPrintf(sockfd, "filename: ");
			ask(sockfd, _SCANF);
			auto_command(sockfd, command, msg);
			break;
		case __LIST_FILE:
			msg = list_file(".");
			clientPrintf(sockfd,msg);
			break;

		case __SEND_FILE:
			ask(sockfd, __DOWNLOAD_FILE);
			//get filename
			msg = recvString(sockfd);
			sendLLONG(sockfd, file_size(msg));
			auto_command(sockfd, __AUTO_SEND_FILE, msg);
			break;

		case __DOWNLOAD_FILE:
			ask(sockfd, __SEND_FILE);
			auto_command(sockfd, __AUTO_DOWNLOAD_FILE, msg);
			break;
				
		case __BATCH:
			//get batch operation
			command = recvInt(sockfd);
			//interpret command
			if(command == _NO_COMMAND)break;
			ask(sockfd, __BATCH);
			num_of_files = recvInt(sockfd);
			printf("server will deal_with() %d files\n", num_of_files);

			int cmdToClient;
			switch(command){
				case __CREATE_FILE:case __REMOVE_FILE:
					cmdToClient = _NO_COMMAND;break;
				case __AUTO_SEND_FILE: cmdToClient = __AUTO_DOWNLOAD_FILE;break;
				case __AUTO_DOWNLOAD_FILE: cmdToClient = __AUTO_SEND_FILE;break;
				default : printf("ERROR!?command%d\n",command);
			}

			for(i=0;i<num_of_files;i++){
				msg = recvString(sockfd);
				ask(sockfd, cmdToClient);
				auto_command(sockfd, command, msg);
			}
			break;

		default:
			clientPrintf(sockfd, "No such command!\n");
	}
	if(msg)free(msg);
}

/*Task*/
void clientPrintf(int sockfd, char *str){
	ask(sockfd, _PRINTF);
	sendString(sockfd, str);
	DBG(printf("client print : %s\n",str);)
}
char *clientScanf(int sockfd){
	ask(sockfd, _SCANF);
	DBG(printf("client scanf() \n");)
	return recvString(sockfd);
}
void clientSystem(int sockfd, char *cmd){
	ask(sockfd, _SYSTEM);
	sendString(sockfd, cmd);
	DBG(printf("client system call : %s\n",cmd);)
}

void clientPrologue(int sockfd){
	clientPrintf(sockfd, "############################################################################\n");
	clientPrintf(sockfd, "#                        Welcome to Internet Editor                        #\n");
	clientPrintf(sockfd, "############################################################################\n");
	clientPrintf(sockfd, "#                There are some option you can choose below                #\n");
	clientPrintf(sockfd, "############################################################################\n");
	clientHelp(sockfd);
	clientPrintf(sockfd, "############################################################################\n");
}

void clientHelp(int sockfd){
	clientPrintf(sockfd, "[C]reate\t");
	clientPrintf(sockfd, "[E]dit\t\t");
	clientPrintf(sockfd, "[R]emove\t");
	clientPrintf(sockfd, "[L]ist\n");
	clientPrintf(sockfd, "[D]ownload\t");
	clientPrintf(sockfd, "[U]pload\t");
	clientPrintf(sockfd, "[H]elp\t\n");
	clientPrintf(sockfd, "[B]atch\t\t");
	clientPrintf(sockfd, "[Q]uit\n");
}

char clientDetermine(int sockfd,char *msg, char *opt1, char *opt2){

	clientPrintf(sockfd,msg);
	char *choice = va_strcat("[",opt1,"/",opt2,"] ",0);
	clientPrintf(sockfd,choice);
	char *opt = clientScanf(sockfd);

	while(toupper(opt[0]) != opt1[0] && toupper(opt[0]) != opt2[0]){
		clientPrintf(sockfd, msg);
		clientPrintf(sockfd,choice);
		opt = clientScanf(sockfd);
	}
	printf("-----clientDetermine(%c)",opt[0]);
	char c = opt[0];
	free(opt);
	free(choice);
	return toupper(c);
}

void auto_command(int sockfd, int command, char*msg){
	char choice, *choiceMsg;

	if(!msg)msg = recvString(sockfd);
	switch(command){
	case __CREATE_FILE:
		if(file_size(msg) != -1){
			choice = 'R';
			choiceMsg = va_strcat("\"",msg,"\" is exist.\n[O]verwrite or [R]ename?\n",0);
		}
		else choice = 'O';
		while(choice == 'R'){
			choice = clientDetermine(sockfd, choiceMsg, "O", "R");
			if(choice == 'R'){
				clientPrintf(sockfd, "filename: ");
				msg = clientScanf(sockfd);
			}
			if(choice == 'R' && file_size(msg) != -1)continue;
			else break;
		}
		create_file(msg);
		break;
	case __REMOVE_FILE:
		if(remove(msg) == -1){
			char *str = va_strcat("\"",msg,"\" is not exist.\n", 0);
			clientPrintf(sockfd, str);
			free(str);
		}
		printf("-----remove_file(%s)...ok\n", msg);
		break;
	case __AUTO_DOWNLOAD_FILE:
		sendString(sockfd, msg);
		recvFILE(sockfd, msg);break;
	case __AUTO_SEND_FILE:
		sendString(sockfd, msg);
		sendFILE(sockfd, msg);break;

	}

}


/*FILE OPERATION*/
/*FILE OPERATION*/
/*FILE OPERATION*/
/*FILE OPERATION*/
/*FILE OPERATION*/

char* list_file(char *path){

	DIR *dir_p;
	struct dirent* entry;
	dir_p = opendir(path);
	char *output = calloc(1*KB,sizeof(char));
	// char output[1*KB] = "";

	while((entry = readdir(dir_p)) != NULL){
		strcat(output, entry->d_name);
		strcat(output,"\n");
	}

	strcat(output,"\0");

	closedir(dir_p);
	printf("-----list_file()...ok\n");
	return output;
}

void create_file(char* fileName){
	FILE* fptr = fopen(fileName,"w");
	fclose(fptr);
	printf("-----create_file()...ok\n");
}



/*COMMUNICATION*/
/*COMMUNICATION*/
/*COMMUNICATION*/
/*COMMUNICATION*/
/*COMMUNICATION*/

int init_socket(int port){
	
	/*create socket*/
	//PF_INET = Protocol Family _ InterNET
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	//let the address be reused; (Solution of cannot connet when reopen)
	int flag = 1; 
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(int));

	/*server address struct*/
	//sockaddr_"in" for internet
	struct sockaddr_in serverAddr;

	/*init server address*/
	//every bit turn to zero
	bzero(&serverAddr,sizeof(serverAddr));
	//AF_INET = Address Family_ InterNET
	serverAddr.sin_family = AF_INET;  
	//host to "n"etwork byte order in "s"hort
	serverAddr.sin_port = htons(port);

	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);//let any address connect to this

	//bind() = attach local endpoint to a socket
	// and assign the port number to socket.
	bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

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
	long long num;
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
	DBG(printf("-----sendString():%s\nlen=%d\n", string, size);)
}
char *recvString(int sockfd){
	//recv data size
	int size = recvInt(sockfd);
	char *string = malloc(size+1);
	//recv data
	int r = read( sockfd, string, size);
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
	printf("-----sendFILE(%s)\n",fileName);
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
	printf("-----recvFILE(%s)\n",fileName);
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


