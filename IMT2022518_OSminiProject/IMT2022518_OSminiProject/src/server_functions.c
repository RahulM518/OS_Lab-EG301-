#include "../header/server.h"
#include "../header/head.h"

int get_id(){
    int fd=open("../db/books.bin",O_RDONLY,0666);
    int offset=lseek(fd,0,SEEK_END);

    if(offset <= 0) return 1;
    else return(offset/sizeof(struct Book) + 1);
}

void get_all_books(){
    struct Book temp;
    int fd=open("../db/books.bin",O_RDONLY,0666);
    read(fd,&temp,sizeof(struct Book));
    printf("Title: %s\n",temp.title);
    printf("Author: %s\n",temp.author);
    printf("Copies: %d\n",temp.copies);
    printf("Valid: %d\n",temp.valid);
    printf("ID: %d\n",temp.id);
    printf("\n");
    while(read(fd,&temp,sizeof(struct Book)))
    {
        printf("Title: %s\n",temp.title);
        printf("Author: %s\n",temp.author);
        printf("Copies: %d\n",temp.copies);
        printf("Valid: %d\n",temp.valid);
        printf("ID: %d\n",temp.id);
        printf("\n");
    }
}

void add_book(int nsd){
    sem_wait(&books_semaphore);

    struct Book temp;
    bzero(&temp,sizeof(struct Book));
    read(nsd,&temp,sizeof(struct Book));
    temp.id = get_id();

    int fd=open("../db/books.bin",O_WRONLY|O_CREAT,0666);
    
    lseek(fd,0,SEEK_END);
    write(fd,&temp,sizeof(struct Book));
    close(fd);

    get_all_books();

    sem_post(&books_semaphore);
}

void send_books(int nsd){
    sem_wait(&books_semaphore);

    int fd=open("../db/books.bin",O_RDONLY,0666);
    int offset=lseek(fd,0,SEEK_END);

    int cnt = -1;
    if(offset <= 0) cnt = 0;
    else cnt = (offset/sizeof(struct Book));
    printf("%d\n",cnt);
    write(nsd,&cnt,sizeof(int));

    struct Book temp;
    for(int i=0;i<cnt;i++){
        lseek(fd,i*sizeof(struct Book),SEEK_SET);
        read(fd,&temp,sizeof(struct Book));
        write(nsd,&temp,sizeof(struct Book));
    }
    close(fd);

    sem_post(&books_semaphore);
}

void delete_book(int nsd){
    sem_wait(&books_semaphore);

    int torid = -1;
    read(nsd,&torid,sizeof(int));

    struct Book temp;
    int fd=open("../db/books.bin",O_WRONLY,0666);


    lseek(fd,(torid-1)*sizeof(struct Book),SEEK_SET);
    read(fd,&temp,sizeof(struct Book));
    temp.valid = 0;
    strcpy(temp.title,"");
    strcpy(temp.author,"");
    temp.id = -1;
    temp.copies = -1;
    lseek(fd,(torid-1)*sizeof(struct Book),SEEK_SET);
    write(fd,&temp,sizeof(struct Book));
    
    get_all_books();

    sem_post(&books_semaphore);
}

void modify_book(int nsd){
    sem_wait(&books_semaphore);

    int cnid = -1, cncp = -1;
    read(nsd,&cnid,sizeof(int));
    read(nsd,&cncp,sizeof(int));

    struct Book temp;
    int fd=open("../db/books.bin",O_RDWR,0666);

    lseek(fd,(cnid-1)*sizeof(struct Book),SEEK_SET);
    read(fd,&temp,sizeof(struct Book));

    temp.copies = cncp;

    lseek(fd,(cnid-1)*sizeof(struct Book),SEEK_SET);
    write(fd,&temp,sizeof(struct Book));
    
    get_all_books();
    sem_post(&books_semaphore);
}

void add_user(int nsd){
    sem_wait(&users_semaphore);
    struct User u;
    read(nsd,&u,sizeof(struct User));

    int fd=open("../db/users.bin",O_CREAT|O_RDWR,0666);
    lseek(fd,0,SEEK_END);
    write(fd,&u,sizeof(u));
    close(fd);
    sem_post(&users_semaphore);
}

void send_users(int nsd){
    // printf("before\n");
    sem_wait(&users_semaphore);
    // printf("enter\n");
    int fd = open("../db/users.bin",O_RDWR,0666);
    int cnt = lseek(fd,0,SEEK_END)/sizeof(struct User);

    write(nsd,&cnt,sizeof(int));
    // printf("%d\n",cnt);
    struct User temp;

    for(int i=0;i<cnt;i++){
        lseek(fd,i*sizeof(struct User),SEEK_SET);
        read(fd,&temp,sizeof(struct User));
        write(nsd,&temp,sizeof(struct User));
    }
    close(fd);
    sem_post(&users_semaphore);
}

