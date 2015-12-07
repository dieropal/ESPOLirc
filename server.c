#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
 
#include<pthread.h> 
 
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    char *message;
     char buffer[256];
    //Creando el socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("No se ha podido crear un socket");
    }
     
    //Preparndo la estrutura sockaddr_in
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Enlazando la el socket a una direccion de servidor y a un puerto especifico 
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("Enlazado fallo");
        return 1;
    }
    puts("Elazado exitoso");
     
    //Llamada al sistema que indicara al socket que escuchara maximo 3 conexiones a la vez (queuue-en cola) 
    listen(socket_desc , 3);
     
    //
    puts("Esperando por alguna conexion...");
    c = sizeof(struct sockaddr_in);
    //Proceso de aceptacion a multiples conexiones y creacion de procesos por cada una recordando que se encolaran si llegan al mismo tiempo
    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("CONEXION ACEPTADA");
         
        //Respuesta al cliente
        message = "HOLA CLIENTE , YO HE RECIVIDO TU CONEXION. Y AHORA TE ASIGNARE UN MANEJADOR DE PROCESO SOLO PARA TI\n";
        write(new_socket , message , strlen(message));




         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("no se pudo crear un thread");
            return 1;
        }
        //pthread_join( sniffer_thread , NULL); // esperar, pero se cambia la metodologia
        puts("MANEJADOR ASIGNADO");

        bzero(buffer,256);
        read(new_socket,buffer,255);
        
    }
     
    if (new_socket<0)
    {
        perror("llamada acept fallida");
        return 1;
    }
     
    return 0;
}
 
/*
 * Manejador para el cliente
 * */
void *connection_handler(void *socket_desc)
{
    //obtener el descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
     
    //Enviamos algunos mensajes al cliente
    message = "MANEJADOR ESTABLECIDO PARA ESTE CLIENTE\n";
    write(sock , message , strlen(message));
     
    message = "Tipea algo para poder repetirlo \n";
    write(sock , message , strlen(message));
     
    //Recibe un mensaje de cliente
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {

        //ENVIO UN MENSAJE A EL PROPIO CLIENTE 
        write(sock , client_message , strlen(client_message));

         
    }
     
    if(read_size == 0)
    {
        puts("Cliente desconectado");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv fallo");
    }
         
    //desalojamos el descriptor
    
    free(socket_desc);
     
    return 0;
}