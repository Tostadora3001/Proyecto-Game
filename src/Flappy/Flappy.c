#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include "Flappy.h"

// Needed Data
#define DRAW_DIRECTORY "./Draws/Flappy_Game/"
#define DRAW_UTIL_DIRECTORY "./Draws/Utils/"
#define MUSIC_DIRECTORY "./Music/"

// Game Parameters
#define PERIMETER 1 // True
#define Main_Lenght 150
#define Main_High 33
#define Gravity 1                        // Vertical
#define Inicial_Pipe_Velocity 1          // Horizontal
#define Time_Frame_seconds 0             // Seconds
#define Time_Frame_nanoseconds 200000000 // Nanoseconds
#define TIME_INCREASE_VELOCITY 250
#define MAX_PIPE_VELOCITY 5

// Pipes Parameters
#define NUM_PIPES 5 * 2 // Each pipe is formed by 2 objects, the upper and the lower one
#define DISTANCE_B_PIPES 40
#define HOLE_PIPE 10

// Flappy Parameters
#define Flappy_starting_x 3
#define Flappy_starting_y 14
#define Flappy_jump 3

int score = 0;
int Max_score = 0;
int Pipe_Velocity = Inicial_Pipe_Velocity;
int Last_Pipe;
struct Matrix Pipe_Low_draw1;
struct Matrix Pipe_Low_draw2;

struct Matrix Pipe_Medium_draw1;
struct Matrix Pipe_Medium_draw2;

struct Matrix Pipe_High_draw1;
struct Matrix Pipe_High_draw2;


void Initial_Setting(struct Matrix *MainMatrix, struct Object *Flappy_Objt, struct Object *Pipes)
{
    clearMatrix(MainMatrix);
    Flappy_Objt->x = Flappy_starting_x;
    Flappy_Objt->y = Flappy_starting_y;

    score = 0;

    for (int i = 0; i < NUM_PIPES; i += 2)
    {
        Pipes[i].x = Main_Lenght + i * DISTANCE_B_PIPES;
        Pipes[i].y = 0;
        
        Pipes[i+1].x = Main_Lenght + i * DISTANCE_B_PIPES;
        Pipes[i+1].y = Pipes[i].draw.n + HOLE_PIPE;
    }
    Last_Pipe = NUM_PIPES - 1; 

    Pipe_Velocity = Inicial_Pipe_Velocity;

    Move_Terminal_Cursor_Beginning();
    insertSubMatrix(MainMatrix, &Flappy_Objt->draw, Flappy_Objt->x, Flappy_Objt->y);
    print_M(MainMatrix, PERIMETER);

    char Num_display[256];
    sprintf(Num_display, "SCORE : %d\nMAX SCORE : %d\nSpace -> Jump / r -> Restart / q -> Exit\n", score, Max_score);
    write(1, Num_display, strlen(Num_display));
}

void Random_Pipe(struct Object *Pipe1, struct Object *Pipe2){
    int r = rand() % 3;

    switch (r)
    {
    case 0:
        Pipe1->draw = Pipe_Low_draw1;
        Pipe2->draw = Pipe_Low_draw2;
        break;
    case 1:
        Pipe1->draw = Pipe_Medium_draw1;
        Pipe2->draw = Pipe_Medium_draw2;
        break;
    case 2:
        Pipe1->draw = Pipe_High_draw1;
        Pipe2->draw = Pipe_High_draw2;
        break;
    }
}

void Exit_GAME(){
    Free_Master_Objects();
    clearTerminal();
    Stop_Music();
    disable_raw_mode();
    clearTerminal();
    disable_raw_mode();
    execlp("./GAME_P", "./GAME_P", NULL);
    error_exit("Error(Main) error returning to the main menu\n", 2);
}

