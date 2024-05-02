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
                printf("3. Update The Book Title And Quantity\n");
                printf("4. Search A Book\n");
                printf("5. Search A Client\n");
                printf("6. Exit\n");

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
                    printf("Enter The Quantity Of The Book :\n");
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
                    // Remove Book
                    // copy all books except the given to delete in temp.txt
                    // copy this file to the original file

                    printf("Enter The BookID Of The Book To Be Removed!\n");
                    int bookid;
                    scanf("%d",&bookid);
                    if (send(fsock, &bookid, sizeof(int), 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }
                    char response_remove_book[100];
                    r = recv(fsock,response_remove_book,100,0);
                    if (r < 0 ){
                        perror("recv():");
                        exit(1);
                    }
                    printf("%s\n",response_remove_book);
                }
                else if(option == 3) {
                    // 3 cases
                    // 1. update book name
                    // 2. update quantity
                    // 3. update both - book name and quantity
                    while(1) {
                    printf("Please Select An Option\n");
                    printf("1. Update Book Name\n");
                    printf("2. Update Quantity Book\n");
                    printf("3. Update Book Name and Quantity\n");
                    printf("4. Exit Update\n");
                    int opt;
                    scanf("%d",&opt);
                    if (send(fsock, &opt, sizeof(int), 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }
                    printf("Enter BookID Of The Book :\n");
                    int bookid;
                    scanf("%d",&bookid);
                    if (send(fsock, &bookid, sizeof(int), 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }

                    if(opt == 1) {
                        printf("Enter The New Book Name :\n");
                        char new_book_name[100];
                        scanf("%s",new_book_name);
                        if (send(fsock, new_book_name, sizeof(new_book_name)-1, 0) == -1) {
                            perror("send():");
                            exit(EXIT_FAILURE);
                        }
                        char response_update_only_book_name[100];
                        r = recv(fsock,response_update_only_book_name,100,0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        printf("%s\n",response_update_only_book_name);
                    }
                    else if(opt == 2) {
                        printf("Enter The New Quantity :\n");
                        int new_quantity;
                        scanf("%d",&new_quantity);
                        if (send(fsock, &new_quantity, sizeof(int), 0) == -1) {
                            perror("send():");
                            exit(EXIT_FAILURE);
                        }
                        char response_update_only_book_quantity[100];
                        r = recv(fsock,response_update_only_book_quantity,100,0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        printf("%s\n",response_update_only_book_quantity);
                    }
                    else if(opt == 3){
                        printf("Enter The New Book Name :\n");
                        char new_book_name[100];
                        scanf("%s",new_book_name);
                        printf("Enter The New Quantity :\n");
                        int new_quantity;
                        scanf("%d",&new_quantity);
                        if (send(fsock, new_book_name, sizeof(new_book_name)-1, 0) == -1) {
                            perror("send():");
                            exit(EXIT_FAILURE);
                        }
                        if (send(fsock, &new_quantity, sizeof(int), 0) == -1) {
                            perror("send():");
                            exit(EXIT_FAILURE);
                        }
                        char response_update_both[100];
                        r = recv(fsock,response_update_both,100,0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        printf("%s\n",response_update_both);
                    }
                    else { break;}
                    }
                }
                else if(option == 4) {
                    // Search Book
                    printf("Enter BookID Of The Book :\n");
                    int bookid;char book_name[100];int recv_quantity = 0;
                    scanf("%d",&bookid);
                    if (send(fsock, &bookid, sizeof(int), 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }
                    int flag = 0;
                    r = recv(fsock,&flag,sizeof(int),0);
                    if (r < 0 ){
                        perror("recv():");
                        exit(1);
                    }
                    if(flag == 1) {
                        r = recv(fsock,&bookid,sizeof(int),0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        r = recv(fsock,book_name,sizeof(book_name)-1,0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        book_name[r] = '\0';
                        int temp_;
                        r = recv(fsock,&temp_,sizeof(int),0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        recv_quantity = temp_;

                        printf("Book ID : %d\n",bookid);
                        printf("Book Name : %s\n",book_name);
                        printf("Quantity : %d\n",recv_quantity);
                    }
                    else {
                        char response_book_search[100];
                        r = recv(fsock,response_book_search,sizeof(response_book_search)-1,0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        response_book_search[r] = '\0';
                        printf("%s\n",response_book_search);
                    }
                }
                else if(option == 5) {
                    // Search Client
                    printf("Enter The User Name Of The Client For Book Issued Data :\n");
                    char user[100];
                    scanf("%s",user);
                    if (send(fsock, user, sizeof(user)-1, 0) == -1) {
                            perror("send():");
                            exit(EXIT_FAILURE);
                        }
                    while(1) {
                        char book_name[100];
                        int bookid;
                        r = recv(fsock,&bookid,sizeof(int),0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        if(bookid == -1) {
                            break;
                        }
                        r = recv(fsock,book_name,sizeof(book_name)-1,0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        book_name[r] = '\0';
                        printf("Book ID : %d\tBook Name : %s\n",bookid,book_name);    
                    }
                }
                else {
                    break;
                }
            }
        }
        else if(choice == 2) {
            while(1) {
                // User Mode;
                printf("Please select an option\n");
                printf("1. Show All Books\n");
                printf("2. Withdraw A Book\n");
                printf("3. Deposit Book\n");
                printf("4. Exit\n");
                int option;
                scanf("%d",&option);
                if (send(fsock, &option, sizeof(int), 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }
                if(option == 1) {
                    printf("Books Available :\n");
                    while(1) {
                        char book_name[100];
                        int bookid;
                        r = recv(fsock,&bookid,sizeof(int),0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        if(bookid == -1) {
                            break;
                        }
                        r = recv(fsock,book_name,sizeof(book_name)-1,0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        book_name[r] = '\0';
                        printf("Book ID : %d\tBook Name : %s\n",bookid,book_name);    
                    }
                } 
                else if(option == 2) {
                    // printf("Please Enter Student ID :\n");
                    // int studentid;
                    // scanf("%d",&studentid);
                    if (send(fsock, user_username, sizeof(user_username)-1, 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }
                    printf("Please Enter Book ID :\n");
                    int bookid;
                    scanf("%d",&bookid);
                    if (send(fsock, &bookid, sizeof(int), 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }
                    char response_issue[100];
                    r = recv(fsock,response_issue,sizeof(response_issue)-1,0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        response_issue[r] = '\0';
                        printf("%s\n",response_issue);
                }
                else if(option == 3){
                    if (send(fsock, user_username, sizeof(user_username)-1, 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }
                    printf("Please Enter Book ID :\n");
                    int bookid;
                    scanf("%d",&bookid);
                    if (send(fsock, &bookid, sizeof(int), 0) == -1) {
                        perror("send():");
                        exit(EXIT_FAILURE);
                    }
                    char response_return[100];
                    r = recv(fsock,response_return,sizeof(response_return)-1,0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        response_return[r] = '\0';
                        printf("%s\n",response_return);
                }
                else {
                    break;
                }
            }
        }
    }
    exit(0); 
}