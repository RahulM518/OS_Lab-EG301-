#include "../header/head.h"
// add a admin into the db/users.bin -> same users can exist too
int main()
{
    char buff[100];
    memset(buff,0,sizeof(buff));
    printf("Enter username: => ");
    scanf("%s",buff);
    struct User u;
    strcpy(u.username,buff);
    printf("Enter password: => ");
    memset(buff,0,sizeof(buff));
    scanf("%s",buff);
    strcpy(u.password,buff);
    u.type=0;
    int fd=open("../db/users.bin",O_CREAT|O_RDWR,0666);
    lseek(fd,0,SEEK_END);
    write(fd,&u,sizeof(u));
    close(fd);
}