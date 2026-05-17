#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<signal.h>
#include<termio.h>

#include"game.h"

//Define char used as representation of empty space
#define EMPTY ' '

//Global variable used to restore the terminal settings at the end of the game
struct termios original_termios;

//Master Objects is a structure that contains an array of all the objects and the number of objects. It is used to manage the objects of the game in a more efficient way.
struct Master_Objets Master_Objets;


// Error Handling
//------------------------------------------------------------------------------------------------------//
void error_exit(char *msg, int e){
    int f = open("error_log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if(f < 0) {
        perror("Error opening error log file");
        exit(1);
    }

    write(f, msg, strlen(msg));
    exit(e);
}


// Matrix Initialization and Management
//------------------------------------------------------------------------------------------------------//
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

    char *filebuf = malloc(size * sizeof(char));
    if (filebuf == NULL) error_exit("Error:(initialice_draw) malloc failure\n", 2);

    int read_bytes = read(fd, filebuf, size * sizeof(char));
    if (read_bytes < 0) error_exit("Error:(initialice_draw) not posible read the file\n", 2);
    if (read_bytes != size) error_exit("Error:(initialice_draw) unexpected read size\n", 2);

    if (close(fd) < 0) error_exit("Error:(initialice_draw) close failure\n", 2);

    int n = 0;
    int m = 0;
    int current_line = 0;

    for (int i = 0; i < size; ++i) {
        if (filebuf[i] == '\n') {
            if (current_line > m) m = current_line;
            current_line = 0;
            ++n;
        }
        else if (filebuf[i] != '\r' && filebuf[i] != 0) {
            ++current_line;
        }
    }

    if (size > 0 && filebuf[size - 1] != '\n') {
        if (current_line > m) m = current_line;
        ++n;
    }

    if (n <= 0 || m <= 0) error_exit("Error:(initialice_draw) invalid picture dimensions\n", 2);

    char *matrix = malloc(n * m * sizeof(char));
    if (matrix == NULL) error_exit("Error:(initialice_draw) malloc failure\n", 2);

    for (int i = 0; i < n * m; ++i) matrix[i] = EMPTY;

    int row = 0;
    int col = 0;

    for (int i = 0; i < size; ++i) {
        if (filebuf[i] == '\n') {
            row++;
            col = 0;
            continue;
        }
        if (filebuf[i] == '\r' || filebuf[i] == 0) continue;
        if (row < n && col < m) {
            matrix[row * m + col] = filebuf[i];
        }
        ++col;
    }

    free(filebuf);

    struct Matrix M;
    M.n = n;
    M.m = m;
    M.mat = matrix;

    return M;
}

void clearMatrix(struct Matrix *M){
    for(int i = 0; i < M->n * M->m; ++i){
        M->mat[i] = EMPTY;
    }
}


// Matrix Manipulation
//------------------------------------------------------------------------------------------------------//

void insertSubMatrix(struct Matrix *M, struct Matrix *draw, int x, int y){
    if(M->n < (draw->n + y) || M->m < (draw->m + x)) error_exit("Error:(insert) draw is bigger than the Main matrix\n", 2);

    //Insert
    int aux = 0;
    for(int i = y; i < y + draw->n; ++i){
        for(int ii = x; ii < x + draw->m; ++ii){
            M->mat[i*M->m + ii] = draw->mat[aux];
            ++aux;
        }
    }
}


void eraseSubMatrix(struct Matrix *M, struct Matrix *draw, int x, int y){
    if(M->n - y < draw->n || M->m - x < draw->m) error_exit("Error:(insert) draw is bigger than the Main matrix\n", 2);

    //Erase
    int aux = 0;
    for(int i = y; i < y + draw->n; ++i){
        for(int ii = x; ii < x + draw->m; ++ii){
            M->mat[i*M->m + ii] = EMPTY;
            ++aux;
        }
    }
}

char Check_Valid_Insert(struct Matrix *M, struct Object *Obj){
    if(M->n < (Obj->draw.n + Obj->y) || M->m < (Obj->draw.m + Obj->x)) return 0;
    else if(Obj->x < 0 || Obj->y < 0) return 0;
    else return 1;
}


// Terminal Output
//------------------------------------------------------------------------------------------------------//

void clearTerminal(){
    write(STDOUT_FILENO, "\033[2J\033[H", 7);
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


// Terminal Input
//------------------------------------------------------------------------------------------------------//

void disable_raw_mode() {
    // Return to the normal screen buffer and show the cursor
    write(STDOUT_FILENO, "\x1b[?1049l", 8);
    write(STDOUT_FILENO, "\x1b[?25h", 6);
    write(STDOUT_FILENO, "\e[?25h", 6); 
    // Noncannonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

void handle_signal(int sig);

void enable_raw_mode() {
    int e = tcgetattr(STDIN_FILENO, &original_termios);
    if (e < 0) error_exit("Error al obtener la configuración del terminal\n", 2);

    // In case of a signal, we want to restore the terminal settings before exiting
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    atexit(disable_raw_mode);

    struct termios raw = original_termios;
    
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    // Switch to the alternate screen buffer
    write(STDOUT_FILENO, "\e[?25l", 6); // Hide cursor
    write(STDOUT_FILENO, "\x1b[?1049h", 8);
    write(STDOUT_FILENO, "\x1b[H", 3);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

char read_key() {
    char c = '\0';
    
    if (read(STDIN_FILENO, &c, 1) == -1) {
        perror("Error al leer teclado");
        exit(1);
    }
    
    return c;
}


// Game Mechanics
//------------------------------------------------------------------------------------------------------//

// Es recomendable usar bool de <stdbool.h> en C moderno, pero mantengo char por tu diseño
char Check_Collision(struct Object *Obj, int x, int y) {
    if (Obj == NULL) return 0; 

    int obj_w = Obj->draw.m;
    int obj_h = Obj->draw.n;

    for (int i = 0; i < Master_Objets.n; ++i) {
        if (Obj == &Master_Objets.Master[i]) continue;

        int master_x = Master_Objets.Master[i].x;
        int master_y = Master_Objets.Master[i].y;
        int master_w = Master_Objets.Master[i].draw.m; 
        int master_h = Master_Objets.Master[i].draw.n;

        if (x < master_x + master_w && x + obj_w > master_x && y < master_y + master_h && y + obj_h > master_y) {    
            return 1;
        }
    }
    
    return 0; // No hay colisión
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


// Utility
//------------------------------------------------------------------------------------------------------//

void Init_Master_Objects(int n){
    Master_Objets.n = n;
    if((Master_Objets.Master = malloc(sizeof(struct Object) * n)) == NULL) error_exit("Error:(Init_Master_Objects) malloc failure\n", 2);
}

void Free_Master_Objects(){
    free(Master_Objets.Master);
    Master_Objets.n = 0;
}

// Esta función se activará cuando pulses Ctrl+C
void handle_signal(int sig) {
    disable_raw_mode();
    _exit(128 + sig);
}

void Nothing(){
    //Nothing to do
}
