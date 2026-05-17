#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>
#include<stdlib.h>
#include<stdio.h>

#include"src/game/game.h"

// Directori where the binaries are stored
#define BIN_DIRECTORY "./bin/"
#define MAX_SONS 10

#define INITIAL_MESSAGE "Welcome to the GAME Project v1.0!\n\nAvailable Games:\n" \
                        "  1. Flappy (v1.0)\n\n" \
                        "Enter the number of the game you wish to play.\n" \
                        "Type 'h' for help or 'q' to exit.\n\n"

#define COMMAND_LIST "Command List:\n" \
                     " - q = Exit\n" \
                     " - h = Help\n" \
                     " - l = List available games\n" \
                     " - Number = Start the chosen game\n\n"

#define AVAILBABLE_GAMES "Available games:\n1 - Flappy (1.0)\n\n"

#define ERROR_COMMAND "Error: Unknown command\n\n"
int main(){
    // Start the game menu and manage the inputs
    enable_raw_mode();
    
    // Print the initial message
    write(STDOUT_FILENO, INITIAL_MESSAGE, strlen(INITIAL_MESSAGE));

    char command;
    while(1){
        command = read_key();
        //Exit
        if(command == 'q'){
            clearTerminal();
            exit(1);
        }
        //Help
        else if(command == 'h'){
            write(STDOUT_FILENO, COMMAND_LIST, strlen(COMMAND_LIST));
        }
        else if(command == 'l'){
            write(STDOUT_FILENO, AVAILBABLE_GAMES, strlen(AVAILBABLE_GAMES));
        }
        //Flappy
        else if(command == '1'){
            disable_raw_mode();
            execlp(BIN_DIRECTORY "Flappy", BIN_DIRECTORY "Flappy", NULL);
            error_exit("Error starting the choosed game\n\n", 2);
        }
    }
}