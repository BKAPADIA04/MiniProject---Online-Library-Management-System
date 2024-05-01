// Bhavya Kapadia 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "server_client.h"
#include "./headers/Book.h"

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
        int pid = fork();
        if (pid < 0)
        {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
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
        FILE *fd = NULL;int auth_succ = 0;
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
                auth_succ = 1;
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
                    printf("Authentication Successful!\n");flag = 1;auth_succ = 1;
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
        if(auth_succ) {
            // printf("%d\n",choice);
            // Book *b = (Book *)malloc(sizeof(Book));
            if(choice == 1) {
                // Admin
                while(1) {
                    int option;
                    r = recv(conn_sock, &option, sizeof(int), 0);
                    if (r < 0 ){
                        perror("recv():");
                        exit(1);
                    }
                    
                    if(option == 1) {
                        int bookid;char book_name[100];int quantity;
                        char response_add_book[100];
                        int temp;
                        r = recv(conn_sock, &temp, sizeof(int), 0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        bookid = temp;
                        char temp_1[100];
                        r = recv(conn_sock, temp_1, sizeof(temp_1)-1, 0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        temp_1[r] = '\0';
                        int temp_2;
                        strcpy(book_name,temp_1);
                        r = recv(conn_sock, &temp_2, sizeof(int), 0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        quantity = temp_2;
                        // printf("%d %s %d\n",b->bookid,b->book_name,b->quantity);
                        FILE *book_fd = NULL;
                        book_fd = fopen("book.txt", "a+");
                        if (fd == NULL) {
                            perror("Error opening book.txt");
                            exit(1);
                        }
                        // Applying write lock
                        struct flock lock;
                        lock.l_whence = SEEK_SET;
                        lock.l_len = 0;
                        lock.l_start = 0;
                        lock.l_type = F_WRLCK;
                        fcntl(fileno(book_fd), F_SETLKW, &lock);

                        rewind(book_fd);
                        int bookid_check;char book_name_check[100];int quantity_check;
                        int write_flag = 0;
                        while(fscanf(book_fd,"%d %s %d",&bookid_check,book_name_check,&quantity_check) != EOF) {
                            if(bookid == bookid_check) {
                                write_flag = 1;
                                printf("Book With BookID %d already exists!\n",bookid);
                                sprintf(response_add_book,"\nBook With BookID %d already exists!\n",bookid);
                            }
                        }
                        if(write_flag == 0) {
                            fprintf(book_fd,"%d %s %d\n",bookid,book_name,quantity);
                            printf("Book Added Successfully!\n");
                            sprintf(response_add_book,"\nBook Added Successfully!\n");
                        }
                        // printf("%d %s %d\n",bookid,book_name,quantity);

                        lock.l_type = F_UNLCK;
                        fcntl(fileno(book_fd), F_SETLKW, &lock);
                        fclose(book_fd);
                        r = send(conn_sock,response_add_book,strlen(response_add_book),0);
                        if (r < 0 ){
                            perror("send():");
                            exit(1);
                        }
                    }
                    else if(option == 2) {
                        char response_remove_book[100];
                        int bookid;int temp;
                        r = recv(conn_sock, &temp, sizeof(int), 0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        bookid = temp;
                        printf("Received BookID : %d\n",bookid);
                        FILE *book_fd = NULL; FILE *dummy_fd = NULL;
                        book_fd = fopen("book.txt", "a+");
                        dummy_fd = fopen("temp.txt","w+");
                        if (book_fd == NULL) {
                            perror("Error opening book.txt");
                            exit(1);
                        }
                        if (dummy_fd == NULL) {
                            perror("Error opening temp.txt");
                            exit(1);
                        }
                        // Applying write lock
                        struct flock lock;
                        lock.l_whence = SEEK_SET;
                        lock.l_len = 0;
                        lock.l_start = 0;
                        lock.l_type = F_WRLCK;
                        fcntl(fileno(book_fd), F_SETLKW, &lock);

                        rewind(book_fd);
                        int bookid_check;char book_name_check[100];int quantity_check;
                        int write_flag = 0;
                        
                        while(fscanf(book_fd,"%d %s %d",&bookid_check,book_name_check,&quantity_check) != EOF) {
                            if(bookid != bookid_check) {
                                // printf("Hi\n");
                                // printf("%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                fprintf(dummy_fd,"%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                fflush(dummy_fd);
                            }
                            if(bookid == bookid_check) {
                                write_flag = 1;
                            }
                        }
                        rewind(dummy_fd);
                        fclose(book_fd);
                        book_fd = fopen("book.txt","w");
                        while(fscanf(dummy_fd,"%d %s %d",&bookid_check,book_name_check,&quantity_check) != EOF) {
                                // printf("%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                fprintf(book_fd,"%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                fflush(book_fd);
                        }
                        if(write_flag == 1) {
                            printf("Book Removed Successfully!\n");
                            sprintf(response_remove_book,"\nBook Removed Successfully!\n");
                        }
                        else {
                            printf("BookID Does Not Exist!\n");
                            sprintf(response_remove_book,"\nInvalid BookID!\n");
                        }

                        lock.l_type = F_UNLCK;
                        fcntl(fileno(book_fd), F_SETLKW, &lock);
                        fclose(dummy_fd);
                        fclose(book_fd);
                        r = send(conn_sock,response_remove_book,strlen(response_remove_book),0);
                        if (r < 0 ){
                            perror("send():");
                            exit(1);
                        }
                    }
                    else if (option == 3){
                        while(1) {
                        int temp;
                        int opt,bookid;
                        r = recv(conn_sock, &temp, sizeof(int), 0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        opt = temp;
                        r = recv(conn_sock, &temp, sizeof(int), 0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        bookid = temp;
                        printf("Received BookID : %d\n",bookid);
                        if(opt == 1) {
                            char response_update_only_book_name[100];
                            char new_book_name[100];
                            char temp_1[100];
                            r = recv(conn_sock, temp_1, sizeof(temp_1)-1, 0);
                            if (r < 0 ){
                                perror("recv():");
                                exit(1);
                            }
                            temp_1[r] = '\0';
                            strcpy(new_book_name,temp_1);
                            printf("Received New Book Name : %s\n",new_book_name);
                            FILE *book_fd = NULL; FILE *dummy_fd = NULL;
                            book_fd = fopen("book.txt", "a+");
                            dummy_fd = fopen("temp.txt","w");
                            if (book_fd == NULL) {
                                perror("Error opening book.txt");
                                exit(1);
                            }
                            if (dummy_fd == NULL) {
                                perror("Error opening temp.txt");
                                exit(1);
                            }
                            fclose(dummy_fd); // empty the file
                            dummy_fd = fopen("temp.txt","w+");
                            if (dummy_fd == NULL) {
                                perror("Error opening temp.txt");
                                exit(1);
                            }

                            rewind(book_fd);
                            // Applying write lock
                            struct flock lock;
                            lock.l_whence = SEEK_SET;
                            lock.l_len = 0;
                            lock.l_start = 0;
                            lock.l_type = F_WRLCK;
                            fcntl(fileno(book_fd), F_SETLKW, &lock);

                            int bookid_check;char book_name_check[100];int quantity_check;
                            int write_flag = 0;
                        
                            while(fscanf(book_fd,"%d %s %d",&bookid_check,book_name_check,&quantity_check) != EOF) {
                                if(bookid != bookid_check) {
                                    // printf("Hi\n");
                                    // printf("%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                    fprintf(dummy_fd,"%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                    fflush(dummy_fd);
                                }
                                if(bookid == bookid_check) {
                                    write_flag = 1;
                                    fprintf(dummy_fd,"%d %s %d\n",bookid_check,new_book_name,quantity_check);
                                }
                            }
                            fclose(book_fd);
                            book_fd = fopen("book.txt","w");
                            rewind(dummy_fd);
                            while(fscanf(dummy_fd,"%d %s %d",&bookid_check,book_name_check,&quantity_check) != EOF) {
                                    fprintf(book_fd,"%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                    fflush(book_fd);
                                    fflush(dummy_fd);   
                                }
                                rewind(book_fd);
                            if(write_flag == 1) {
                                printf("Book Name Updated Successfully!\n");
                                sprintf(response_update_only_book_name,"\nBook Updated Successfully!\n");
                            }
                            else {
                                printf("BookID Does Not Exist!\n");
                                sprintf(response_update_only_book_name,"\nInvalid BookID!\n");
                            }
                            lock.l_type = F_UNLCK;
                            fcntl(fileno(book_fd), F_SETLKW, &lock);
                            fclose(dummy_fd);
                            fclose(book_fd);
                            r = send(conn_sock,response_update_only_book_name,strlen(response_update_only_book_name),0);
                            if (r < 0 ){
                                perror("send():");
                                exit(1);
                            }
                        }
                        else if(opt == 2) {

                            char response_update_only_book_quantity[100];
                            int new_quantity;
                            r = recv(conn_sock, &new_quantity, sizeof(int), 0);
                            if (r < 0 ){
                                perror("recv():");
                                exit(1);
                            }
                            printf("Received New Quantity : %d\n",new_quantity);
                            FILE *book_fd = NULL; FILE *dummy_fd = NULL;
                            book_fd = fopen("book.txt", "a+");
                            dummy_fd = fopen("temp.txt","w");
                            if (book_fd == NULL) {
                                perror("Error opening book.txt");
                                exit(1);
                            }
                            if (dummy_fd == NULL) {
                                perror("Error opening temp.txt");
                                exit(1);
                            }
                            fclose(dummy_fd); // empty the file
                            dummy_fd = fopen("temp.txt","w+");
                            if (dummy_fd == NULL) {
                                perror("Error opening temp.txt");
                                exit(1);
                            }

                            rewind(book_fd);
                            // Applying write lock
                            struct flock lock;
                            lock.l_whence = SEEK_SET;
                            lock.l_len = 0;
                            lock.l_start = 0;
                            lock.l_type = F_WRLCK;
                            fcntl(fileno(book_fd), F_SETLKW, &lock);

                            int bookid_check;char book_name_check[100];int quantity_check;
                            int write_flag = 0;
                        
                            while(fscanf(book_fd,"%d %s %d",&bookid_check,book_name_check,&quantity_check) != EOF) {
                                if(bookid != bookid_check) {
                                    // printf("Hi\n");
                                    // printf("%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                    fprintf(dummy_fd,"%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                    fflush(dummy_fd);
                                }
                                if(bookid == bookid_check) {
                                    write_flag = 1;
                                    fprintf(dummy_fd,"%d %s %d\n",bookid_check,book_name_check,new_quantity);
                                }
                            }
                            fclose(book_fd);
                            book_fd = fopen("book.txt","w");
                            rewind(dummy_fd);
                            while(fscanf(dummy_fd,"%d %s %d",&bookid_check,book_name_check,&quantity_check) != EOF) {
                                    fprintf(book_fd,"%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                    fflush(book_fd);
                                    fflush(dummy_fd);   
                                }
                                rewind(book_fd);
                            if(write_flag == 1) {
                                printf("Book Quantity Updated Successfully!\n");
                                sprintf(response_update_only_book_quantity,"\nBook Updated Successfully!\n");
                            }
                            else {
                                printf("BookID Does Not Exist!\n");
                                sprintf(response_update_only_book_quantity,"\nInvalid BookID!\n");
                            }
                            lock.l_type = F_UNLCK;
                            fcntl(fileno(book_fd), F_SETLKW, &lock);
                            fclose(dummy_fd);
                            fclose(book_fd);
                            r = send(conn_sock,response_update_only_book_quantity,strlen(response_update_only_book_quantity),0);
                            if (r < 0 ){
                                perror("send():");
                                exit(1);
                            }
                        }
                        else if(opt == 3){
                            char response_update_both[100];
                            char new_book_name[100];
                            char temp_1[100];
                            r = recv(conn_sock, temp_1, sizeof(temp_1)-1, 0);
                            if (r < 0 ){
                                perror("recv():");
                                exit(1);
                            }
                            temp_1[r] = '\0';
                            strcpy(new_book_name,temp_1);
                            printf("Received New Book Name : %s\n",new_book_name);
                            int new_quantity;
                            r = recv(conn_sock, &new_quantity, sizeof(int), 0);
                            if (r < 0 ){
                                perror("recv():");
                                exit(1);
                            }
                            printf("Received New Quantity : %d\n",new_quantity);
                            FILE *book_fd = NULL; FILE *dummy_fd = NULL;
                            book_fd = fopen("book.txt", "a+");
                            dummy_fd = fopen("temp.txt","w");
                            if (book_fd == NULL) {
                                perror("Error opening book.txt");
                                exit(1);
                            }
                            if (dummy_fd == NULL) {
                                perror("Error opening temp.txt");
                                exit(1);
                            }
                            fclose(dummy_fd); // empty the file
                            dummy_fd = fopen("temp.txt","w+");
                            if (dummy_fd == NULL) {
                                perror("Error opening temp.txt");
                                exit(1);
                            }

                            rewind(book_fd);
                            // Applying write lock
                            struct flock lock;
                            lock.l_whence = SEEK_SET;
                            lock.l_len = 0;
                            lock.l_start = 0;
                            lock.l_type = F_WRLCK;
                            fcntl(fileno(book_fd), F_SETLKW, &lock);

                            int bookid_check;char book_name_check[100];int quantity_check;
                            int write_flag = 0;
                        
                            while(fscanf(book_fd,"%d %s %d",&bookid_check,book_name_check,&quantity_check) != EOF) {
                                if(bookid != bookid_check) {
                                    // printf("Hi\n");
                                    // printf("%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                    fprintf(dummy_fd,"%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                    fflush(dummy_fd);
                                }
                                if(bookid == bookid_check) {
                                    fprintf(dummy_fd,"%d %s %d\n",bookid_check,new_book_name,new_quantity);
                                }
                            }
                            fclose(book_fd);
                            book_fd = fopen("book.txt","w");
                            rewind(dummy_fd);
                            while(fscanf(dummy_fd,"%d %s %d",&bookid_check,book_name_check,&quantity_check) != EOF) {
                                    if(bookid == bookid_check) { write_flag = 1;}
                                    fprintf(book_fd,"%d %s %d\n",bookid_check,book_name_check,quantity_check);
                                    fflush(book_fd);
                                    fflush(dummy_fd);   
                                }
                                rewind(book_fd);
                            if(write_flag == 1) {
                                printf("Book Name And Quantity Updated Successfully!\n");
                                sprintf(response_update_both,"\nBook Updated Successfully!\n");
                            }
                            else {
                                printf("BookID Does Not Exist!\n");
                                sprintf(response_update_both,"\nInvalid BookID!\n");
                            }
                            lock.l_type = F_UNLCK;
                            fcntl(fileno(book_fd), F_SETLKW, &lock);
                            fclose(dummy_fd);
                            fclose(book_fd);
                            r = send(conn_sock,response_update_both,strlen(response_update_both),0);
                            if (r < 0 ){
                                perror("send():");
                                exit(1);
                            }
                        }
                        else { break;}
                        }
                    }
                    else if(option == 4){
                        // Search
                        int bookid;
                        char book_name[100];
                        int quantity;
                        r = recv(conn_sock, &bookid, sizeof(int), 0);
                        if (r < 0 ){
                            perror("recv():");
                            exit(1);
                        }
                        FILE * book_fd = NULL;
                        book_fd = fopen("book.txt","r+");
                        if (book_fd == NULL) {
                            perror("Error opening book.txt");
                            exit(1);
                        }
                        int bookid_check;char book_name_check[100];int quantity_check;
                        int flag = 0;
                        while(fscanf(book_fd,"%d %s %d",&bookid_check,book_name_check,&quantity_check) != EOF) {
                            if(bookid == bookid_check) {
                                strcpy(book_name,book_name_check);
                                bookid = bookid_check;
                                quantity = quantity_check;
                                flag = 1;
                                break;
                            }
                        }
                        if (send(conn_sock, &flag, sizeof(int), 0) == -1) {
                                perror("send():");
                                exit(EXIT_FAILURE);
                            }
                        if(flag == 1) {
                            printf("Book Search Successful!\n");
                            if (send(conn_sock, &bookid, sizeof(int), 0) == -1) {
                                perror("send():");
                                exit(EXIT_FAILURE);
                            }
                            if (send(conn_sock, book_name, sizeof(book_name)-1, 0) == -1) {
                                perror("send():");
                                exit(EXIT_FAILURE);
                            }
                            if (send(conn_sock, &quantity, sizeof(int), 0) == -1) {
                                perror("send():");
                                exit(EXIT_FAILURE);
                                }
                            }
                            else {
                                printf("Book Search Unsuccessful!\n");
                                char response_book_search[100];
                                sprintf(response_book_search,"\nInvalid BookID!\n");
                                if (send(conn_sock, response_book_search, sizeof(response_book_search)-1, 0) == -1) {
                                perror("send():");
                                exit(EXIT_FAILURE);
                                }
                            }
                    }
                    else if(option == 5) {
                        // Search client
                    }
                    else { break;}
                    }
                }
            else {
                // User
            }
        }
    }
    else {    // close(conn_sock);
    close(conn_sock);
    }
    }
}