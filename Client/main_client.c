#include "NetEditorClient.h"



int main(int argc, char **argv){

	int port = 5381;
	if(argv[1])port = atoi(argv[1]);

	int sockfd = connect_socket("127.0.0.1", port);

	int command;
	read(sockfd, &command, sizeof(int));

	while(deal_with(sockfd, command))
		read(sockfd, &command, sizeof(int));
	
	printf("Client had been close.\n");
	/* Close connection */
	close(sockfd);


}