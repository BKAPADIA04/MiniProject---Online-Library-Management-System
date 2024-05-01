// Bhavya Kapadia

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server_client.h"
#include "./headers/Book.h"

int main(int argc,char * argv[]) {
    printf("Client Side\n");
    // Declaration of client variables
    int fsock;
	struct sockaddr_in server;
	char str[100];
    char suffix[50]="Client Message - Client PID : ";
    sprintf(str,"%s %u\n",suffix, getpid());
    
    fillzero(server);

    fsock=socket(AF_INET, SOCK_STREAM , 0);
    if (fsock < 0 ){
        perror("socket():");
        exit(1);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(SRV_PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connection to the server
    int r = connect(fsock,(struct sockaddr *) &server, sizeof(server));
    if (r < 0 ){
        perror("connect():");
        exit(1);
    }
    
    // send
    r = send(fsock,str,strlen(str),0);
    if (r < 0 ){
        perror("send():");
        exit(1);
    }
    // receive
    r = recv(fsock,str,100,0);
    if (r < 0 ){
        perror("recv():");
        exit(1);
    }
    str[r]='\0';
    printf("Got : %s\n",str);

    printf("Welcome To The Operating Systems Library Management System\n\n");

    printf("Please select an option:\n");
    printf("1. Admin Mode\n");
    printf("2. User Mode\n");

    int choice;
    scanf("%d",&choice);
    r = send(fsock,&choice,sizeof(int),0);
    // write(fsock,&choice,sizeof(int)); // can use write as well
	if (r < 0 ){
		perror("send():");
		exit(1);
	}

    char user_username[100];
    char user_password[100];

    printf("Please Enter Your UserName:\n");
    scanf("%s",user_username);
    r = send(fsock,user_username,sizeof(user_username)-1,0);
    if (r < 0 ){
		perror("send():");
		exit(1);
	}
    printf("Please Enter Your Password:\n");
    scanf("%s",user_password);

    r = send(fsock,user_password,sizeof(user_password)-1,0);
    if (r < 0 ){
		perror("send():");
		exit(1);
	}
    char response_authentication[100];
    r = recv(fsock,response_authentication,sizeof(response_authentication),0);
    if (r < 0 ){
        perror("send():");
        exit(1);
    }
    printf("%s\n",response_authentication);

    // Checking if the authentication is valid
    if((strcmp(response_authentication,"\nAdmin Authentication Successful!\n") == 0) || (strcmp(response_authentication,"\nUser Authentication Successful!\n") == 0)) {
        if(choice == 1) {
            while(1) {
                // Admin Mode
                printf("Please select an option\n");
                printf("1. Add A Book\n");
                printf("2. Remove A Book\n");
                printf("3. Update The Book Title\n");
                printf("4. Search A Book\n");
                printf("5. Exit\n");

                int option;
                scanf("%d",&option);
                if (send(fsock, &option, sizeof(int), 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }
                if(option == 1) {
                    // Adding the book
                    // bookid,title,quantity
                    // if bookid found,quantity increment
                    // else new book
                    int bookid;int quantity;
                    char book_name[100];
                    printf("Enter The Book ID :\n");
                    scanf("%d",&bookid);
                    printf("Enter The Book Name :\n");
                    scanf("%s",book_name);
                    printf("Enter The Quanity Of The Book :\n");
                    scanf("%d",&quantity);
                    if (send(fsock, &bookid, sizeof(int), 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }
                    if (send(fsock, book_name, sizeof(book_name)-1, 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }
                    if (send(fsock, &quantity, sizeof(int), 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }
                    char response_add_book[100];
                    r = recv(fsock,response_add_book,100,0);
                    if (r < 0 ){
                        perror("recv():");
                        exit(1);
                    }
                    printf("%s\n",response_add_book);
                }
                else if(option == 2) {

                }
                else if(option == 3) {

                }else if(option == 4) {

                }
                else {
                    break;
                }
            }
        }
        else if(choice == 2) {
            while(1) {
                // User Mode
                printf("Please select an option\n");
                printf("1. Search A Book\n");
                printf("2. Withdraw A Book\n");
                printf("3. Deposit Book\n");
                break;
            }
        }

    }

    exit(0); 
}