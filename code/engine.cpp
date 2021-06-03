#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "opengl.h"
#include "engine.h"
#include "math.h"

static char *
ReadShader(const char *Path)
{
    char *Result = 0;

    FILE *FileHandle = fopen(Path, "rb");
    if (FileHandle)
    {
        long FileSize;
        fseek(FileHandle, 0, SEEK_END);
        FileSize = ftell(FileHandle);
        fseek(FileHandle, 0, SEEK_SET);

        Result = (char *)malloc(FileSize + 1);
        if ((fread(Result, FileSize, 1, FileHandle)) != 1)
        {
            __debugbreak();
            Result = 0;
        }

        Result[FileSize] = 0;
        fclose(FileHandle);    
    }
    else
    {
        fprintf(stderr, "[ERROR]: File not found: %s\n", Path);
    }

    return(Result);
}


static GLuint
CreateShader(GLint ShaderType, const char *ShaderPath)
{
    GLuint Result = 0;

    char *ShaderSource = ReadShader(ShaderPath);
    if(ShaderSource)
    {
        GLuint VertexShader;
        VertexShader = glCreateShader(ShaderType);

        glShaderSource(VertexShader, 1, &ShaderSource, 0);
        glCompileShader(VertexShader);

        int  Success;
        glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Success);
    
        if(Success)
        {
            Result = VertexShader;
        }
        else
        {
            char InfoLog[512];
            glGetShaderInfoLog(VertexShader, sizeof(InfoLog), 0, InfoLog);
            char *Type = (ShaderType == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
            fprintf(stderr, "ERROR: %s::SHADER::COMPILATION_FAILED\n%s\n", Type, InfoLog);
        }
        free(ShaderSource);
    }
    return(Result);
}

static GLuint
CreateOpenGLProgram()
{
    GLuint Result = 0;

    GLuint VertexShader = CreateShader(GL_VERTEX_SHADER, "../code/shaders/vertex.vs");
    GLuint FragmentShader = CreateShader(GL_FRAGMENT_SHADER, "../code/shaders/fragment.fs");

    Program = glCreateProgram();
    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);
    glLinkProgram(Program);

    int Success;
    glGetProgramiv(Program, GL_LINK_STATUS, &Success);
    if(Success)
    {
        Result = Program;
        Position = glGetAttribLocation(Program, "aPosition");
        Color = glGetAttribLocation(Program, "aColor");
        //Time = glGetUniformLocation(Program, "Time");
        
        glDeleteShader(VertexShader);
        glDeleteShader(FragmentShader);
    }
    else
    {
        char InfoLog[512];
        glGetProgramInfoLog(Program, sizeof(InfoLog), 0, InfoLog);
        fprintf(stderr, "ERROR: PROGRAM::LINK_FAILED\n%s\n", InfoLog);
    }

    return(Result);
}

extern "C" __declspec(dllexport) void
InitializeOpenGL(type_wglGetProcAddress *wglGetProcAddress)
{
    AssignOpenGLFunctions(wglGetProcAddress);

    #define GL_GETINTEGERV(N) {GLint T; glGetIntegerv(N, &T); printf(#N ": %d\n", T);}
    GL_GETINTEGERV(GL_MAX_VERTEX_ATTRIBS);
    GL_GETINTEGERV(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
    GL_GETINTEGERV(GL_MAX_DRAW_BUFFERS);
  
} 

extern "C" __declspec(dllexport) void
Engine()
{
    if(!Program)
    {
        Program = CreateOpenGLProgram();

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

       //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        printf("offsetof: %zd\n", offsetof(vertice, P));
        printf("offsetof: %zd\n", offsetof(vertice, C));
    }

    glViewport(0, 0, 500, 500);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(Program);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), &Triangle, GL_STREAM_DRAW);
    glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertice), (void*)offsetof(vertice, P));
    glEnableVertexAttribArray(Position);

    glVertexAttribPointer(Color, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertice), (void*)offsetof(vertice, C));
    glEnableVertexAttribArray(Color);

    static float Time = 0.0f;
    Time += 0.01f;
    
    //glUniform1f(Time, 1.0f);
    Triangle.A.x = ClampRange(sinf(Time), -0.2, -0.8, -1.0, 1.0);
    Triangle.A.y = ClampRange(sinf(Time), -0.2, -0.8, -1.0, 1.0);
    //printf("%f ", vertices[0]);
    
    glDrawArrays(GL_TRIANGLES, 0, 3);

}
