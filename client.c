/*
 * Example of client using TCP protocol.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>

// You may/may not use pthread for the client code. The client is communicating with
// the server most of the time until he recieves a "GET <file>" request from another client.
// You can be creative here and design a code similar to the server to handle multiple connections.
// #define PORT "6000"
char* client_name;
char* LOG_FILE; //"client-log.txt";
#define MAX_RECEIVE_BUFFER_LENGTH 500


void update_list_of_files(FILE *fileListFile, char files[][80], char* file_list_name){
    char line[80];
    fileListFile = fopen(file_list_name, "rt");
    int i = 0;
    while(fgets(line, 80, fileListFile) != NULL){
        sscanf(line, "%s",files[i]);
        printf("HERE\n");
        printf("%s\n", files[i]);
        i++;
    }

}

void get_printable_time(struct timeval *time_, char* tmbuf, size_t size){
    time_t nowtime;
    struct tm *nowtm;
    //char tmbuf[64];
    nowtime = time_->tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmbuf, size, "%Y-%m-%d %H:%M:%S", nowtm);
}

void print_to_log(char* stmt){
    FILE *logFile;
    char* logFileName = LOG_FILE;
    //Get current time of day
    struct timeval currTime;
    gettimeofday(&currTime,NULL);
    char tmbuf[64];
    //Open file and print string plus time of day
    logFile = fopen(logFileName,"a");
    get_printable_time(&currTime,tmbuf,sizeof(tmbuf));
    fprintf(logFile,"%s | %s\n",stmt,tmbuf);
    fclose(logFile);
}

int main(int argc, char *argv[])

{
    int sockfd; //connect to server on sockfd
    int numBytes;
    struct addrinfo hints, *servinfo, *p;
    int status; //Error status
    char receive_buffer[MAX_RECEIVE_BUFFER_LENGTH];
    char * server_ip, *server_port_num, *file_list_name;

    FILE *fileListFile;
    char files[20][80];


    if(argc != 5){
    	printf("usage is ./client <client name> <server ip> <server port#> <list of files>\n");
    	return 0;
    }
    server_ip = argv[2];
    server_port_num = argv[3];
    client_name = argv[1];
    file_list_name  = argv[4];
    LOG_FILE = malloc(sizeof(argv[1]) + sizeof("-client-log.txt"));
    LOG_FILE = strcat(client_name, "-client-log.txt");
//Get the list of files
    update_list_of_files(fileListFile, files, argv[4]);
//Log start time
    print_to_log("Client started at ");

//Set up the address struct
    memset(&hints, 0, sizeof(hints));
    hints.ai_family= AF_UNSPEC; //AF_INET or AF_INET6
    hints.ai_socktype = SOCK_STREAM; //TCP stream sockets

    if((status = getaddrinfo(server_ip, server_port_num, &hints, &servinfo)) != 0){
    	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    	return 2;
    }	
//Loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next){
    	if((sockfd = socket(p->ai_family, p->ai_socktype, 
    		p->ai_protocol)) == -1){
    		perror("Client could not set up socket\n");
    		continue;
    	}

    	if ((status = connect(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        printf("Client connected!\n");
        break;
    }
//If no connection was made, then exit
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

//
    char *msg = client_name;
	int len, bytes_sent;
	
	//Send a message to server with name and files list
	len = strlen(msg);
	if((bytes_sent = send(sockfd, msg, len, 0)) == -1){
		perror("send error");
		return 2;
	}
	printf("Bytes sent: %d\n", bytes_sent);

	int bytes_received;

	//Wait for the welcome response and hashtable
	if((bytes_received = recv(sockfd,receive_buffer,
			MAX_RECEIVE_BUFFER_LENGTH,0)) == -1){
		perror("receive error");
		return 2;
	}
	else{
		printf("Bytes received %d\n%s", 
					bytes_received, receive_buffer);
	}

	//########## SEND MORE THINGS TO TEST THREADS
	
	
	//Send a message to server with name and files list
    int i;
	char msg2[100];
	for(i = 0; i < 20; i++){
		sprintf(msg2,"test %d from %s", i, client_name);
		len = strlen(msg2);
		printf("%s\n", msg2);
		if((bytes_sent = send(sockfd, msg2, len, 0)) == -1){
			perror("send error");
			return 2;
		}
		// printf("Bytes sent: %d\n", bytes_sent);
		if((bytes_received = recv(sockfd,receive_buffer,
			MAX_RECEIVE_BUFFER_LENGTH,0)) == -1){
		perror("receive error");
		return 2;
		}
		else{
			// printf("Bytes received %d\n%s", 
			// 			bytes_received, receive_buffer);
		}
	}
	


    return 0;
}
