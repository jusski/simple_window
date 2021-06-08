#pragma once
GLuint VBO;
GLuint Program = 0;
GLint Position;
GLint Color;
GLint RotationMatrix;

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

triangle Triangle =
{
    -0.5f, -0.5f, 0.0f,  
    0.5f, -0.5f, 0.0f, 
    0.0f,  0.5f, 0.0f, 
};

point *Points;
int PointCount;

struct model
{
    vertex *Vertices;
    int *Indices;

    int IndexCount;
    int VertexCount;
};

arena PersistentArena;

model *Sphere;
