#pragma once
#include "types.h"
#include "opengl.h"

GLuint VBO;
GLuint EBO;
GLuint FBO;
GLuint ColorBuffer[2];
GLuint TestTexture;
GLuint TestTexture128;

int ScreenWidth = 512;
int ScreenHeight = 512;

struct opengl_program
{
    GLuint Program;

    GLint Position;
    GLint Normal;
    GLint TexCoord;
    
    GLint Color;

    GLint LightSource;
    GLint CameraPosition;
    
    GLint Model;
    GLint View;
    GLint Projection;
};

opengl_program *GLProgram;
opengl_program *EmiterProgram;
bool Initialized = false;

struct vertex
{
    v3 Position;
    v3 Normal;
    v2 TexCoord;
};

struct triangle
{
    v3 A;
    v3 B;
    v3 C;    
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

struct object3d
{
    polygon_mesh *Mesh;
    m4 WorldSpaceTransform;
    m4 InverseTransform;
    v3 Color;

    opengl_program *Program;
    GLuint VBO;
    GLuint EBO;

    v3 Origin;    // Bounding box
    float SquaredRadius; // Bounding box
};

struct game_state
{
    camera Camera;
    float Time;
};

game_state GameState;

arena PersistentArena;

polygon_mesh *Sphere;
polygon_mesh *Thorus;
polygon_mesh *Cube;
polygon_mesh *BRDM;
polygon_mesh *Cylinder;

point Points[10];
int PointCount = 10;


input_state Input;
float LeftOffset;
float UpOffset;

v3 BLUE = {0.1f, 0.1f, 0.9f};
v3 CORAL = {1.0f, 0.5f, 0.31f};
v3 OLIVE = {0.33f, 0.42f, 0.18f};
v3 WHITE = {0.8f, 0.9f, 0.9f};
v3 BLACK = {0.2f, 0.3f, 0.3f};
v3 YELLOW = {1.0f, 1.0f, 0.f};
v3 SelectedObjectColor;

m4 Projection;

object3d *CubeObject;
object3d *SphereObject;
object3d *BRDMObject;
object3d *SelectedObject;

object3d *Objects3d[10];
int ObjectCount = 0;
