// Bhavya Kapadia 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server_client.h"

int main(int argc, char * argv[]) {
    printf("Server Side\n");
    // Declaration of socket variables
    int l_sock, conn_sock;
	int r;
	struct sockaddr_in local, remote;
	socklen_t remote_size=sizeof(remote);
    char str[100];
    char suffix[50]="Server Message - Server PID : ";
    
    fillzero(local);
    // Socket Creation (Internet Socket)
    l_sock=socket(AF_INET, SOCK_STREAM , 0);
	if (l_sock < 0 ){
		perror("socket():");
		exit(1);
	}
    // Struct for initializing local address details
    // We would bind these details to the socket
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons( SRV_PORT );

    // Bind Phase
    r = bind(l_sock,(struct sockaddr *)&local, sizeof(local));
	if (r < 0 ){
		perror("bind():");
		exit(1);
	}

    // Socket set to listen phase for an incoming connection request
    // We would set the queue size to 1
    r = listen(l_sock,1);
	if (r < 0 ){
		perror("listen():");
		exit(1);
	}

    while(1) {
        fillzero(remote);
        // waiting for a connection
        // conn_sock would a new full sock
        conn_sock=accept(l_sock,(struct sockaddr *)&remote,&remote_size);
		if (conn_sock < 0 ){
			perror("accept():");
			exit(1);
		}
        // making a string to send to the client
        sprintf(str,"%s %u\n",suffix,getpid());
        // send
        r = send(conn_sock,str,strlen(str),0);
		if (r < 0 ){
			perror("send():");
			exit(1);
		}
        // receive
        r = recv(conn_sock,str,100,0);
		if (r < 0 ){
			perror("recv():");
			exit(1);
		}
        str[r] = '\0';
        printf("Got : %s\n",str);

        close(conn_sock);
    }
    // close(conn_sock);
}