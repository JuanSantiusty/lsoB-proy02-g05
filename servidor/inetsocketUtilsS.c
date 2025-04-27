/***************************************************************************************/
/* @file    inetsocketUtilsS.c                                                        */
/* @brief   Servidor secuencial. Sockets TCP                                             */
/***************************************************************************************/

/*simbolos estandar */
#include <unistd.h>  

/* sockets */
#include <netdb.h> 
#include <netinet/in.h>  
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>

/* strings / errors*/
#include <errno.h>
#include <stdio.h> 
#include <string.h>
#include <ctype.h>

/* hilos */
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stddef.h>

#include "inetsocket.h" 

int numConectados=0;
int numNombres=0;
char **nombres=NULL;


int create_inet_server()
{ 
    int sockfd, connfd ;  /* descriptores de socket escuchando y socket conexion */
    unsigned int len;     /* longitud de la direccion del cliente */
    struct sockaddr_in servaddr, client; 
    
    int  len_rx = 0;          /* longitud de recibido en bytes */
    char buff_tx[BUF_SIZE] = "Hola client, mensaje desde el server";
    char buff_rx[BUF_SIZE];   /* buffers para recepcion  */
    
    
    
     
    /* creacion de socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) 
    { 
        fprintf(stderr, "[SERVER-error]: creacion de socket fallida. %d: %s \n", errno, strerror( errno ));
        return -1;
    } 
    else
    {
        printf("[SERVER]: Socket creado exitosamente..\n"); 
    }
    
    /* limpiar estructura */
    memset(&servaddr, 0, sizeof(servaddr));
  
    /* asignar IP, SERV_PORT, IPV4 */
    servaddr.sin_family      = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR); 
    servaddr.sin_port        = htons(SERV_PORT); 
    
    
    /* Bind socket */
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) 
    { 
        fprintf(stderr, "[SERVER-error]: bind fallido. %d: %s \n", errno, strerror( errno ));
        return -1;
    } 
    else
    {
        printf("[SERVER]: bind exitoso \n");
    }
  
    /* Listen */
    if ((listen(sockfd, BACKLOG)) != 0) 
    { 
        fprintf(stderr, "[SERVER-error]: listen falido. %d: %s \n", errno, strerror( errno ));
        return -1;
    } 
    else
    {
        printf("[SERVER]: Socket escuchando en SERV_PORT %d \n\n", ntohs(servaddr.sin_port) ); 
    }

    len = sizeof(client); 

      /* Acceptando datos desde sockets iterativamente */
      while(1)
      {
        //Validacion del limite de usuarios activos 
        if(numConectados<BACKLOG){
          connfd = accept(sockfd, (struct sockaddr *)&client, &len);
        }else{
          connfd = accept(sockfd, (struct sockaddr *)&client, &len);
          const char *msg = "Servidor ocupado. Intente más tarde\n";
          write(connfd, msg, strlen(msg));
          close(connfd);
        }
        
        
        if (connfd < 0) 
        { 
            fprintf(stderr, "[SERVER-error]: conexion no aceptada. %d: %s \n", errno, strerror( errno ));
            return -1;
        }
        else
        {
            while(1) /* leer datos desde un client socket hasta que el socket sea cerrado */ 
            {
                /* leer mensaje del client, copiar este dentro de un buffer,Leer nombre */
                len_rx = read(connfd, buff_rx, sizeof(buff_rx));
                //Comvertir nombre a minusculas
                nombreMinusculas(buff_rx);
                /*Cerrar conexion si el nombre de usuario existe, caso contrario guardar el nombre*/
                if(buscar_nombre(buff_rx)!=-1){
                    const char *mensaje = "Nombre ocupado, Intente otro nombre\n";
                    write(connfd, mensaje, strlen(mensaje));
                    close(connfd);
                    break;
                }else{
                    agregar_nombre(buff_rx);
                    numConectados++;
                }

                if(len_rx == -1)
                {
                    fprintf(stderr, "[SERVER-error]: connfd no puede ser leido. %d: %s \n", errno, strerror( errno ));
                }
                else if(len_rx == 0) /* si len_rx es 0 client socket cerrado, entonces salir */
                {
                    printf("[SERVER]: client socket cerrado \n\n");
                    close(connfd);
                    break;
                }
                else
                {
                    write(connfd, buff_tx, strlen(buff_tx));
                    printf("[SERVER]: %s \n", buff_rx);
                }
	    }
        }
    }
}

void* usuario(void* nomUsuario){
  
}

void agregar_nombre(const char *nombre) {
        nombres = realloc(nombres, (numNombres + 1) * sizeof(char*));
        nombres[numNombres] = malloc(strlen(nombre) + 1);
        strcpy(nombres[numNombres], nombre);
        numNombres++;
}

int buscar_nombre(const char *nombre){
  /*Si la lista esta vacia retornar -1*/
  if(nombres==NULL){
    return -1;
  }
  int i;
  /*Buscar nombre si lo encuentra devuelve su indice de lo contrario revuelve -1*/
  for (i=0;i<numNombres;i++){
    if(strcmp(nombres[i],nombre)==0){
      return i;
    }
  }
  return -1;
}

void nombreMinusculas(char *nom){
    while(*nom) {  
        *nom = tolower(*nom);  
        nom++;  
    }
}
