#pragma once
GLuint VBO;
GLuint Program = 0;
GLint Position;
GLint Color;
//GLint Time;

float vertices[] =
{
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
};

union vertice
{
    struct 
    {
        float P[3];
        float C[3];    
    };
    struct 
    {
        float x;
        float y;
        float z;

        float r;
        float g;
        float b;
    };
};

union triangle
{
    struct
    {
        vertice A;
        vertice B;
        vertice C;    
    };
    vertice P[3];
    
};

triangle Triangle =
{
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
};
