/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

//Espacio que se reservara para el buffer 
#define BUFFER_SIZE 1000

//Función utilizada para implementar un hash
static unsigned long sdbm(unsigned char *str);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char buffer[BUFFER_SIZE];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

	 //CREA EL FILE DESCRIPTOR DEL SOCKET PARA LA CONEXION
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
	 //AF_INET - FAMILIA DEL PROTOCOLO - IPV4 PROTOCOLS INTERNET
	//SOCK_STREAM - TIPO DE SOCKET 
	
     if (sockfd < 0) 
        error("ERROR opening socket");

     bzero((char *) &serv_addr, sizeof(serv_addr));
     //ASIGNA EL PUERTO PASADO POR ARGUMENTO
	 //ASIGNA LA IP EN DONDE ESCUCHA (SU PROPIA IP)
	 portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
	 
	 //VINCULA EL FILE DESCRIPTOR CON LA DIRECCION Y EL PUERTO
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
          error("ERROR on binding");
			  
	 //SETEA LA CANTIDAD QUE PUEDEN ESPERAR MIENTRAS SE MANEJA UNA CONEXION		  
     listen(sockfd,5);
	 
	 // SE BLOQUEA A ESPERAR UNA CONEXION
     printf("Esperando por conexiones...\n");
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
				 
     //DEVUELVE UN NUEVO DESCRIPTOR POR EL CUAL SE VAN A REALIZAR LAS COMUNICACIONES
	 if (newsockfd < 0) 
          error("ERROR on accept");
     bzero(buffer,BUFFER_SIZE);

	//LEE EL MENSAJE DEL CLIENTE
     n = read(newsockfd,buffer,BUFFER_SIZE - 1);
     if (n < 0) 
          error("ERROR reading from socket");
     
     printf("Se recibio mensaje con %li caracteres\n",strlen(buffer));
	 
     //Lee mensaje recibido sobre tamaño del contenido del buffer 
     size_t size;
     n = read(newsockfd, &size, sizeof(size));

     if (n < 0)
          error("ERROR reading from socket");

     if (strlen(buffer) == size){
          printf("Coinciden el tamaño recibido del buffer %lu y el tamaño indicado por el cliente %lu\n", strlen(buffer),size);
     } else {
          printf("No coordinan el tamaño recibido del buffer con el tamaño indicado por el cliente\n");
     }

     //Lee mensaje recibido de hash
     long client_hash;
     n = read(newsockfd, &client_hash, sizeof(client_hash));
     if (n < 0)
          error("ERROR reading from socket");

     long server_hash = sdbm(buffer);
     if (client_hash == server_hash){
          printf("Client hash: %lu   =   Server hash: %lu\n", client_hash,server_hash);    
     } else {
          printf("No coinciden el client_hash con el server_hash\n");
     }

	 //RESPONDE AL CLIENTE
     n = write(newsockfd,"I got your message",18);
     if (n < 0) 
          error("ERROR writing to socket");
     return 0; 
}

static unsigned long sdbm(unsigned char *str){
     unsigned long hash = 0;
     int c;

     while (c = *str++)
          hash = c + (hash << 6) + (hash << 16) - hash;
     return hash;
}