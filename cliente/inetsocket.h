#ifndef INET_SOCKET_H
#define INET_SOCKET_H

#define SERVER_ADDRESS  "192.168.1.10"     /* IP del servidor */
#define PORT            8080               /* Puerto del servidor */

#include <stdio.h>  /* Para FILE* */
#include <pthread.h> /* Para pthread_t */

extern int sockfd;
extern FILE *chat_file;
extern char username[50]; 

int create_inet_client(); 
void *enviar_mensajes(void *arg);
void *recibir_mensajes(void *arg);

#endif
