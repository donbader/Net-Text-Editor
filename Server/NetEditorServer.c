#include "NetEditorServer.h"
#define DEBUG 0
#define DBG( ... ) \
if (DEBUG) {  __VA_ARGS__; }

/*COMMUNICATION*/
/*COMMUNICATION*/
/*COMMUNICATION*/
/*COMMUNICATION*/
/*COMMUNICATION*/

void deal_with(int sockfd, int command){
	char *filename;
	char buffer[BUFFER_SIZE]="";

	switch(command){

	case __CREATE_FILE:
		create_file(sockfd);
		break;

	case __EDIT_FILE:
		filename = clientGetFilename(sockfd);
		edit_file(sockfd, filename);
		free(filename);
		break;

	case __REMOVE_FILE:
		filename = clientGetFilename(sockfd);
		remove_file(sockfd,filename);
		free(filename);
		break;
	case __LIST_FILE:
		clientPrintf(sockfd, list_file("."));
		break;

	case __SEND_FILE:
		filename = clientGetFilename(sockfd);
		if(filename){
			ask(sockfd, __DOWNLOAD_FILE);
			//send file name
			sendMsg(sockfd, filename);
			//send file
			send_file(sockfd,filename);
			free(filename);
		}
		break;
	case __DOWNLOAD_FILE:
		ask(sockfd, __UPLOAD_FILE);
		//get file name
		s_recvData(sockfd, buffer);
		download_file(sockfd, buffer);
		break;

	case _HELP:
		clientHelp(sockfd);
		break;

	default: ;
		printf("get weird command : %d\n",command);
		clientPrintf(sockfd, "There is no this kind of command!\n");
	}

}


