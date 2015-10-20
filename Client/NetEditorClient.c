#include "NetEditorClient.h"
#define DEBUG 0
#define DBG( ... ) \
if (DEBUG) {  __VA_ARGS__; }


/*COMMUNICATION*/
/*COMMUNICATION*/
/*COMMUNICATION*/
/*COMMUNICATION*/
/*COMMUNICATION*/

char deal_with(int sockfd, int command){
	DBG(printf("deal_with command:%d\n",command );)
	char input[MAX_INPUT_COMMAND]="";
	char buffer[BUFFER_SIZE]="";

	switch(command){
	case _COMMAND:
		printf("----------------------------\n");
		printf("[Cmd]: ");fflush(stdout);
		scanf("%s",input);
		ask(sockfd, ItoC(input[0]));
		printf("----------------------------\n");
		if(ItoC(input[0]) == _QUIT)return 0;
		return 1;

	case _PRINTF: 
		bzero(buffer,sizeof(buffer));
		s_recvData(sockfd, buffer);
		printf("%s",buffer);
		fflush(stdout);
		return 1;
	
	case _SCANF:
		scanf("%s",input);
		sendMsg(sockfd, input);
		return 1;
	
	case _SYSTEM:
		s_recvData(sockfd, buffer);
		DBG(printf("system call: %s.\n",buffer );)
		system(buffer);
		return 1;

	case __DOWNLOAD_FILE:
		//get filename
		s_recvData(sockfd, buffer);
		//buffer means new filename;
		download_file(sockfd, buffer);
		return 1;

	case __SEND_FILE:
		//get filename
		s_recvData(sockfd, buffer);
		//buffer means filename;
		send_file(sockfd, buffer);
		return 1;
	case __UPLOAD_FILE:
		//input file name
		printf("filename: ");fflush(stdout);
		scanf("%s",input);
		sendMsg(sockfd, input);
		send_file(sockfd, input);
		return 1;
	case __REMOVE_FILE:
		//get filename
		s_recvData(sockfd, buffer);
		//buffer means filename;
		if(remove(buffer) != -1)
		DBG(printf("-------------------remove(%s)...ok\n", buffer);)
		return 1;

	case _QUIT:
		return 0;	

	default: printf("Server Error!!(connect failed)\n");

	}
	return 0;
}


int connect_socket(char ip[],unsigned int port){
	
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


void ask(int sockfd, int command){
	write(sockfd, &command, sizeof(int));
	DBG(printf("ask command:%d\n",command );)
}


void sendMsg(int sockfd, char *msg){
	int len = data_volume(msg);
	write(sockfd, &len, sizeof(int));
	write(sockfd, msg, len);
	DBG(printf("--sendMsg:%s(len=%d)\n",msg,len );)
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
				printf("%s",buffer);
				fflush(stdout);
				break;
			}
			printf("%s",buffer);
			fflush(stdout);
		}
	}
	return recv_len;

}
//


/*FILE OPERATION*/
/*FILE OPERATION*/
/*FILE OPERATION*/
/*FILE OPERATION*/
/*FILE OPERATION*/
char send_file(int sockfd, char* filename){
	if(!filename)return 0;

	//send file size
	struct stat filestat;
	if(lstat(filename, &filestat) < 0){
		filestat.st_size = 0;
		write(sockfd, &filestat.st_size, sizeof(filestat.st_size));
		return 0;
	}
	DBG(printf("sending %s...size:%lld\n",filename, filestat.st_size);)
	write(sockfd, &filestat.st_size, sizeof(filestat.st_size));
	
	//open file
	FILE* file_p;
	file_p = fopen(filename, "rb ");

	unsigned char buffer[BUFFER_SIZE];
	long long len;
	while(!feof(file_p)){
		bzero(buffer,sizeof(buffer));
		len = fread(buffer, sizeof(char), BUFFER_SIZE, file_p);
		len = write(sockfd, buffer, len);
	}

	fclose(file_p);
	DBG(printf("-------------------send_file(%s)...ok\n",filename);)
	return 1;
}


void download_file(int sockfd, char *filename){
	DBG(printf("downloading %s...\n",filename );)

	struct stat filestat;
	char choice[10];
	char new_filename[MAX_FILE_NAME];
	//file exist
	while(lstat(filename, &filestat) >= 0){
		printf("file \"%s\" is exist.\n ",filename);
		fflush(stdout);

		do{
			printf("[O]:overwrite [R]:rename --" );
			fflush(stdout);
			scanf("%s",choice);
		}while(toupper(choice[0]) != 'O' && toupper(choice[0]) != 'R');

		if(toupper(choice[0]) == 'R'){
			printf("filename: ");
			fflush(stdout);
			scanf("%s",new_filename);
			filename = new_filename;
		}
		else break;
	}

	long long real_len, recv_len=0, len = BUFFER_SIZE;
	//get file size
	read(sockfd, &real_len, sizeof(long long));
	if(real_len <= BUFFER_SIZE) len = real_len;
	DBG(printf("downloading %s...size:%lld\n",filename,real_len );)


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
	DBG(printf("-------------------download_file(%s)...ok\n",filename);)
	return ;
}



/*TOOLS*/
/*TOOLS*/
/*TOOLS*/
/*TOOLS*/
/*TOOLS*/

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

int ItoC(char input){
	switch(toupper(input)){
	case 'C':return __CREATE_FILE;
	case 'E':return __EDIT_FILE;
	case 'R':return __REMOVE_FILE;
	case 'L':return __LIST_FILE;
	case 'D':return __SEND_FILE;
	case 'U':return __DOWNLOAD_FILE;
	case 'H':return _HELP;
	case 'Q':return _QUIT;
	case '0':printf("Server will be shutdown.\n");
			return _SHUTDOWN;
	default: return 90;
	}
}

