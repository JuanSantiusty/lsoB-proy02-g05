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

#include <signal.h>
#include <fcntl.h> 
#include <sys/stat.h>

#include "inetsocket.h" 
//Hilos
int numConectados=0;
cliente_t clientes[BACKLOG];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//Variables para guardar y contar el numero de nombres ingresado
int numNombres=0;
char **nombres=NULL;


int create_inet_server()
{ 
    //Manejador de señales
    signal(SIGTERM, manejar_sigterm);
    int sockfd, connfd ;  /* descriptores de socket escuchando y socket conexion */
    unsigned int len;     /* longitud de la direccion del cliente */
    struct sockaddr_in servaddr, client; 
    
    int  len_rx = 0;          /* longitud de recibido en bytes */
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
	printf("Esperando nueva conexión...\n");
        connfd = accept(sockfd, (struct sockaddr *)&client, &len);
         if (connfd < 0) {
        fprintf(stderr, "[SERVER-error]: conexion no aceptada. %d: %s \n", errno, strerror(errno));
        continue;  // Continuar en lugar de retornar
        }
        //Validacion del limite de usuarios activos 
        if(numConectados<BACKLOG){
          printf("Conectado");
        }else{
          const char *msg = "Servidor ocupado. Intente más tarde\n";
          write(connfd, msg, strlen(msg));
          close(connfd);
          continue;
        }
        
        
                /* leer mensaje del client, copiar este dentro de un buffer,Leer nombre */
                len_rx = read(connfd, buff_rx, sizeof(buff_rx));
                //Comvertir nombre a minusculas
                nombreMinusculas(buff_rx);
                /*Cerrar conexion si el nombre de usuario existe, caso contrario guardar el nombre*/
                if(buscar_nombre(buff_rx)!=-1){
                    const char *mensaje = "Usuario repetido";
                    write(connfd, mensaje, strlen(mensaje));
                    close(connfd);
                    continue;
                }else{
                    agregar_nombre(buff_rx);
                }
                
                //Crear hilo de cliente
		printf("Hola antes de agregar cliente");
                pthread_mutex_lock(&mutex);
                  //Guardar nuevo cliente en el arreglo
                  strcpy(clientes[numConectados].nombre,buff_rx);
                  clientes[numConectados].socket = connfd;
                  clientes[numConectados].address = client;
                  numConectados++;
		pthread_mutex_unlock(&mutex);
		printf("Hola despues de agregar cliente");
		printf("Cliente recibido %d",connfd);
		printf("Cliente guardado %d",clientes[numConectados-1].socket);
                  //Crear Hilo
                  pthread_t hiloCliente;
                  int *sock_ptr = malloc(sizeof(int)); 
                  *sock_ptr = connfd;
                  printf("Hola desde crear un hilo");
                  pthread_create(&hiloCliente, NULL,usuario,sock_ptr);
                  pthread_detach(hiloCliente);
        
    }
    return 1;
}

