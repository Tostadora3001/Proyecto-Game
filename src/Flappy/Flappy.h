//Flappy.h
#ifndef FLAPPY_H
#define FLAPPY_H

#include "../game/game.h"
#include "../Music/Music.h"

//  This function is used to set the initial settings of the game, such as the position of the pipes and the flappy, and to clear the terminal and print the initial state of the game.
void Initial_Setting(struct Matrix *MainMatrix, struct Object *Flappy_Objt, struct Object *Pipes);

void Random_Pipe(struct Object *Pipe1, struct Object *Pipe2);

void Exit_GAME();
#endif