#ifndef UNIX_SOCKET_H
#define UNIX_SOCKET_H

#include <stddef.h>
#include <netdb.h> 
#include <netinet/in.h>  
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>


/* parametros del servidor */
#define SERV_PORT       8080              /* puerto */
#define SERV_HOST_ADDR "192.168.1.10"     /* IP, solo soporta IPV4  */
#define BUF_SIZE        100               /* Tamaño max de Buffer rx, tx max  */
#define BACKLOG         5                 /* Max. conexiones cliente pendientes  */

typedef struct {
  char nombre[50];
  int socket;
  struct sockaddr_in address;
}cliente_t;


int create_inet_server();

void* usuario(void* nomUsuario);

/*Funcion para agregar un nombre en la lista*/
void agregar_nombre(const char *nombre);

/*Funcion para buscar un nombre en la lista*/
int buscar_nombre(const char *nombre);

/*Funcion para convertir un nombre en minisculas*/
void nombreMinusculas(char *nom);

//Funcion para enviar los mensajes a los diferentes usuarios
void enviarMensajeUsuarios(const char *mensaje, int remitente);

//Funcion para eliminar un cliente
void eliminarCliente(const char *nombre);

#endif
