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

struct Matrix{
    int n;
    int m;
    char *mat;
};

//Each object has its draw and its position (refering the top left)
struct Object{
    struct Matrix draw;
    int x;  //Horizontal position
    int y;  //Vertical position
};

//A vector used to represent the movement. For exemple, if Object A has to move to the right, it has the vector (1*K, 0),
//being k the distance to move.
struct Vector_Movement{
    int x;
    int y;
};

//A function for manage errors
void error_exit(char *msg, int e);

//Pre:
//Post: A Matrix is of nxm is initialiced
struct Matrix initialice_M(int n, int m);

//Pre: The given name is a path to a accesible file where the draw is store. It only contais the picture. The picture must be a 
//     rectangular matrix wuth \n at the end of each line. The empty spaces must be filled with " ".
//Post: A Matrix draw is initialiced with the content of the given file
struct Matrix initialice_draw(char *name);

//Pre:  
//Post: The given Matrix is cleaned, being filled wit " ".
void clearMatrix(struct Matrix *M);

//Pre: The Matrix draw must be equal to or smaller than Matrix M and it must fit with the given coordenates.
//Post: The Matrix draw is inserted into M using values x(n) and y(m). If draw collies with another picture, the function aborts
//      and returns 0 (collision), else 1 (No collision)
char insertSubMatrix(struct Matrix *M, struct Matrix *draw, int x, int y);

//Pre: The Matrix draw must be equal to or smaller than Matrix M and it must fit with the given coordenates.
//Post: The Matrix draw is erased from Matrix M using values x(n) and y(m). The space of draw
void eraseSubMatrix(struct Matrix *M, struct Matrix *draw, int x, int y);

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


//-------------------------------------------------------------------------------------------------

//The next functions are made for managing needed task but with process SONs

//Pre: An empty vector of two integers is given and an empty integer that will contain SON's pid
//Post This process will read the terminal input in order to gice the data to Father with a pipeline.
//     The process will monitorice the father in order to die along with the father if he can't terminate
//     the son due to an error.
//     If during the function any error is produced the function returns -1 and SON dies writing the error.
//     The read descriptor (father) will be p[0] (p[1] close for father).
//     The write descriptor (son) will be p[1] (p[0] close for son).
//     The pid will contain SON's pid.
char SON_TerminalInput(int *my_pipe, int *pid);


//-------------------------------------------------------------------------------------------------

//The next functions are made to operate with moving objects

//Pre:
//Post: the given Vector_Movement is added to the position of the object
//      If the char(used as bool) negative is 0 (false), the function will use the positive V_M,
//      if it is bigger than 0 (true) the negative of V_M 
void Move_Object(struct Object *Obj, struct Vector_Movement *V_M, char negative);

#endif