#ifndef INET_SOCKET_H
#define INET_SOCKET_H

#define SERVER_ADDRESS  "192.168.1.10"     /* server IP */
#define PORT            8080

int create_inet_client();
void *enviar_mensajes(void *arg);
void *recibir_mensajes(void *arg);

#endif
