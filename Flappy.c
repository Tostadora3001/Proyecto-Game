#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<signal.h>
#include<sys/wait.h>
#include<time.h>
#include<string.h>
#include<stdio.h>

#include"game.h"

//Game Parameters
#define PERIMETER 1     //True
#define Main_Lenght 150
#define Main_High 33
#define Gravity 1                   //Vertical
#define Inicial_Pipe_Velocity 1    //Horizontal
#define Time_Frame_seconds 0                   //Seconds
#define Time_Frame_nanoseconds 300000000        //Nanoseconds
#define TIME_INCREASE_VELOCITY 250
#define MAX_PIPE_VELOCITY 5

//Pipes Parameters
#define NUM_PIPES 5
#define DISTANCE_B_PIPES 40

//Flappy Parameters
#define Flappy_starting_x 3
#define Flappy_starting_y 14
#define Flappy_jump 3

int score = 0;
int Max_score = 0;
int Pipe_Velocity = Inicial_Pipe_Velocity;

void Initial_Setting(struct Matrix *MainMatrix, struct Object *Flappy_Objt, struct Object *Pipes){
    clearMatrix(MainMatrix);
    Flappy_Objt->x = Flappy_starting_x;
    Flappy_Objt->y = Flappy_starting_y;

    score = 0;

    for(int i = 0; i < NUM_PIPES; ++i){
        Pipes[i].x = Main_Lenght + i * DISTANCE_B_PIPES;
        Pipes[i].y = 0;
    }
    
    char buff[256];
    insertSubMatrix(MainMatrix, &Flappy_Objt->draw, Flappy_Objt->x, Flappy_Objt->y);

    clearTerminal();
    print_M(MainMatrix, PERIMETER);

    char Num_display[256];
    sprintf(Num_display, "SCORE : %d\nMAX SCORE : %d\nSpace -> Jump / r -> Restart / q -> Exit\n", score, Max_score);
    write(1, Num_display, strlen(Num_display));
}

