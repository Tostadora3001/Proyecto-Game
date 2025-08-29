#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>
#include<stdlib.h>
#include<stdio.h>

#include"game.h"

#define MAX_SONS 10
int main(){
    char buff[256] = "Welcome to GAME Proyect (1.0)\nAvaible games:\n"
    "  1 -Flappy (1.0)\n"
    "Enter the number of the desired game\n"
    "Enter h if help needed\n"
    "Enter q for exit\n\n";

    write(1, buff, strlen(buff));

    int pid_sons[MAX_SONS];
    char command;
    while(read(0, &command, sizeof(char)*1)){
        //Exit
        if(command == 'q'){
            for(int i = 0; i < MAX_SONS; ++i){
                kill(pid_sons[i], SIGKILL);
            }

            clearTerminal();
            exit(1);
        }
        //Help
        else if(command == 'h'){
            sprintf(buff, "Command List :\n"
                "-q = Exit\n"
                "-h = Help\n"
                "-l = List\n"
                "Number = The choosen game is started\n\n");
            write(1, buff, strlen(buff));
        }
        //Flappy
        else if(command == '1'){
            execlp("./Flappy", "./Flappy", NULL);
            error_exit("Error starting the choosed game\n\n", 2);
        }
        //Commands to ignore
        else if(command == '\n');
        //Bad command
        else{
            sprintf(buff, "Not valid command\n\n");
            write(1, buff, strlen(buff));
        }
    }
}