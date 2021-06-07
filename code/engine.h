#pragma once
GLuint VBO;
GLuint Program = 0;
GLint Position;
GLint Color;
//GLint Time;

union vertex
{
    struct 
    {
        float x;
        float y;
        float z;

        float r;
        float g;
        float b;
    };
    struct
    {
        v3 P;
        v3 C;
    };
};

union vertex2
{
    float x,y,z;
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
    vertex2 A;
};

triangle Triangle =
{
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
};

triangle *Triangles;
int TriangleCount;

point *Points;
int PointCount;
