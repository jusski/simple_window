#pragma once
#include "types.h"

GLuint VBO;
GLuint Program = 0;
GLint Position;
GLint Color;
GLint Model;
GLint View;
GLint Projection;

float XAxisRotationAngle;
float YAxisRotationAngle;

//m4 ViewMatrix;

union vertex
{
    struct
    {
        float x,y,z;
    };
    v3 P;
};

struct triangle
{
    vertex A;
    vertex B;
    vertex C;    
};

struct point
{
    vertex A;
};

struct line
{
    vertex A;
    vertex B;
};

struct polygon_mesh
{
    triangle *Triangles;
    int *Indices;

    int IndexCount;
    int TriangleCount;
};

arena PersistentArena;

polygon_mesh *Sphere;
polygon_mesh *Thorus;

point *Points;
int PointCount;


m4 Identity =
{
    V4(1, 0, 0, 0),
    V4(0, 1, 0, 0),
    V4(0, 0, 1, 0),
    V4(0, 0, 0, 1)
};
    
static triangle
Triangle(vertex A, vertex B, vertex C)
{
    triangle Result = {A, B, C};
    return(Result);
}

input_state Input;
float LeftOffset;
float UpOffset;
