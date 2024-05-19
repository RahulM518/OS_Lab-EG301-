#include "../header/server.h"
#include "../header/head.h"

sem_t books_semaphore;
sem_t issues_semaphore;
sem_t users_semaphore;

void *handleThread(void *args){

    int nsd=*((int *)args);
    write(nsd,"Connected to server\n",sizeof("Connected to server\n"));

    struct User temp;
    read(nsd,&temp,sizeof(struct User));

    printf("Username: %s\n",temp.username);
    printf("Password: %s\n",temp.password);

    struct flock lock;
    lock.l_type=F_RDLCK;
    lock.l_whence=SEEK_SET;
    lock.l_start=0;
    lock.l_len=0;
    lock.l_pid=getpid();

    int fd=open("../db/users.bin",O_RDONLY,0666);
    fcntl(fd,F_SETLKW,&lock);

    struct User u;
    int flag=0;
    while(read(fd,&u,sizeof(u))){
        if(strcmp(u.username,temp.username)==0){
            flag=1;
            break;
        }
    }

    lock.l_type=F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);

    int ok;
    if(strcmp(u.password,temp.password) || !flag) ok=-1;
    else ok=u.type;

    write(nsd,&ok,sizeof(int));
    
    if(ok==0) admin_mode(nsd);
    else if(ok==1) user_mode(nsd);

    return NULL;
}

int main(){

    sem_init(&books_semaphore, 0, 1);
    sem_init(&issues_semaphore, 0, 1);
    sem_init(&users_semaphore, 0, 1);

    int sd,nsd;
    socklen_t len;
    struct sockaddr_in serv,cli;
    sd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&serv, sizeof(serv)); 
    serv.sin_family=AF_INET;
    serv.sin_addr.s_addr=htonl(INADDR_ANY);
    serv.sin_port=htons(PORT);

    bind(sd,(struct sockaddr*)&serv,sizeof(serv));

    listen(sd,50);

    printf("Server listening.. on port %d\n",PORT);
    
    while(1){
        len=sizeof(cli);
        nsd=accept(sd,(struct sockaddr*)&cli,&len);
        pthread_t tid;
        pthread_create(&tid,NULL,handleThread,(void*)&nsd); 
    }

    sem_destroy(&books_semaphore);
    sem_destroy(&issues_semaphore);
    sem_destroy(&users_semaphore);

    return 0;
}