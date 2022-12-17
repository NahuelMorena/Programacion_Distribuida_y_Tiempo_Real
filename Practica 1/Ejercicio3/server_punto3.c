/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

//Libreria utilizada para obtener los tiempos de comunicación
#include <time.h>

//Espacio que se reservara para el buffer 
#define BUFFER_SIZE 1000000

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


     int cont = 0;
     do {
          //LEE EL MENSAJE DEL CLIENTE
          n = read(newsockfd,buffer,BUFFER_SIZE);
          if (n < 0) 
               error("ERROR reading from socket");
          cont += n;
     } while (cont < BUFFER_SIZE);
	
     printf("El mensaje es de %li bytes\n",strlen(buffer));
	
     clock_t start;
     start = clock();

	//RESPONDE AL CLIENTE
     n = write(newsockfd,"I got your message",18);
     if (n < 0) 
          error("ERROR writing to socket");

     //Calcula el tiempo utilizado en segundos para mostrarlo en pantalla.
     clock_t end;
     end = clock();

     double time_taken = ((double) end - start)/CLOCKS_PER_SEC;
     printf("Tiempo consumido en la comunicación del lado del servidor: %f\n", time_taken);

     return 0; 
}