int init_socket(unsigned int port){
	
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


void ask(int sockfd, int command){
	write(sockfd, &command, sizeof(int));
	DBG(printf("ask command: %d\n", command);)
}

void sendMsg(int sockfd, char *msg){
	int len = data_volume(msg);
	write(sockfd, &len, sizeof(int));
	write(sockfd, msg, len);
	DBG(printf("--sendMsg:%s(len=%d)\n",msg,len);)
}

int s_recvData(int sockfd, char *buffer){
	int real_len, recv_len=0, len;
	read(sockfd, &real_len, sizeof(int));
	if(real_len <= BUFFER_SIZE)
		recv_len = read(sockfd, buffer, real_len);
	
	else{
		while((len = read(sockfd, buffer, BUFFER_SIZE)) != -1){
			recv_len += len;
			if(real_len - recv_len < BUFFER_SIZE){
				len = read(sockfd, buffer, real_len - recv_len);
				recv_len += len;
				break;
			}
		}
	}

	DBG(printf("recv:\n%s\n", buffer);)
	return recv_len;

}

/*TOOLS*/
/*TOOLS*/
/*TOOLS*/
/*TOOLS*/
/*TOOLS*/

void clientPrintf(int sockfd, char* string){
	ask(sockfd, _PRINTF);
	sendMsg(sockfd, string);
}
void clientScanf(int sockfd, char* string){
	ask(sockfd, _SCANF);
	s_recvData(sockfd, string);
}
void clientSystem(int sockfd, char* string){
	ask(sockfd, _SYSTEM);
	sendMsg(sockfd, string);
}
void clientPrologue(int sockfd){
	clientPrintf(sockfd, "########################################################################################\n");
	clientPrintf(sockfd, "|                            Welcome to Internet Editor                                |\n");
	clientPrintf(sockfd, "########################################################################################\n");
	clientPrintf(sockfd, "|                    There are some option you can choose below                        |\n");
	clientPrintf(sockfd, "########################################################################################\n");
	clientHelp(sockfd);
	clientPrintf(sockfd, "########################################################################################\n");

}

void clientHelp(int sockfd){
	clientPrintf(sockfd, "[C]reate\t");
	clientPrintf(sockfd, "[E]dit\t\t");
	clientPrintf(sockfd, "[R]emove\t");
	clientPrintf(sockfd, "[L]ist\n");
	clientPrintf(sockfd, "[D]ownload\t");
	clientPrintf(sockfd, "[U]pload\t");
	clientPrintf(sockfd, "[H]elp\t\t");
	clientPrintf(sockfd, "[Q]uit\n");
}

char* clientGetFilename(int sockfd){
	// ask for filename
	char *filename = calloc(MAX_FILE_NAME,sizeof(char));
	clientPrintf(sockfd, "filename: ");
	clientScanf(sockfd, filename);

	struct stat filestat;
	if (lstat(filename, &filestat) < 0){
		// printf("\"%s\" is not exist.\nPlease try [C] for creating file.\n", filename);
		char *msg = va_strcat("\"", filename, "\"", " is not exist.\n",0);
		clientPrintf(sockfd, msg);
		free(msg);
		free(filename);		
		return NULL;
	}
	return filename;
}


char file_exist(char* filename){
	struct stat filestat;
	if (lstat(filename, &filestat) < 0)return 0;
	else return 1;
}

unsigned long data_volume(char* data){
	unsigned int i=0;
	while((*data++)!='\0')i++;
	return i;
}

char *va_strcat(char *first, ...){
	size_t len = strlen(first);
	char *retbuf;
	va_list argp;
	char *p;

	if(first == NULL)
		return NULL;

	va_start(argp, first);
	//get length
	while((p = va_arg(argp, char *)) != NULL)
		len += strlen(p);
	va_end(argp);
	retbuf = malloc(len + 1);	/* +1 for trailing \0 */

	strcpy(retbuf, first);

	va_start(argp, first);
	while((p = va_arg(argp, char *)) != NULL)
		strcat(retbuf, p);
	va_end(argp);
	return retbuf;
}





/*FILE OPERATION*/
/*FILE OPERATION*/
/*FILE OPERATION*/
/*FILE OPERATION*/
/*FILE OPERATION*/


char* list_file(char *path){
	printf("list_file()...");

	DIR *dir_p;
	struct dirent* entry;
	dir_p = opendir(path);
	char *output = malloc(1*KB*sizeof(char));
	// char output[1*KB] = "";

	while((entry = readdir(dir_p)) != NULL){
		strcat(output, entry->d_name);
		strcat(output,"\n");
	}

	strcat(output,"\0");

	closedir(dir_p);
	printf("...ok\n");
	return output;
}

void create_file(int sockfd){
	//ask for filename
	char filename[MAX_FILE_NAME]="";

	clientPrintf(sockfd, "filename: ");
	clientScanf(sockfd, filename);

	//check if exist
	if(file_exist(filename)){

		//printf("\"%s\" is exist, do you want to overwrite?\n", filename);
		char *msg = va_strcat("\"", filename, "\"", " is exist, do you want to overwrite?\n",0);
		clientPrintf(sockfd,msg);
		free(msg);
		//
		char choice[10];

		do{
			//printf("[Y/N]");
			clientPrintf(sockfd, "[Y/N]");
			//scanf("%s",choice);
			clientScanf(sockfd, choice);

		}while(toupper(choice[0]) != 'Y' && toupper(choice[0]) != 'N');



		if(toupper(choice[0]) == 'N'){
			//printf("\"%s\" hasn't been overwrite ! \n", filename);
			char *msg = va_strcat("\"", filename, "\"", " hasn't been overwrite !\n",0);
			clientPrintf(sockfd,msg);
			free(msg);
			//
			return;
		}
	}

	FILE* file_p;
	file_p = fopen(filename, "w");
	fclose(file_p);
	printf("-------------------create_file()...ok\n");

}


void remove_file(int sockfd, char* filename){
	if(!filename) return;
	//

	// printf("\"%s\" has been removed.\n", filename);
	remove(filename);
	char *msg = va_strcat("\"", filename, "\"", " has been removed.\n",0);
	clientPrintf(sockfd,msg);
	free(msg);
}


void edit_file(int sockfd, char *filename){
	if(!filename)return;

	//rename()
	//send file as .file.temp
	ask(sockfd, __DOWNLOAD_FILE);
	char *tempName = va_strcat(".temp.",filename,0);
	printf("rename:%s",tempName);
	sendMsg(sockfd, tempName);
	send_file(sockfd, filename);

	//edit
	char *command = va_strcat("vim ",tempName,0);
	ask(sockfd, _SYSTEM);
	sendMsg(sockfd, command);

	//download file (.file.temp)
	ask(sockfd, __SEND_FILE);
	sendMsg(sockfd, tempName);
	download_file(sockfd, filename);

	//remove client file
	ask(sockfd, __REMOVE_FILE);
	sendMsg(sockfd, tempName);

	free(tempName);
	free(command);
}

char send_file(int sockfd, char* filename){
	if(!filename)return 0;
	DBG(printf("sending %s...\n",filename);)


	//send file size
	struct stat filestat;
	lstat(filename, &filestat);
	write(sockfd, &filestat.st_size, sizeof(filestat.st_size));
	
	//open file
	FILE* file_p;
	file_p = fopen(filename, "rb ");

	unsigned char buffer[BUFFER_SIZE];
	bzero(buffer,sizeof(buffer));
	long long len;
	while(!feof(file_p)){
		len = fread(buffer, sizeof(char), BUFFER_SIZE, file_p);
		len = write(sockfd, buffer, len);
	}

	fclose(file_p);
	printf("-------------------send_file(%s)...ok\n",filename);
	return 1;
}

void download_file(int sockfd, char *filename){
	if(!filename)return;

	long long real_len, recv_len=0, len = BUFFER_SIZE;
	//get file size
	read(sockfd, &real_len, sizeof(long long));
	if(real_len <= 0) return;
	if(real_len <= BUFFER_SIZE) len = real_len;
	printf("downloading %s...size:%lld\n",filename,real_len );

	//file open
	FILE* file_p;
	file_p = fopen(filename, "wb");
	
	unsigned char buffer[BUFFER_SIZE];
	bzero(buffer, sizeof(buffer));

	//download
	while(real_len - recv_len > 0){
		len = read(sockfd, buffer, len);
		recv_len += len;
		fwrite(buffer, sizeof(char), len, file_p);
		fflush(file_p);
		if(real_len - recv_len < len)
			len = real_len - recv_len;
	}
	
	printf("-------------------download_file(%s)...ok\n",filename);
	return ;
}

