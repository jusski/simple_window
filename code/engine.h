#pragma once
#include "types.h"

GLuint VBO;
GLuint EBO;

struct opengl_program
{
    GLuint Program;

    GLint Position;
    GLint Normal;
    GLint TexCoord;
    
    GLint Color;
    GLint Model;
    GLint View;
    GLint Projection;
};

opengl_program GLProgram;
opengl_program EmiterProgram;
bool Initialized = false;

struct vertex
{
    v3 Position;
    v3 Normal;
    v2 TexCoord;
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

struct polygon_mesh
{
    vertex *Vertices;
    int *Indices;

    int IndexCount;
    int VertexCount;
};

struct camera
{
    v3 Position;
    v3 Direction;
    v3 UpDirection;
};


struct game_state
{
    camera Camera;
};

game_state GameState;

arena PersistentArena;

polygon_mesh Sphere;
polygon_mesh Thorus;
polygon_mesh Cube;

point *Points;
int PointCount;


input_state Input;
float LeftOffset;
float UpOffset;

v3 BLUE = {0.1f, 0.1f, 0.9f};
v3 CORAL = {1.0f, 0.5f, 0.31f};
v3 OLIVE = {0.33f, 0.42f, 0.18f};
v3 WHITE = {0.8f, 0.9f, 0.9f};

m4 Projection;
