#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>

//Libreria utilizada para obtener los tiempos de comunicación
#include <time.h>

//Espacio que se reservara para el buffer
#define BUFFER_SIZE 1000000

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
	//TOMA EL NUMERO DE PUERTO DE LOS ARGUMENTOS
    portno = atoi(argv[2]);
	
	//CREA EL FILE DESCRIPTOR DEL SOCKET PARA LA CONEXION
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//AF_INET - FAMILIA DEL PROTOCOLO - IPV4 PROTOCOLS INTERNET
	//SOCK_STREAM - TIPO DE SOCKET 
	
    if (sockfd < 0) 
        error("ERROR opening socket");
	
	//TOMA LA DIRECCION DEL SERVER DE LOS ARGUMENTOS
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	
	//COPIA LA DIRECCION IP Y EL PUERTO DEL SERVIDOR A LA ESTRUCTURA DEL SOCKET
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
     serv_addr.sin_port = htons(portno);
	
	//DESCRIPTOR - DIRECCION - TAMAÑO DIRECCION
    if (connect(sockfd,&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    //Se completa el campo de memoria que referencia la variable buffer con el caracter 'x'
    bzero(buffer,BUFFER_SIZE);

    for (int j=0; j < BUFFER_SIZE; j++){
        strcat(buffer,"x");
    }

    printf("Cantidad de caracteres a enviar: %zd\n", strlen(buffer));

    //Empieza a contar el tiempo de comunicación desde que se enviar el mensaje al servidor hasta que recibe una respuesta
    clock_t start;
    start = clock();

    //ENVIA UN MENSAJE AL SOCKET
	n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,BUFFER_SIZE);
	
    //ESPERA RECIBIR UNA RESPUESTA
	n = read(sockfd,buffer,BUFFER_SIZE);
    if (n < 0) 
         error("ERROR reading from socket");
    
    //Calcular el tiempo utilizado en segundos para mostrarlo en pantalla.
    clock_t end;
    end = clock();

    double time_taken = ((double) end - start)/CLOCKS_PER_SEC;
    printf("Tiempo consumido en la comunicacion del lado del cliente: %f\n", time_taken);
    return 0;
}
