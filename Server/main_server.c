#include "NTE_Server.h"


int main(int argc , char **argv){

	int port = 5381;
	if(argv[1])port = atoi(argv[1]);
	int sockfd = init_socket(port);

	struct sockaddr_in clientAddr;
	unsigned int addrlen = sizeof(clientAddr);
	int clientfd, command = _PRINTF;

	while(command != _SHUTDOWN){
		/*Listen & accept*/
		listen(sockfd, MAX_CONNECTION);
		printf("Listening for connection.\n" );

		clientfd = accept(sockfd, (struct sockaddr*)&clientAddr, &addrlen);
		printf("client ip:%s is connect\n", inet_ntoa(clientAddr.sin_addr));
		
		/*MAIN LOOP*/
		command = _PRINTF;
		clientPrologue(clientfd);
		// sendInt(clientfd, __HELP);
		while(clientfd != -1 && command > 0){
			ask(clientfd, _COMMAND);
			command = recvInt(clientfd);
			deal_with(clientfd, command);
		}

		ask(clientfd, _QUIT);
		close(clientfd);
	}

close(sockfd);
return 0;
}