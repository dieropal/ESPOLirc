#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define NUM_CLIENTS 5

void error(const char *);
void *processToClient(void *);

//estructura con informacion de un cliente
typedef struct Client{
	struct sockaddr_in sock_in;
	int id;
	int cSocket;
};


struct Client clients[5];

int main(int argc, char *argv[]){
	int mSocket, newsocket, portn;
	socklen_t client_len;
	struct sockaddr_in server_addr, client_addr;
	pthread_t pthread_list[NUM_CLIENTS];
	int id;
	

	if(argc<2){
		fprintf(stderr, "Error, no se especifico un puerto de conexcion");
		exit(1);
	}
	//llamada al sistema para crear el socket
	mSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(mSocket<0)
		error("Error abriendo el socket");

	bzero((char *) &server_addr, sizeof(server_addr));
	portn = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portn);
	
	/*llamada al sistema para enlazar el socket a la direccion y el puerto en 
	  el que esta corriendo el servidor*/
	if(bind(mSocket, (struct sockaddr *) &server_addr, sizeof(server_addr))<0)
		error("Error binding");

	for(id=0;id<NUM_CLIENTS;id++){
		/*llamada al sistema que permite al proceso escuchar en el socket por
		una conexion*/
	    listen(mSocket, 5);
		client_len = sizeof(client_addr);
	  	//llamada al sistema que bloquea el proceso hasta que un cliente se conecte al servidor
	  	newsocket = accept(mSocket, (struct sockaddr *) &client_addr, &client_len);
	  	bzero((char *) &clients[id],sizeof(clients[id]));
	  	clients[id].sock_in.sin_addr = client_addr.sin_addr;
	  	clients[id].id = id;
	  	clients[id].cSocket = newsocket;
	  	// se crea un hilo para cada cliente 
		pthread_create(&pthread_list[id], NULL, processToClient, (void *) &clients[id]);	

	}
	pthread_exit(NULL);
	close(newsocket);
	close(mSocket);
}

void error(const char *msg){
	perror(msg);
	exit(1);
}

//proceso por cada cliente
void *processToClient(void * cl){
	char buffer[256], ipstr[INET_ADDRSTRLEN];
	struct Client *client;
	client = (struct Client *) cl;
	while(1){

	  	inet_ntop(AF_INET, &(client->sock_in.sin_addr), ipstr, sizeof(ipstr));
	  	bzero(buffer,256);
	  	//se bloquea el proceso hasta que haya algo que leer en el socket
		read(client->cSocket, buffer, 256);
	  	printf("Peer IP address: %s  id:%d\n", ipstr, client->id);
	  	printf("here is the mesage: %s \n", buffer);
	}
	pthread_exit(NULL);
}