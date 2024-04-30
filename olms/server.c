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

        int temp = 0;
        int choice = 0;
        r = recv(conn_sock,&temp,sizeof(int),0);
		if (r < 0 ){
			perror("recv():");
			exit(1);
		}
        choice = temp;
        printf("Got : %d\n",choice);


        char temp_1[100];
        char temp_2[100];
        char user_name[100];
        char user_password[100];

        // receive
        r = recv(conn_sock,temp_1,sizeof(temp_1)-1,0);
		if (r < 0 ){
			perror("recv():");
			exit(1);
		}
        temp_1[r] = '\0';
        strcpy(user_name,temp_1);
        printf("Username sent from client : %s\n",user_name);
        
        r = recv(conn_sock,temp_2,sizeof(temp_2)-1,0);
		if (r < 0 ){
			perror("recv():");
			exit(1);
		}
        temp_2[r] = '\0';
        strcpy(user_password,temp_2);
        printf("Password sent from client : %s\n",user_password);

        // Now we will open admin.dat if the choice = 1 
        // If choice = 2 ,then open user.dat
        FILE *fd = NULL;
        char response_authentication[100];
        if (choice == 1) {
            fd = fopen("admin.dat", "rb+");
            if (fd == NULL) {
                perror("Error opening admin.dat");
                exit(1);
            }
            char user_check[100];
            char password_check[100];
            fscanf(fd,"%s %s",user_check,password_check);
            if((strcmp(user_check,user_name) == 0) && (strcmp(password_check,user_password) == 0)) {
                printf("Authentication Successful!\n");
                sprintf(response_authentication, "\nAdmin Authentication Successful!\n");
            }
            else {
                printf("Authentication Unsuccessful!\n");
                sprintf(response_authentication,"\nAdmin Authentication Unsuccessful!\n");
            }
        }
        else if (choice == 2) {
            fd = fopen("user.dat", "rb+");
            if (fd == NULL) {
                perror("Error opening user.dat");
                exit(1);
            }
            char user_check[100];char pass_check[100];int flag = 0;
            while(fscanf(fd,"%s %s",user_check,pass_check) != EOF) {
                if((strcmp(user_name,user_check) == 0) && (strcmp(user_password,pass_check) == 0)) {
                    printf("Authentication Successful!\n");flag = 1;
                    sprintf(response_authentication,"\nUser Authentication Successful!\n");
                    break;
                }
            }
            if(flag == 0) {
                printf("Authentication Unsuccessful!\n");
                sprintf(response_authentication,"\nUser Authentication Unsuccessful!\n");
            }
        }
        else {
            perror("Invalid user type (Neither admin nor user)\n");
            exit(1);
        }

        if (fd == NULL) {
            perror("Error opening file!\n");
            exit(1);
        }
        
        r = send(conn_sock,response_authentication,strlen(response_authentication),0);
        if (r < 0 ){
			perror("send():");
			exit(1);
		}


        // close(conn_sock);
    }
    close(conn_sock);
}