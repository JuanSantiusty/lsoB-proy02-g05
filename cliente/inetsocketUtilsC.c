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
#include <pthread.h>  
#include <signal.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include "inetsocket.h"

#define MAX_MESSAGE_SIZE 1024

int sockfd;
FILE *chat_file;
char username[50];

void *recibir_mensajes(void *arg) {
    char buffer[MAX_MESSAGE_SIZE];
    while (1) {
        int bytes = read(sockfd, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) {
            printf("Servidor desconectado o error.\n");
            break;
        }
        buffer[bytes] = '\0';
        printf("%s\n", buffer);
        if (chat_file != NULL) {
            fprintf(chat_file, "%s\n", buffer); // Guardar en chats.txt
            fflush(chat_file);
        }
    }
    pthread_exit(NULL);
}

void *enviar_mensajes(void *arg) {
    char buffer[MAX_MESSAGE_SIZE];
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0'; // Quitar salto de línea

            if (strcmp(buffer, "/exit") == 0) {
                write(sockfd, buffer, strlen(buffer)); // Avisar al servidor
                break;
            }

            write(sockfd, buffer, strlen(buffer));
        }
    }
    pthread_exit(NULL);
}


int create_inet_client() 
{
    int sockfd; 
    struct sockaddr_in servaddr;
    pthread_t hilo_envio, hilo_recepcion;
    char respuesta[100];

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

    /* Asignar IP, PORT */
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS); 
    servaddr.sin_port = htons(PORT);

    /* Para conectar el socket client al socket server */
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) 
    { 
        printf("CLIENT: conexión con el server fallida...\n");  
        return -1;
    } 

    printf("CLIENT: conectado al server..\n");

    /* Ingresar nombre de usuario */
    printf("Ingrese su nombre de usuario: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0'; // quitar salto de línea

    /* Enviar nombre de usuario */
    write(sockfd, username, strlen(username));

    /* Esperar confirmación del servidor */
    int bytes = read(sockfd, respuesta, sizeof(respuesta) - 1);
    if (bytes <= 0) {
        printf("CLIENT: error en la respuesta del servidor.\n");
        close(sockfd);
        return -1;
    }
    respuesta[bytes] = '\0';
    printf("Servidor: %s\n", respuesta);

    if (strcmp(respuesta, "Usuario repetido") == 0) {
        printf("CLIENT: Nombre de usuario en uso. Terminando conexión.\n");
        close(sockfd);
        return -1;
    }

    /* Crear carpeta chats y abrir archivo para guardar historial */
    mkdir("./chats", 0777);
    chat_file = fopen("./chats/chats.txt", "w");
    if (chat_file == NULL) {
        perror("Error abriendo chats.txt");
    }

    /* Crear hilos para envío y recepción de mensajes */
    pthread_create(&hilo_envio, NULL, enviar_mensajes, NULL);
    pthread_create(&hilo_recepcion, NULL, recibir_mensajes, NULL);

    /* Esperar que el hilo de envío finalice (cuando se envíe /exit) */
    pthread_join(hilo_envio, NULL);

    /* Cerrar socket */
    close(sockfd);
    printf("CLIENT: desconectado del servidor.\n");

    /* Cerrar archivo de chats */
    if (chat_file != NULL) {
        fclose(chat_file);
    }

    /* Terminar hilo de recepción */
    pthread_cancel(hilo_recepcion);

    return 0;
}
