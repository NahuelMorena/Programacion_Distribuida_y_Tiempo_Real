#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>

#define TRANSFER_NUMBER 200

bool parity(unsigned char *ptr_buffer, int buffer_size) {
    int ones = 0;
    unsigned char *ptr = ptr_buffer;
    unsigned char ch;
    for (int i=0; i < (buffer_size - 1); i++) {
        ch = *ptr;
        if (ch == '1')
            ones++;
        ptr++;
    }
    if ((ones % 2) == 0)
        return true;
    return false;
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     struct sockaddr_in serv_addr, cli_addr;
     unsigned char *dinamic_buffer;
     int n;
     int buffersize = 0;
     int bytes_count = 0;
     unsigned char *ptr = dinamic_buffer; //Puntero secundario al buffer

     //I TAKE THE ARGUMENTS
     if (argc < 3)
          error("ERROR bad arguments");
     portno = atoi(argv[1]);
     buffersize = atoi(argv[2]);

     //SOCKET CONFIGURATION
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

     bzero((char *) &serv_addr, sizeof(serv_addr));
	
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
          error("ERROR on binding");

     listen(sockfd,5);

     clilen = sizeof(cli_addr);

     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");

     //CREO EL BUFFER DINAMICO
     dinamic_buffer = (unsigned char *) malloc(buffersize);

     //LOOP
     for (int l=0; l<TRANSFER_NUMBER; l++) {

          //LEE EL MENSAJE DEL CLIENTE
          bytes_count = 0; ptr = dinamic_buffer;
          bzero(dinamic_buffer, buffersize);
          while (bytes_count < buffersize) {
               n = read(newsockfd, ptr, buffersize);
               if (n < 0)
                    error("ERROR reading from socket");
               if (n != 0) {
                    bytes_count = bytes_count + n;
                    ptr = ptr + n;
               }
          }
          
          //CHEQUEO DE TAMANIO
          if (bytes_count != buffersize)
               error("[SERVER-ERROR] The array size is wrong.\n");

          //ESCRIBO EL ARREGLO AL SOCKET
          bytes_count = 0; ptr = dinamic_buffer;
          while (bytes_count < buffersize) {
               n = write(newsockfd, ptr, buffersize);
               if (n < 0)
                    error("ERROR reading from socket");
               if (n != 0) {
                    bytes_count = bytes_count + n;
                    ptr = ptr + n;
               }
          }

     }
     free(dinamic_buffer);    //Libero la memoria reservada
     close(newsockfd);        //Cierro el socket
     
     return 0; 
}