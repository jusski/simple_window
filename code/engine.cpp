#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "platform.h"
#include "opengl.cpp"
#include "engine.h"
#include "math.h"
//#include "sphere.inl"

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
        //Color = glGetAttribLocation(Program, "aColor");
        RotationMatrix = glGetUniformLocation(Program, "RotationMatrix");
        
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
    glPointSize(10);
  
}

static void
DrawFigures(triangle *Figures, int Count)
{
    glUseProgram(Program);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle) * Count, Figures, GL_STREAM_DRAW);
    glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertex), (void*)offsetof(vertex, P));
    glEnableVertexAttribArray(Position);

    glDrawArrays(GL_TRIANGLES, 0, 3 * Count);
}

static void
DrawFigures(point *Figures, int Count)
{
    glUseProgram(Program);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point) * Count, Figures, GL_STREAM_DRAW);
    glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertex), (void*)offsetof(vertex, P));
    glEnableVertexAttribArray(Position);
    
    glDrawArrays(GL_POINTS, 0, Count);
}

static void
DrawFigures(vertex *Figures, int Count)
{
    glUseProgram(Program);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * Count, Figures, GL_STREAM_DRAW);
    glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertex), (void*)offsetof(vertex, P));
    glEnableVertexAttribArray(Position);

    static float Time = 0.0f;
    Time += 0.01f;
    
    m3 Rotation = YRotate(Time);
    glUniformMatrix3fv(RotationMatrix, 1, GL_TRUE, (GLfloat *)Rotation.E);
    
    glDrawArrays(GL_TRIANGLES, 0, Count);
}

extern "C" __declspec(dllexport) void
Engine()
{
    arena *Arena = &PersistentArena;
    if(!Program)
    {
        Arena->Memory = (unsigned char *)malloc(Megabytes(3));
        Arena->Index = 0;
        Arena->MaxIndex = Megabytes(3);
        
        Sphere = LoadModel(Arena, "../code/models/sphere.stl");
        Program = CreateOpenGLProgram();

        glGenBuffers(1, &VBO);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        PointCount = 1;
        Points = (point *)malloc(sizeof(point) * PointCount);
        
    }

    glViewport(0, 0, 500, 500);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    static float Time = 0.0f;
    Time += 0.05f;
    
    //DrawFigures(Triangles, TriangleCount);
    DrawFigures(Points, PointCount);
    DrawFigures(Sphere->Vertices, Sphere->VertexCount);
}
