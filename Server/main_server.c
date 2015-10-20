#include "NetEditorServer.h"


int main(int argc, char **argv){

	int port = 5381;
	if(argv[1])port = atoi(argv[1]);

	int sockfd = init_socket(port);
	struct sockaddr_in clientAddr;
	unsigned int addrlen = sizeof(clientAddr);
	int clientfd;
	int command;

	while(1){
		/*Listen & accept*/
		listen(sockfd, MAX_CONNECTION);
		printf("Listening for connection.\n" );

		clientfd = accept(sockfd, (struct sockaddr*)&clientAddr, &addrlen);
		printf("client ip:%s is connect\n", inet_ntoa(clientAddr.sin_addr));
		//

		command=_COMMAND;
		clientPrologue(clientfd);
		while(1){
			ask(clientfd, _COMMAND);
			recv(clientfd, &command, sizeof(int), 0);
			printf("get command: %d\n",command);
			if(command == _QUIT || command == _SHUTDOWN){
				ask(clientfd, _QUIT);
				break;
			}

			deal_with(clientfd,command);
		}

		printf("close(clientfd=%d)\n",clientfd);
		/* close(client) */
		close(clientfd);
		if(command == _SHUTDOWN)break;
	}	

printf("Server had been shutdown.\n");
return 0;
}



