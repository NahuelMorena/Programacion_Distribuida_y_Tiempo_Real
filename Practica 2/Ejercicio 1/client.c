#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>
//Libreria utilizada para obtener los tiempos de comunicación
#include <time.h>
#include <math.h>

static unsigned long sdbm(unsigned char *str);
double standard_deviation(double *times, double mean, int loop);

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
    //cantidad de comunicaciones
    int loop = 100;

    //Se valida que se pasen todos los parametros requeridos
    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port buffer_size\n", argv[0]);
       exit(0);
    }

    //Asigna a la variable el tamaño que poseera el buffer
    int buffer_size = atoi(argv[3]);
    unsigned char buffer[buffer_size];

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

    //archivo donde se concatenara la información de las trasnferencias
    char file[10000];
    bzero(file,10000);
    char temp_str[2000];

    sprintf(temp_str, "Analisis de tiempos obtenidos en mensajes de %i bytes...\n",buffer_size);
    strcat(file, temp_str);

    //Se completa el campo de memoria que referencia la variable buffer con el caracter 'x'.
    bzero(buffer,buffer_size);
    for (int j=0; j < (buffer_size - 1); j++){
        strcat(buffer,"x");
    } 

    //Se genera el hash del contenido del buffer
    long hash = sdbm(buffer);
    int original_size = strlen(buffer);

    //LOOP
    double sum_of_times = 0.0;
    int successful_transfers = 0;

    clock_t start, end;
    double time_taken = 0.0;
    double times[loop - 1];
    int cont = 0;

    for (int i=0; i<loop; i++){
        start = clock();

        //ENVIA UN MENSAJE AL SOCKET
	    n = write(sockfd,buffer,buffer_size);
        if (n < 0) 
            error("ERROR writing to socket");
        bzero(buffer,buffer_size);
        
        //ESPERA RECIBIR UNA RESPUESTA
        cont = 0;
        do {
            //LEE EL MENSAJE DEL SERVIDOR
            n = read(sockfd, &buffer[cont], buffer_size);
            if (n < 0)
                error("ERROR reading from socket");
            cont += n;
        } while (cont < buffer_size);

        if (hash == sdbm(buffer) && (original_size == cont - 1)){
            //Calculo del promedio
            end = clock();
            successful_transfers++;
            time_taken = ((double) (end - start) / CLOCKS_PER_SEC) / 2.0;
            //sum_of_times = sum_of_times + time_taken;
            sum_of_times += time_taken;
            times[i] = time_taken;
        }
    }

    close(sockfd);
    double mean = (sum_of_times/((double) successful_transfers));
    sprintf(temp_str, "--->Tiempo promedio obtenido = %g segundos<---\n", mean);
    strcat(file, temp_str);
    sprintf(temp_str, "--->Desviación estandar = %g segundos<----\n", standard_deviation(times,mean,loop));
    strcat(file, temp_str);
    sprintf(temp_str, "---------------------------------------------------\n\n");
    strcat(file, temp_str);

    printf("%s",file);
    return 0;
}

static unsigned long sdbm(unsigned char *str){
    unsigned long hash = 0;
    int c;

    while (c = *str++)
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash;
}

double standard_deviation(double *times, double mean, int loop){
    double sum = 0;
    for (int i = 0; i < loop; i++){
        sum += pow((times[i] - mean),2);
    } 
    return sqrt(sum/loop);
} 