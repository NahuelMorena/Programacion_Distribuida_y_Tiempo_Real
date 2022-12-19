#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <strings.h>

#define BUFFERSIZE 1024
#define TRANSFER_NUMBER 200 // Para sacar un promedio repito varias veces la transferencia

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

void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int buffersize = 0;
    int bytes_count = 0;
    unsigned char buffer[BUFFERSIZE];
    unsigned char *dinamic_buffer;
    unsigned char *ptr = dinamic_buffer; //Puntero secundario al buffer
    clock_t t0, t1; //Para tomar el tiempo
    double elapsed;
    char temp_str[1024];
    char registro[8192];

    //TOMO LOS ARGUMENTOS
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    server = gethostbyname(argv[1]);
    portno = atoi(argv[2]);
    buffersize = atoi(argv[3]);
	
	//SOCKET CONFIGURATION
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
	
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
	
    while (connect(sockfd, &serv_addr, sizeof(serv_addr)) < 0) {
        sleep(1);
    }

    //CREO EL BUFFER DINAMICO
    dinamic_buffer = (unsigned char *) malloc(buffersize);
    bzero(dinamic_buffer, buffersize);
    
    //GENERO EL ARREGLO DE BYTES - CON CEROS Y UNOS
    for (int i=0; i < (buffersize - 1); i++) {
        if ((i % 2) == 0)
            strcat(dinamic_buffer, "1");
        else
            strcat(dinamic_buffer, "0");
    }

    /*En este registro voy poniendo la info de las transferencias
    con el objetivo de su posterior análisis.*/
    
    sprintf(temp_str, "TRANSFERENCIA DE %i BYTES (%i REPETICIONES, PUERTO %i):\n", buffersize, TRANSFER_NUMBER, portno);
    strcat(registro, temp_str);

    //CALCULO LA PARIDAD
    bool parity_value = parity(dinamic_buffer, buffersize);

    //LOOP
    double sum_of_times = 0.0;          //ME GUARDO LA SUMA DE LOS TIEMPOS PARA LUEGO SACAR LA MEDIA
    int successful_transfers = 0;       //La transferencia es correcta si coincide en tamanio y en paridad
    
    for (int l=0; l<TRANSFER_NUMBER; l++) {

        //SET TIMESTAMP IN SECONDS
        t0 = clock();

        //ESCRIBO EL ARREGLO AL SOCKET
        bytes_count = 0; ptr = dinamic_buffer;
        while (bytes_count < buffersize) {
            n = write(sockfd, ptr, buffersize);
            if (n < 0)
                error("ERROR reading from socket");
            if (n != 0) {
                bytes_count = bytes_count + n;
                ptr = ptr + n;
            }
        }

        //RECIBO LA RESPUESTA DEL SERVIDOR
        bytes_count = 0; ptr = dinamic_buffer;
        bzero(dinamic_buffer, buffersize);
        while (bytes_count < buffersize) {
            n = read(sockfd, ptr, buffersize);
            if (n < 0)
                error("ERROR reading from socket");
            if (n != 0) {
                bytes_count = bytes_count + n;
                ptr = ptr + n;
            }
        }

        //CHEQUEO DE TAMANIO
        if (bytes_count == buffersize) {
            //CHEQUEO DE PARIDAD
            if (parity(dinamic_buffer, buffersize) == parity_value) {
                successful_transfers++;
                //CALCULATE TIMESTAMP IN SECONDS
                t1 = clock();
                elapsed = ((double) (t1 - t0) / CLOCKS_PER_SEC) / 2.0;
                sum_of_times = sum_of_times + elapsed;
            }
        }
    }

    free(dinamic_buffer); //LIBERO LA MEMORIA, LA TRANSFERENCIA HA FINALIZADO.
    close(sockfd);

    //CALCULO EL PORCENTAJE DE TRANSFERENCIAS EXITOSAS
    sprintf(temp_str, "PORCENTAJE DE TRANSFERENCIAS EXITOSAS = %i%%\n", ((successful_transfers/TRANSFER_NUMBER)*100));
    strcat(registro, temp_str);

    //CALCULO EL TIEMPO PROMEDIO DE LA TRANSFERENCIA
    sprintf(temp_str, "TIEMPO PROMEDIO DE TRANSFERENCIAS = %.15g SECONDS\n\n", (sum_of_times/((double) successful_transfers)));
    strcat(registro, temp_str);
    printf("%s", registro);
    
    return 0;
}