int main()
{
    enable_raw_mode();

    // Setup
    struct Matrix MainMatrix = initialice_M(Main_High, Main_Lenght);
    //struct Matrix Auxiliar = initialice_draw(DRAW_UTIL_DIRECTORY "Column_5");
    Pipe_Low_draw1 = initialice_draw(DRAW_DIRECTORY "Pipe_Low1");
    Pipe_Low_draw2 = initialice_draw(DRAW_DIRECTORY "Pipe_Low2");

    Pipe_Medium_draw1 = initialice_draw(DRAW_DIRECTORY "Pipe_Medium1");
    Pipe_Medium_draw2 = initialice_draw(DRAW_DIRECTORY "Pipe_Medium2");

    Pipe_High_draw1 = initialice_draw(DRAW_DIRECTORY "Pipe_High1");
    Pipe_High_draw2 = initialice_draw(DRAW_DIRECTORY "Pipe_High2");

    //Malloc for all the objects, including the Flappy and the Pipes
    Init_Master_Objects(NUM_PIPES + 1); // +1 for Flappy

    //Set Flappy Object
    Master_Objets.Master[Master_Objets.n - 1].draw = initialice_draw(DRAW_DIRECTORY "Flappy");
    struct Object *Flappy = &Master_Objets.Master[Master_Objets.n - 1];

    // Initial Random Pipes
    srand(time(NULL));
    for (int i = 0; i < NUM_PIPES; i += 2)
    {
        Random_Pipe(&Master_Objets.Master[i], &Master_Objets.Master[i + 1]);
    }
    struct Object *Pipes = Master_Objets.Master;

    struct Vector_Movement Vector_Flappy_Gravity;
    struct Vector_Movement Vector_Flappy_Jump;
    struct Vector_Movement Pipe_Velocity_Vector;

    Pipe_Velocity_Vector.x = Pipe_Velocity;
    Pipe_Velocity_Vector.y = 0;

    Vector_Flappy_Jump.y = Flappy_jump;
    Vector_Flappy_Jump.x = 0;

    Vector_Flappy_Gravity.y = Gravity;
    Vector_Flappy_Gravity.x = 0;

    // Frame Time
    struct timespec ts;
    ts.tv_sec = Time_Frame_seconds;
    ts.tv_nsec = Time_Frame_nanoseconds;

    Initial_Setting(&MainMatrix, Flappy, Pipes);

    // Controls
    //   Space -> jump the bird
    //   q -> exit and return to the menu
    //   r -> reset the game

    char running = 0; // At the beginning the game is stopped, it starts when space is clicked
    int r;  // Input char

    Start_Music(MUSIC_DIRECTORY "inono777-game-8-bit-399898.mp3");

    while (1)
    {
        r = read_key();

        // Manage the inputs
        if (r == ' ')
        {
            running = 1;
            Move_Object(Flappy, &Vector_Flappy_Jump, 1);
        }
        else if (r == 'q')
        {
            Exit_GAME();
        }
        else if (r == 'r')
        {
            running = 0;
            if (Max_score < score)
                Max_score = score;
            srand(time(NULL));
            clearTerminal();
            Initial_Setting(&MainMatrix, Flappy, Pipes);
        }

        // Logical Operations

        if (running > 0)
        {
            // The Pipes velocity increases each TIME_INCREASE_VELOCITY points, until it reaches MAX_PIPE_VELOCITY
            if (score % TIME_INCREASE_VELOCITY == 0 && Pipe_Velocity <= MAX_PIPE_VELOCITY){
                ++Pipe_Velocity;
                Pipe_Velocity_Vector.x = Pipe_Velocity;
            }
            clearMatrix(&MainMatrix);

            // Pipes movement
            // Only draw if the x is inside the matrix
            for (int i = 0; i < NUM_PIPES; i += 2)
            {
                Move_Object(&Pipes[i], &Pipe_Velocity_Vector, 1);
                Move_Object(&Pipes[i + 1], &Pipe_Velocity_Vector, 1);

                if (Pipes[i].x < 0){
                    Pipes[i].x = Pipes[Last_Pipe].x + DISTANCE_B_PIPES;
                    Pipes[i+1].x = Pipes[Last_Pipe].x + DISTANCE_B_PIPES;
                    Last_Pipe = i;
                    Random_Pipe(&Pipes[i], &Pipes[i + 1]);
                }
                
                if (Check_Valid_Insert(&MainMatrix, &Pipes[i]))
                {
                    insertSubMatrix(&MainMatrix, &Pipes[i].draw, Pipes[i].x, Pipes[i].y);
                    insertSubMatrix(&MainMatrix, &Pipes[i + 1].draw, Pipes[i + 1].x, Pipes[i + 1].y);
                }
            }

            // Gravity
            int b = Check_Collision(Flappy, Flappy->x, Flappy->y);
            //int b = 0;
            if(b == 0){
                // Check Matrix limits
                if(Check_Valid_Insert(&MainMatrix, Flappy)){
                    Move_Object(Flappy, &Vector_Flappy_Gravity, 0);
                    if(!Check_Valid_Insert(&MainMatrix, Flappy)){
                        Move_Object(Flappy, &Vector_Flappy_Gravity, 1);
                    }
                }
                else{
                    Flappy->y = 0;
                }
                insertSubMatrix(&MainMatrix, &Flappy->draw, Flappy->x, Flappy->y);
            }
            else{
                if (Max_score < score) Max_score = score;
                Initial_Setting(&MainMatrix, Flappy, Pipes);
                running = 0;
            }

            // Print the Matrix
            Move_Terminal_Cursor_Beginning();
            print_M(&MainMatrix, PERIMETER);

            char Num_display[256];
            sprintf(Num_display, "SCORE : %d\nMAX SCORE : %d\n\n", score, Max_score);
            write(1, Num_display, strlen(Num_display));

            
            if (nanosleep(&ts, NULL) < 0)
                error_exit("Error(Main) nanosleep failure\n", 2);
            ++score;
        }
    }

    Free_Master_Objects();
    Stop_Music();
}