void get_all_issues(){

    struct issue temp;
    int fd=open("../db/issues.bin",O_RDONLY,0666);
    read(fd,&temp,sizeof(struct issue));
    printf("bookid: %d\n",temp.bookid);
    printf("username: %s\n",temp.username);
    printf("valid: %d\n",temp.valid);
    printf("\n");
    while(read(fd,&temp,sizeof(struct issue)))
    {
        printf("bookid: %d\n",temp.bookid);
        printf("username: %s\n",temp.username);
        printf("valid: %d\n",temp.valid);
        printf("\n");
    }
}

void issue_book(int nsd){
    sem_wait(&books_semaphore);
    sem_wait(&issues_semaphore);

    struct issue temp;
    read(nsd,&temp,sizeof(struct issue));

    struct Book newbook;
    int fd=open("../db/books.bin",O_RDWR,0666);

    lseek(fd,(temp.bookid-1)*sizeof(struct Book),SEEK_SET);
    read(fd,&newbook,sizeof(struct Book));
    if(newbook.copies<=0 || newbook.valid == 0){
        int status = -1;
        write(nsd,&status,sizeof(int));
        sem_post(&books_semaphore);
        sem_post(&issues_semaphore);
        return;
    }

    newbook.copies = newbook.copies -1;

    lseek(fd,(temp.bookid-1)*sizeof(struct Book),SEEK_SET);
    write(fd,&newbook,sizeof(struct Book));

    close(fd);


    fd=open("../db/issues.bin",O_WRONLY|O_CREAT,0666);

    lseek(fd,0,SEEK_END);
    write(fd,&temp,sizeof(struct issue));
    close(fd);

    int status = 1;
    write(nsd,&status,sizeof(int));  

    get_all_issues();

    sem_post(&issues_semaphore);
    sem_post(&books_semaphore);

}

void return_book(int nsd){

    sem_wait(&books_semaphore);
    sem_wait(&issues_semaphore);

    struct issue temp;
    read(nsd,&temp,sizeof(struct issue));
    temp.valid = false;
    bool flag = false;

    struct issue tempissue;
    int fd=open("../db/issues.bin",O_RDWR,0666);
    lseek(fd,0,SEEK_SET);
    while(read(fd,&tempissue,sizeof(struct issue))>0){
        printf("%d",tempissue.bookid);
        printf(" %s\n",tempissue.username);
        if(tempissue.bookid == temp.bookid && !strcmp(temp.username,tempissue.username) && tempissue.valid!=0){
            flag = true;
            lseek(fd,-sizeof(struct issue),SEEK_CUR);
            write(fd,&temp,sizeof(struct issue));
            break;
        }
    }

    if(!flag){
        int status = -1;
        write(nsd,&status,sizeof(int));
        sem_post(&books_semaphore);
        sem_post(&issues_semaphore);
        return;
    }

    struct Book newbook;
    fd=open("../db/books.bin",O_RDWR,0666);
    lseek(fd,(temp.bookid-1)*sizeof(struct Book),SEEK_SET);
    read(fd,&newbook,sizeof(struct Book));
    if(newbook.valid == 0){
        int status = -1;
        write(nsd,&status,sizeof(int));
        sem_post(&books_semaphore);
        sem_post(&issues_semaphore);
        return;
    }
    newbook.copies = newbook.copies + 1;
    lseek(fd,(temp.bookid-1)*sizeof(struct Book),SEEK_SET);
    write(fd,&newbook,sizeof(struct Book));
    close(fd);

    int status = 1;
    write(nsd,&status,sizeof(int));

    sem_post(&issues_semaphore);
    sem_post(&books_semaphore);
}

void send_issues(int nsd){
    sem_wait(&issues_semaphore);

    struct issue temp;
    int fd=open("../db/issues.bin",O_RDWR,0666);
    int cnt = lseek(fd,0,SEEK_END)/sizeof(struct issue);
    write(nsd,&cnt,sizeof(int));
    
    for(int i=0;i<cnt;i++){
        lseek(fd,i*sizeof(struct issue),SEEK_SET);
        read(fd,&temp,sizeof(struct issue));

        write(nsd,&temp,sizeof(struct issue));
    }
    close(fd);

    sem_post(&issues_semaphore);
}


void admin_mode(int nsd){
    while(1){

        int choice;
        read(nsd,&choice,sizeof(choice));
        printf("Choice: %d\n",choice);

        if(choice==1){
            add_book(nsd);
        }
        else if(choice==2){
            delete_book(nsd);
        }
        else if(choice==3){
            modify_book(nsd);
        }
        else if(choice==4){
            add_user(nsd);
        }
        else if(choice==5){
            send_books(nsd);
        }
        else if(choice == 6){
            send_users(nsd);
        }
        else return;
    }
}

void user_mode(int nsd){
    while(1){

        int choice;
        read(nsd,&choice,sizeof(choice));
        printf("Choice: %d\n",choice);

        if(choice==1){
            send_books(nsd);
        }
        else if(choice==2){
            issue_book(nsd);
        }
        else if(choice == 3){
            return_book(nsd);
        }
        else if(choice == 4){
            send_issues(nsd);
        }
        else return;
    }
}