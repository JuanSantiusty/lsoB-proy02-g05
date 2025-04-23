##VARIABLES
CC = gcc
CFLAGS = -Wall -g -pthread
TARGET_SERVER = inet_server
OBJS = inetsocketUtilsS.o
##Directiva principal
all: $(TARGET_SERVER)
##Actividades
$(TARGET_SERVER): principalServer.o $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) principalServer.o $(OBJS)

principalServer.o: principalServer.c inetsocket.h
	$(CC) $(CFLAGS) -c principalServer.c

inetsocketUtilsS.o: inetsocketUtilsS.c inetsocket.h
	$(CC) $(CFLAGS) -c inetsocketUtilsS.c
##Directiva phony
clean:
	rm -f $(TARGET_SERVER) $(TARGET_CLIENT) *.o /tmp/inet_socket
