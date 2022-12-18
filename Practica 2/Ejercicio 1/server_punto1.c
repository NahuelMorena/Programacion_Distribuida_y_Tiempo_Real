/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     //Se valida que se pasen todos los parametros requeridos
     if (argc < 3) {
          fprintf(stderr,"usage %s port_number buffer_size\n", argv[0]);
          exit(1);
     }

     //Asigna a la variable el tamaño que poseera el buffer
     int buffer_size = atoi(argv[2]);
     unsigned char buffer[buffer_size];

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
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
          sizeof(serv_addr)) < 0) 
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
     bzero(buffer,buffer_size);

     //LOOP 100 repeticiones
     for (int i=0; i<100; i++){
          int cont = 0;
          do {
               //LEE EL MENSAJE DEL CLIENTE
               n = read(newsockfd, &buffer[cont], buffer_size);
               if (n < 0)
                    error("ERROR reading from socket");
               cont += n;
          } while (cont < buffer_size);

	     //RESPONDE AL CLIENTE
          n = write(newsockfd,buffer,buffer_size);
          if (n < 0) error("ERROR writing to socket");
          bzero(buffer,buffer_size);
     }

     close(newsockfd);

     return 0; 
}