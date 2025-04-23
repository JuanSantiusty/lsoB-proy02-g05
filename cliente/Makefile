##VARIABLES
CC = gcc
CFLAGS = -Wall -g
TARGET_CLIENT = inet_client
OBJS = inetsocketUtilsC.o
##Directiva principal
all: $(TARGET_SERVER) $(TARGET_CLIENT)
##Actividades

$(TARGET_CLIENT): principalClient.o $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT) principalClient.o $(OBJS)

principalClient.o: principalClient.c inetsocket.h
	$(CC) $(CFLAGS) -c principalClient.c

inetsocketUtilsC.o: inetsocketUtilsC.c inetsocket.h
	$(CC) $(CFLAGS) -c inetsocketUtilsC.c
##Directiva phony
clean:
	rm -f $(TARGET_SERVER) $(TARGET_CLIENT) *.o /tmp/inet_socket
