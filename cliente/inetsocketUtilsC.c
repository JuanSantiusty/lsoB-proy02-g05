/*************************************************************************************/
/* @file    inetsocketUtilsC.c                                                               */
/* @brief   Este cliente se conecta,                                                   */
/*          envia un texto y se desconecta                                     */
/*************************************************************************************/

#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include "inetsocket.h"

/* Mensaje de prueba */
char buf_tx[] = "Hola server. Saludo desde un client";      
char buf_rx[100];

/* Este cliente se conecta, envia un texto y se desconecta */
int create_inet_client() 
{ 
    int sockfd; 
    struct sockaddr_in servaddr; 
    
    /* Creación del Socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) 
    { 
        printf("CLIENT: creacion del socket fallida...\n"); 
        return -1;  
    } 
    else
    {
        printf("CLIENT: Socket creado exitosamente..\n"); 
    }
    
    
    memset(&servaddr, 0, sizeof(servaddr));

    /* Asinar IP, PORT */
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr( SERVER_ADDRESS ); 
    servaddr.sin_port = htons(PORT); 

    /* Para conectar el socket client a el socket server */
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) 
    { 
        printf("conexión con el server fallida...\n");  
        return -1;
    } 

    printf("conectado a el server..\n"); 

    /* enviar texto*/
    write(sockfd, buf_tx, sizeof(buf_tx));     
    read(sockfd, buf_rx, sizeof(buf_rx));
    printf("CLIENT:Recibido: %s \n", buf_rx);

    /* cerrar socket */
    close(sockfd);
    return 0;
}
