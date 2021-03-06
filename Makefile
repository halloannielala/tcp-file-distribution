CFLAGS = -Wall

CC = gcc
SRCS = client.c server.c
LOGS = client-log.txt server-log.txt

all: client server
client: client.o 
	${CC} ${CFLAGS} -o $@ client.o 

server: server.o
	${CC} ${CFLAGS} -lpthread -o $@ server.o

clean:
	rm *.o client server ${LOGS}
