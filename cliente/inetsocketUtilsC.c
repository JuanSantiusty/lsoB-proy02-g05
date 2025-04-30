/*************************************************************************************/
/* @file    inetsocketUtilsC.c                                                               */
/* @brief   Este cliente se conecta,                                                   */
/*          envia un texto y se desconecta                                     */
/*************************************************************************************/

#include "inetsocket.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_MSG 1024

int sockfd;
char login[50];
FILE *log_file;

void *enviar_mensajes(void *arg);
void *recibir_mensajes(void *arg);

int create_inet_client()
{
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("CLIENT: Creación del socket fallida...\n");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("CLIENT: Conexión con el servidor fallida...\n");
        return -1;
    }

    printf("CLIENT: Conectado al servidor...\n");

    printf("Ingrese su nombre de usuario: ");
    fgets(login, sizeof(login), stdin);
    login[strcspn(login, "\n")] = 0;

    write(sockfd, login, strlen(login));

    char response[100];
    int len = read(sockfd, response, sizeof(response) - 1);
    response[len] = '\0';

    if (strcmp(response, "ERROR") == 0)
    {
        printf("CLIENT: Nombre de usuario en uso. Intenta con otro.\n");
        close(sockfd);
        return -1;
    }

    printf("CLIENT: Usuario aceptado. Bienvenido al chat, %s.\n", login);

    system("mkdir -p ./chats");
    log_file = fopen("./chats/chats.txt", "w");

    pthread_t hilo_envio, hilo_recepcion;
    pthread_create(&hilo_envio, NULL, enviar_mensajes, NULL);
    pthread_create(&hilo_recepcion, NULL, recibir_mensajes, NULL);

    pthread_join(hilo_envio, NULL);

    fclose(log_file);
    close(sockfd);
    return 0;
}

void *enviar_mensajes(void *arg)
{
    char mensaje[MAX_MSG];

    while (1)
    {
        if (fgets(mensaje, sizeof(mensaje), stdin) == NULL)
            break;

        mensaje[strcspn(mensaje, "\n")] = 0;

        if (strcmp(mensaje, "/exit") == 0)
        {
            write(sockfd, mensaje, strlen(mensaje));
            printf("CLIENT: Saliendo del chat...\n");
            break;
        }

        write(sockfd, mensaje, strlen(mensaje));
    }

    return NULL;
}

void *recibir_mensajes(void *arg)
{
    char mensaje[MAX_MSG];

    while (1)
    {
        int len = read(sockfd, mensaje, sizeof(mensaje) - 1);
        if (len <= 0)
        {
            printf("CLIENT: Conexión cerrada por el servidor o error.\n");
            break;
        }

        mensaje[len] = '\0';

        printf("%s\n", mensaje);
        fprintf(log_file, "%s\n", mensaje);
        fflush(log_file);
    }

    return NULL;
}
