#include "NTE_Client.h"

int main(int argc , char **argv){

	char ip[] = "127.0.0.1";
	int port = 5381;
	if(argv[1])port = atoi(argv[1]);
	
	int sockfd = connect_to("127.0.0.1", port);
	if(sockfd == -1){
		printf("ERROR: cannot connect to server,ip=%s:%d\n", ip, port);
		return -1;
	}
	//
	int command = 1;

	while(sockfd != -1 && command > 0){
		command = recvInt(sockfd);
		deal_with(sockfd, command);
	}


close(sockfd);
return 0;
}