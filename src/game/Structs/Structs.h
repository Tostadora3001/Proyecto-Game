#ifndef STRUCTS_H
#define STRUCTS_H

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

struct Master_Objets{
    struct Object *Master;
    int n;
};

//A vector used to represent the movement. For exemple, if Object A has to move to the right, it has the vector (1*K, 0),
//being k the distance to move.
struct Vector_Movement{
    int x;
    int y;
};

#endif