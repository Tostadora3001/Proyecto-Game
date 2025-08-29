#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<signal.h>

#include"game.h"

//Define char used as representation of empty space
#define EMPTY ' '

void error_exit(char *msg, int e){
    write(2, msg, strlen(msg));
    exit(e);
}


struct Matrix initialice_M(int n, int m){
    char *p = malloc(sizeof(char)*n*m);
    if(p == NULL) error_exit("Error:(initialice_M) malloc failure\n", 2);
    struct Matrix M;
    M.n = n;
    M.m = m;
    M.mat = p;

    int i = 0;
    while(i < n*m){
        p[i] = EMPTY;
        ++i;
    }

    return M;
}


struct Matrix initialice_draw(char *name){
    //Acces to the draw
    int fd = open(name, O_RDONLY);
    if(fd < 0) error_exit("Error:(initialice_draw) not posible to open the given file\n", 2);

    //Control and adquisition of the size of the picture
    int size = lseek(fd, 0, SEEK_END);
    if(size < 0) error_exit("Error:(initialice_draw) lseek failure\n", 2);
    if(size == 0) error_exit("Error(initialice_draw) void picture\n", 2);
    lseek(fd, 0, SEEK_SET);

    //Two buffer. buff2 contains the picture while buff contains the picture without \n or fragmentation.
    char *buff = malloc(size*sizeof(char));
    if(buff == NULL) error_exit("Error:(initialice_draw) malloc failure\n", 2);
    char *buff2 = malloc(size*sizeof(char));
    if(buff2 == NULL) error_exit("Error:(initialice_draw) malloc failure\n", 2);

    //Read
    if(read(fd, buff2, sizeof(char)*size) < 0) error_exit("Error:(initialice_draw) not posible read the file\n", 2);

    if(close(fd) < 0) error_exit("Error:(initialice_draw) close failure|n", 2);

    //Making buff (buff2 - {\n})
    int i = 0, ii = 0, n = 0;
    while (i < size) {
        if (buff2[i] != '\n' && buff2[i] != 0) {
            buff[ii] = buff2[i];
            ++ii;
        }
        else ++n;

        ++i;
    }

    int m = (size/n) - 1;
    free(buff2);

    struct Matrix M;
    M.n = n;
    M.m = m;
    M.mat = buff;

    return M;
}

void clearMatrix(struct Matrix *M){
    for(int i = 0; i < M->n * M->m; ++i){
        M->mat[i] = EMPTY;
    }
}


char insertSubMatrix(struct Matrix *M, struct Matrix *draw, int x, int y){
    if(M->n < (draw->n + y) || M->m < (draw->m + x)) error_exit("Error:(insert) draw is bigger than the Main matrix\n", 2);

    //Checking collisions

    for(int i = y; i < y + draw->n; ++i){
        for(int ii = x; ii < x + draw->m; ++ii){
            if(M->mat[i*M->m + ii] != EMPTY ) return 0;
        }
    }

    //Insert
    
    int aux = 0;
    for(int i = y; i < y + draw->n; ++i){
        for(int ii = x; ii < x + draw->m; ++ii){
            M->mat[i*M->m + ii] = draw->mat[aux];
            ++aux;
        }
    }

    return 1; //No collision
}


void eraseSubMatrix(struct Matrix *M, struct Matrix *draw, int x, int y){
    if(M->n - y < draw->n || M->m - x < draw->m) error_exit("Error:(insert) draw is bigger than the Main matrix\n", 2);

    //Checking collisions
    for(int i = y; i < y + draw->n; ++i){
        for(int ii = x; ii < x + draw->m; ++ii){
            if(M->mat[i*M->m + ii] != EMPTY ) error_exit("Test: colision", 1);
        }
    }

    //Erase
    int aux = 0;
    for(int i = y; i < y + draw->n; ++i){
        for(int ii = x; ii < x + draw->m; ++i){
            M->mat[i*M->m + ii] = EMPTY;
            ++aux;
        }
    }
}


void clearTerminal(){
    char buff[30];
    sprintf(buff, "\033[2J\033[H");
    write(1, buff, strlen(buff));
}

void Move_Terminal_Cursor_Beginning(){
    char buff[30];
    sprintf(buff, "\033[H");
    write(1, buff, strlen(buff));
}


void print_M(struct Matrix *M, char Perimeter){
    char buff[M->m + 1];

    if(Perimeter > 0){
        for(int i = 0; i < M->m; ++i) buff[i] = '-';
        buff[M->m] = '\n';
        write(1, buff, sizeof(char)*(M->m + 1));
    }

    for (int i = 0; i < M->n; ++i) {
        write(1, &M->mat[i * M->m], M->m*sizeof(char));
        write(1, "\n", 1);
    }

    if(Perimeter > 0){
        write(1, buff, sizeof(char)*(M->m + 1));
    }
}

void Nothing(){
    //Nothing to do
}

char SON_TerminalInput(int *my_pipe, int *pid){
    int pid_father = getpid();
    if(pipe2(my_pipe, O_NONBLOCK) < 0) error_exit("Error(Main) pipe2 failure\n", 2);

    int aux;
    aux =  fork();

    if(aux < 0) error_exit("Error(Main) fork failure\n", 2);
    else if(aux == 0){
        if(close(my_pipe[0])) error_exit("Error(Main-SON_Input) close failure\n", 2);

        //Modifie the SIGALRM default handler in order to avoid killing the process
        //The alarm is used to skip the syscall read if it gets blocked to much time
        struct sigaction sa;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = Nothing;

        if(sigaction(SIGALRM, &sa, NULL) < 0) error_exit("Error(Main-SON_Input) sigaction failure\n", 2);
        
        //If the father dies the son too
        while(getppid() == pid_father){
            char buff[20];

            //Set an SIGALRM in order to avoid a blocker read
            alarm(5);
            int r = read(0, buff, sizeof(char)*20);
            alarm(0);

            if(r > 0){
                if(write(my_pipe[1], buff, sizeof(char)*r) < 0) error_exit("Error(Main-SON_Input) write on pipe failure\n", 2);
                r = 0;
            }
        }

        if(close(my_pipe[1]) < 0) error_exit("Error(Main) close failure\n", 2);
        exit(1);
    }

    *pid = aux;

    if(close(my_pipe[1]) < 0) error_exit("Error(Main) close failure\n", 2);

    if(waitpid(*pid, NULL, WNOHANG) == *pid) return -1;

    return 0;
}

void Move_Object(struct Object *Obj, struct Vector_Movement *V_M, char negative){
    if(negative > 0){
        Obj->x -= V_M->x;
        Obj->y -= V_M->y;
    }
    else{
        Obj->x += V_M->x;
        Obj->y += V_M->y;
    }
}