# include <stdio.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
#include  <string.h>
#include  <stdlib.h>

int main(int argc,char *argv[]){
    FILE *fd1;
    fd1 = fopen("user.dat","wb+");

    struct flock lock;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_start = 0;
    lock.l_type = F_WRLCK;

    fcntl(fileno(fd1), F_SETLKW, &lock);
    
    for(int i=0;i<20;i++) 
    {
        char client_name[100];
        char client_password[100];
        char str[10];
        sprintf(str, "%d", i+1);
        strcpy(client_name,"client");
        strcat(client_name,str);
        strcpy(client_password,"pass");
        strcat(client_password,str);
        // fprintf(fd1,"%s %s\n",client_name,client_password);
        fwrite(client_name, sizeof(char), strlen(client_name), fd1);
        fwrite(" ", sizeof(char), 1, fd1);  // Separate client_name and client_password
        fwrite(client_password, sizeof(char), strlen(client_password), fd1);
        fwrite("\n", sizeof(char), 1, fd1);  // Newline character
    }

    lock.l_type = F_UNLCK;
    fcntl(fileno(fd1), F_SETLKW, &lock);

    fclose(fd1);

    FILE *fd2;
    fd2 = fopen("admin.dat","wb+");

    struct flock lock_;
    lock_.l_whence = SEEK_SET;
    lock_.l_len = 0;
    lock_.l_start = 0;
    lock_.l_type = F_WRLCK;

    fcntl(fileno(fd2), F_SETLKW, &lock_);
    
    char admin_name[100];
    char admin_password[100];
    strcpy(admin_name,"admin");
    strcpy(admin_password,"admin45");
    // fprintf(fd1,"%s %s\n",client_name,client_password);
    fwrite(admin_name, sizeof(char), strlen(admin_name), fd1);
    fwrite(" ", sizeof(char), 1, fd1);  // Separate client_name and client_password
    fwrite(admin_password, sizeof(char), strlen(admin_password), fd1);
    fwrite("\n", sizeof(char), 1, fd1);  // Newline character

    lock.l_type = F_UNLCK;
    fcntl(fileno(fd2), F_SETLKW, &lock_);

    fclose(fd2);

    return 0;   
}