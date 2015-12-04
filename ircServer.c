#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *);

int main(int argc, char *argv[]){
	char buffer[256];
	int mSocket, newsocket, portn, n;
	socklen_t client_len;
	struct sockaddr_in server_addr, client_addr;

	if(argc<2){
		fprintf(stderr, "Error, no se especifico un puerto de conexcion");
		exit(1);
	}
	mSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(mSocket<0)
		error("Error abriendo el socket");
	bzero((char *) &server_addr, sizeof(server_addr));
	portn = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portn);
	if(bind(mSocket, (struct sockaddr *) &server_addr, sizeof(server_addr))<0)
		error("Error binding");

	while(1){
		listen(mSocket, 5);
	}
}

void error(const char *msg){
	perror(msg);
	exit(1);
}