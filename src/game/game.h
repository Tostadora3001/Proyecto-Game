//Tostadora3001
//This is an own made library where I create functions that I think are usefull for developing game in the Linux Terminal
//This code is developed with C for Linux enviroment.
//You are free to use this code as you wish, with credits although.
//Hope you find my work interesting and usefull :) .

//-------------------------------------------------------------------------------------------------

//The next functions are made for manipulate a given matrix inserting, erasing and moving submatrix 
//inside it. The idea is to be able to draw different given pictures in order to create the ilusion 
//of movement and be able to controll it.
//The code will use a structure Matrix that has integers n, m and a char pointer. The char pointer contains the draw. It 
//must be a squere (fill the gaps with " ").

#ifndef GAME_H
#define GAME_H

#include"Structs/Structs.h"

//Master Objects is a structure that contains an array of all the objects and the number of objects. It is used to manage the objects of the game in a more efficient way.
extern struct Master_Objets Master_Objets;

// Error Handling
//------------------------------------------------------------------------------------------------------//

//A function for manage errors
void error_exit(char *msg, int e);

// Matrix Initialization and Management
//------------------------------------------------------------------------------------------------------//

//Pre:
//Post: A Matrix is of n_x_m is initialiced
struct Matrix initialice_M(int n, int m);

//Pre: The given name is a path to a accesible file where the draw is store. It only contais the picture. The picture must be a 
//     rectangular matrix wuth \n at the end of each line. The empty spaces must be filled with " ".
//Post: A Matrix draw is initialiced with the content of the given file
struct Matrix initialice_draw(char *name);

//Pre:  
//Post: The given Matrix is cleaned, being filled wit " ".
void clearMatrix(struct Matrix *M);

// Matrix Manipulation
//------------------------------------------------------------------------------------------------------//

//Pre: The Matrix draw must be equal to or smaller than Matrix M and it must fit with the given coordenates.
//Post: The Matrix draw is inserted into M using values x(n) and y(m). If draw collies with another picture, the function aborts
//      and returns 0 (collision), else 1 (No collision)
void insertSubMatrix(struct Matrix *M, struct Matrix *draw, int x, int y);

//Pre: The Matrix draw must be equal to or smaller than Matrix M and it must fit with the given coordenates.
//Post: The Matrix draw is erased from Matrix M using values x(n) and y(m). The space of draw
void eraseSubMatrix(struct Matrix *M, struct Matrix *draw, int x, int y);

//Pre: 
//Post: Returns 1 if the submatrix draw can be inserted into M using values x(n) and y(m), else 0.
char Check_Valid_Insert(struct Matrix *M, struct Object *Obj);

// Terminal Output
//------------------------------------------------------------------------------------------------------//

//Pre:
//Post: The terminal is cleaned
void clearTerminal();

//Pre:
//Post: The cursor of the terminal is moved to the beginning. This function is used
//      for redrawing each frame
void Move_Terminal_Cursor_Beginning();

//Pre: A rectangular Matrix and a char
//Post: the give Matrix is printed on the Terminal. If the char is 0 (false) just the Matrix is printed
//      If the char is >0 (true) a perimeter (----) is draw above and under the matrix
void print_M(struct Matrix *M, char Perimeter);

// Terminal Input
//------------------------------------------------------------------------------------------------------//

//Pre:
//Post: Disable raw mode for terminal input
void disable_raw_mode();

//Pre:
//Post: Enable raw mode for terminal input
void enable_raw_mode();

//Pre:
//Post: Read a key from terminal input
char read_key();

// Game Mechanics
//------------------------------------------------------------------------------------------------------//

//Pre: The Object must be initialiced and the position (x, y) must be inside the matrix of the draw of the object
//Post: Check if the position (x, y) collides with the draw of the given Object. If it collides return 1, else 0.
char Check_Collision(struct Object *Obj, int x, int y);

//Pre:
//Post: the given Vector_Movement is added to the position of the object
//      If the char(used as bool) negative is 0 (false), the function will use the positive V_M,
//      if it is bigger than 0 (true) the negative of V_M 
void Move_Object(struct Object *Obj, struct Vector_Movement *V_M, char negative);

// Utility
//------------------------------------------------------------------------------------------------------//

//Pre:
//Post: Initialize the Master Objects structure
void Init_Master_Objects(int n);

//Pre:
//Post: Free the Master Objects structure
void Free_Master_Objects();

//Pre:
//Post: Do nothing
void Nothing();

//-------------------------------------------------------------------------------------------------

#endif