int main(){
    //Setup
    struct Matrix MainMatrix = initialice_M(Main_High, Main_Lenght);
    struct Matrix Auxiliar = initialice_draw("./Draws/Column_5");
    struct Matrix Pipe_Low_draw = initialice_draw("./Draws/Pipe_Low");
    struct Matrix Pipe_Medium_draw = initialice_draw("./Draws/Pipe_Medium");
    struct Matrix Pipe_High_draw = initialice_draw("./Draws/Pipe_High");

    struct Object Flappy_Objt;
    struct Object Pipes[NUM_PIPES];

    Pipes[0].draw = Pipe_Low_draw;
    Pipes[1].draw = Pipe_High_draw;
    Pipes[2].draw = Pipe_Medium_draw;
    Pipes[3].draw = Pipe_High_draw;
    Pipes[4].draw = Pipe_Low_draw;
    
    //struct Object Pipe1_Objt;
    //struct Object Pipe2_Objt;
    //struct Object Pipe3_Objt;
    //struct Object Pipe4_Objt;
    //struct Object Pipe5_Objt;

    Flappy_Objt.draw = initialice_draw("./Draws/Flappy");
    struct Vector_Movement Vector_Flappy_Gravity;
    struct Vector_Movement Vector_Flappy_Jump;

    Vector_Flappy_Jump.y = Flappy_jump;
    Vector_Flappy_Jump.x = 0;

    Vector_Flappy_Gravity.y = Gravity;
    Vector_Flappy_Gravity.x = 0;

    //Frame Time
    struct timespec ts;
    ts.tv_sec = Time_Frame_seconds;
    ts.tv_nsec = Time_Frame_nanoseconds;

    Initial_Setting(&MainMatrix, &Flappy_Objt, Pipes);

    //Main logic
    //First - Create a son process in order to manage the input from the user. The communication is made 
    //        with a pipeline. Each iteration the father will check the inputs and process them. It's managing 
    //        won't be inmediate, just in its given time.
    //
    //Second - The code works with a endless loop with three phases.
    //      1 -> Read inputs
    //      2 -> Logical operations
    //      3 -> Print in the terminal
    //
    //Third - Once the game anded or the user exit, the process will mute to the Main code

    //Controls
    //  Space -> jump the bird
    //  q -> exit and return to the menu
    //  r -> reset the game

    //First
    int my_pipe[2];
    int pid;
    if(SON_TerminalInput(my_pipe, &pid) < 0) error_exit("Error(Main) SON_TerminalInput failure\n", 2);

    //Second
    char buff[20];
    char running = 0;   //At the beginning the game is stopped, it starts when space is clicked
    int r;

    while(1){
        r = read(my_pipe[0], buff, sizeof(char)*20);
        if(r < 0 && errno != EAGAIN && errno != EWOULDBLOCK) error_exit("Error(Main) read failure\n", 2);

        //Manage the inputs
        for(int i = 0; i < r; ++i){
            if(buff[i] == ' '){
                running = 1;
                Move_Object(&Flappy_Objt, &Vector_Flappy_Jump, 1);
            }
            else if(buff[i] == 'q'){
                free(MainMatrix.mat);
                free(Auxiliar.mat);
                free(Pipe_Low_draw.mat);
                free(Pipe_Medium_draw.mat);
                free(Pipe_High_draw.mat);
                free(Flappy_Objt.draw.mat);

                if(close(my_pipe[0])) error_exit("Error(Main) close failure\n", 2);
                if(kill(pid, SIGKILL) < 0) error_exit("Error(Main) kill failure\n", 2);

                if(waitpid(pid, NULL, 0) < 0) error_exit("Error(Main) waitpid failure\n", 2);

                clearTerminal();
                execlp("./GAME_P", "./GAME_P", NULL);
                error_exit("Error(Main) error returning to the main menu\n", 2);
            }
            else if(buff[i] == 'r'){
                running = 0;
                if(Max_score < score) Max_score = score;
                Initial_Setting(&MainMatrix, &Flappy_Objt, Pipes);
            }
        }

        //Logical Operations

        if(running > 0){
            if(score % TIME_INCREASE_VELOCITY == 0 && Pipe_Velocity <= MAX_PIPE_VELOCITY) ++Pipe_Velocity;
            clearMatrix(&MainMatrix);
            //Pipes movement
            //Only draw if the x is inside the matrix
            for(int i = 0; i < NUM_PIPES; ++i){
                Pipes[i].x -= Pipe_Velocity;

                if(Pipes[i].x < 0) Pipes[i].x = Main_Lenght;
                else if((Pipes[i].x + Pipes[i].draw.m) <= Main_Lenght){
                    if(insertSubMatrix(&MainMatrix, &Pipes[i].draw, Pipes[i].x, Pipes[i].y) == 0){
                        Initial_Setting(&MainMatrix, &Flappy_Objt, Pipes);
                    }
                }
            }

            //Gravity
            Move_Object(&Flappy_Objt, &Vector_Flappy_Gravity, 0);
            if((Flappy_Objt.y + Flappy_Objt.draw.n) >= Main_High){
                Flappy_Objt.y = Main_High - Flappy_Objt.draw.n;
            }
            if(Flappy_Objt.y < 0) Flappy_Objt.y = 0;

            //Collision avoidance
            if(insertSubMatrix(&MainMatrix, &Flappy_Objt.draw, Flappy_Objt.x, Flappy_Objt.y) == 0){
                if(Max_score < score) Max_score = score;
                Initial_Setting(&MainMatrix, &Flappy_Objt, Pipes);
                running = 0;
            }
            
            //Print the Matrix
            Move_Terminal_Cursor_Beginning();
            print_M(&MainMatrix, PERIMETER);

            char Num_display[256];
            sprintf(Num_display, "SCORE : %d\nMAX SCORE : %d\n\n", score, Max_score);
            write(1, Num_display, strlen(Num_display));

            if(nanosleep(&ts, NULL) < 0) error_exit("Error(Main) nanosleep failure\n", 2);
            ++score;

        }
    }

}