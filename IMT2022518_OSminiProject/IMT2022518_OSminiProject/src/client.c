#include"../header/head.h"

bool handleAdmin(int sd)
{
    printf("Choose option:\n");
    printf("1. Add a new book.\n");
    printf("2. Delete a book.\n");
    printf("3. Modify a book.\n");
    printf("4. Add a new user.\n");
    printf("5. Show all available books\n");
    printf("6. Show all users\n");
    printf("7. Logout \n");
    int response;
    scanf("%d",&response);
    write(sd,&response,sizeof(int)); //show the response in the server

    if(response==1){ //add a book
        struct Book b; 
        printf("Enter name of the book: ");
        scanf("%s",b.title);
        printf("Enter name of Author: ");
        scanf("%s",b.author);
        printf("Enter number of copies available: ");
        scanf("%d",&b.copies);
        b.id=-1;
        b.valid = true;
        write(sd,&b,sizeof(b));
    }
    else if(response == 2){ //id of a book based deletion
        printf("Enter ID of the book to delete: ");
        int id = -1;
        scanf("%d",&id);
        write(sd,&id,sizeof(int));
    }
    else if(response == 3){ //id of a book based modification
        printf("Enter ID of the book to modify: ");
        int id = -1;
        scanf("%d",&id);
        printf("Enter the new number of copies available: ");
        int nc = -1;
        scanf("%d",&nc);
        write(sd,&id,sizeof(int));
        write(sd,&nc,sizeof(int));
    }
    else if(response == 4){ //add a user
        struct User u;
        printf("Enter username: ");
        scanf("%s",u.username);
        printf("Enter password: ");
        scanf("%s",u.password);
        u.type = 1;
        write(sd,&u,sizeof(struct User));
    }
    else if(response == 5){  //athe books, with author and title (only use of single continuous name, sepreated by _ as a substitute for space)
        int cnt = 0;
        read(sd,&cnt,sizeof(int));
        struct Book temp;
        for(int i=0;i<cnt;i++){
            read(sd,&temp,sizeof(struct Book));
            if(! temp.valid) continue;
            printf("Title: %s\n",temp.title);
            printf("Author: %s\n",temp.author);
            printf("Copies: %d\n",temp.copies);
            printf("Valid: %d\n",temp.valid);
            printf("ID: %d\n",temp.id);
            printf("\n");
        }
    }
    else if(response == 6){ //  user present
        int cnt = 0;
        read(sd,&cnt,sizeof(int));
        printf("%d\n",cnt);
        struct User temp;
        for(int i=0;i<cnt;i++){
            read(sd,&temp,sizeof(struct User));
            printf("Username: %s\n",temp.username);
            printf("Role: %s\n",temp.type ? "User":"Admin");
        }
    }
    else return false;
    return true;
}


bool handleUser(int sd, char * username ){
    printf("Choose option:\n");
    printf("1. Check all available books.\n");
    printf("2. Issue book \n");
    printf("3. Return book.\n");
    printf("4. Check all books issued by %s\n",username);
    printf("5. Logout \n");

    int response;
    scanf("%d",&response);
    write(sd,&response,sizeof(int));

    if(response == 1){
        int cnt = 0;
        read(sd,&cnt,sizeof(int));
        struct Book temp;
        for(int i=0;i<cnt;i++){
            read(sd,&temp,sizeof(struct Book));
            if(!temp.valid || temp.copies == 0) continue;
            printf("Title: %s\n",temp.title);
            printf("Author: %s\n",temp.author);
            printf("Copies: %d\n",temp.copies);
            printf("Valid: %d\n",temp.valid);
            printf("ID: %d\n",temp.id);
            printf("\n");
        }
    }
    else if(response == 2){
        int bookid=-1;
        printf("Enter book id of the book you want to issue: ");
        scanf("%d",&bookid);
        struct issue is;
        is.bookid = bookid;
        strcpy(is.username,username);
        is.valid = true;
        write(sd,&is,sizeof(struct issue));      
        int stat=0;
        read(sd,&stat,sizeof(int));
        if(stat == -1) printf("Book not available\n");
        else printf("Book issued Succesfully\n");  
    }
    else if(response == 3){
        int bookid=-1;
        printf("Enter book id of the book you want to return: ");
        scanf("%d",&bookid);
        struct issue is;
        is.bookid = bookid;
        strcpy(is.username,username);
        is.valid = false;
        write(sd,&is,sizeof(struct issue)); 

        int stat=0;
        read(sd,&stat,sizeof(int));
        if(stat == -1) printf("Book could not be returned\n");
        else printf("Book returned Succesfully\n");  

    }
    else if(response == 4){
        int cnt = 0;
        read(sd,&cnt,sizeof(int));
        struct issue is;
        for(int i=0;i<cnt;i++){
            read(sd,&is,sizeof(struct issue));
            if(is.valid && !strcmp(is.username,username)){
                printf("Username of borrower: %s\n",is.username);
                printf("Id of Book issued: %d\n",is.bookid);
            }
        }
    }
    else return false;
    return true;
}


int main(){

    /* Creates a socket for communication.
       Specifies server details (IP address and port).
       Establishes a connection to the server.*/
    struct sockaddr_in server;
    int sd = socket(AF_INET,SOCK_STREAM,0);

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    int stat = connect(sd,(struct sockaddr*)&server,sizeof(server));
    if(stat<0){
        perror("Connect");
        return 0;
    }
    /* Authentication:
    Reads a welcome message from the server.
    Prompts the user for a username and password.
    Sends the user credentials to the server.
    Receives authentication status from the server.*/
    char buf[BUFSIZ];
    read(sd,buf,BUFSIZ);
    printf("%s\n",buf);

    struct User temp;
    printf("Enter username: ");
    scanf("%s",temp.username);
    printf("Enter Password: ");
    scanf("%s",temp.password);
    temp.type = 0;

    int authstat = 0;

    write(sd,&temp,sizeof(temp));
    read(sd,&authstat,sizeof(int));

    if(authstat == 0){
        printf("\nLogged in Successfully as Admin\n\n");
        bool br = true;
        while(br){
            br = handleAdmin(sd);
        }
    } 
    else if(authstat == 1){
        printf("Logged in Successfully as user %s\n",temp.username);
        printf("Hello %s\n",temp.username);
        bool br = true;
        while(br){
            br = handleUser(sd,temp.username);
        }
    }
    else {
        printf("Authentication Failed\n");
        printf("Try again Later \n ");
        return -1;
    }
}