void* usuario(void* cliente) {
    int clienteSocket = *((int *)cliente);
    free(cliente);
    
    printf("Hilo iniciado para socket %d\n", clienteSocket);
    
    char mensajeFinal[153] = {0};
    char buffer[BUF_SIZE] = {0};
    int len_rx;
    char nombreCliente[50] = {0};
    
    // Obtener nombre del cliente
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < numConectados; i++) {
        if(clientes[i].socket == clienteSocket) {
            strncpy(nombreCliente, clientes[i].nombre, sizeof(nombreCliente)-1);
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    
    printf("Cliente [%s] en socket %d listo para recibir\n", nombreCliente, clienteSocket);
    
    // Configurar timeout para el socket
    struct timeval tv;
    tv.tv_sec = 70;  // 30 segundos de timeout
    tv.tv_usec = 0;
    setsockopt(clienteSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    
    // Bucle de recepción mejorado
    while(1) {
        len_rx = read(clienteSocket, buffer, sizeof(buffer)-1);
        
        if(len_rx > 0) {
            buffer[len_rx] = '\0';
            printf("Mensaje recibido de %s (socket %d): %s\n", nombreCliente, clienteSocket, buffer);
            
            if(strcmp(buffer, "/exit") == 0) {
                printf("Cliente %s solicitó salir\n", nombreCliente);
                break;
            }
            
            // Formatear y enviar mensaje
            snprintf(mensajeFinal, sizeof(mensajeFinal), "[%s]: %s", nombreCliente, buffer);
            enviarMensajeUsuarios(mensajeFinal, clienteSocket);
            
            memset(buffer, 0, sizeof(buffer));
            memset(mensajeFinal, 0, sizeof(mensajeFinal));
        }
        else if(len_rx == 0) {
            printf("Conexión cerrada por el cliente %s (socket %d)\n", nombreCliente, clienteSocket);
            break;
        }
        else {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("Timeout en socket %d\n", clienteSocket);
            } else {
                perror("Error en read");
            }
            break;
        }
    }
    
    // Limpieza final
    eliminarCliente(nombreCliente);
    close(clienteSocket);
    printf("Hilo terminado para cliente %s\n", nombreCliente);
    pthread_exit(NULL);
}

void agregar_nombre(const char *nombre) {
	printf("Hola desde agregar nombres");
        nombres = realloc(nombres, (numNombres + 1) * sizeof(char*));
        nombres[numNombres] = malloc(strlen(nombre) + 1);
        strcpy(nombres[numNombres], nombre);
        numNombres++;
}

int buscar_nombre(const char *nombre){
  /*Si la lista esta vacia retornar -1*/
  if(numConectados==0){
    return -1;
  }
  int i;
  /*Buscar nombre si lo encuentra devuelve su indice de lo contrario revuelve -1*/
  for (i=0;i<numConectados;i++){
    if(strcmp(clientes[i].nombre,nombre)==0){
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

void enviarMensajeUsuarios(const char *mensaje, int remitente) { 
pthread_mutex_lock(&mutex);
printf("Enviando mensaje");   
    int i;
    for(i = 0; i < numConectados; i++) {
        
          ssize_t bytesEnviados=write(clientes[i].socket, mensaje, strlen(mensaje));
          if(bytesEnviados<0) {
            perror("Error al enviar mensaje");
              // Opcional: marcar cliente como desconectado
          }else{
            printf("Mensaje enviado correctamente");
          }
        
    }
    pthread_mutex_unlock(&mutex);
}

void eliminarCliente(const char *nombre){
    int dirCliente=buscar_nombre(nombre);
    pthread_mutex_lock(&mutex);
    memmove(&clientes[dirCliente], &clientes[dirCliente+1], 
       (numConectados - dirCliente - 1) * sizeof(cliente_t));
    numConectados--;
    pthread_mutex_unlock(&mutex);
}

void manejar_sigterm(int sig) {
      int i;
      /* Crear carpeta logs y abrir archivo para guardar historial */
      mkdir("./logs", 0777);
      FILE *log_file = fopen("./logs/logs-servidor.txt", "w");
      if (log_file == NULL) {
          perror("Error abriendo logs-servidor.txt");
      }
    //Guardar los nombres 
     // Primero calcular el tamaño total necesario
    size_t total_length = 0;
    for(i = 0; i < numNombres; i++) {
        total_length += strlen(nombres[i]) + 2; // +2 para la coma y espacio
    }
    
    char* resultado = malloc(total_length);
    if(resultado == NULL) {
        perror("Error al guadar nombres de log");
    }
    
    resultado[0] = '\0'; // Inicializar cadena vacía
    
    // Concatenar todos los nombres
    for(i = 0; i < numNombres; i++) {
        strcat(resultado, nombres[i]);
        if(i < numNombres - 1) {
            strcat(resultado, ", ");
        }
    }
    fprintf(log_file, "[LOG %s]\n", resultado);
    fclose(log_file);
    _exit(EXIT_FAILURE);
}
