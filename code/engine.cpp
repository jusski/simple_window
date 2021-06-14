#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "platform.h"
#include "opengl.cpp"
#include "math.h"
#include "engine.h"


extern "C" __declspec(dllexport) void
InitializeOpenGL(type_wglGetProcAddress *wglGetProcAddress)
{
    AssignOpenGLFunctions(wglGetProcAddress);

    #define GL_GETINTEGERV(N) {GLint T; glGetIntegerv(N, &T); printf(#N ": %d\n", T);}
    GL_GETINTEGERV(GL_MAX_VERTEX_ATTRIBS);
    GL_GETINTEGERV(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
    GL_GETINTEGERV(GL_MAX_DRAW_BUFFERS);
    glPointSize(10);
    glEnable(GL_DEPTH_TEST); 
  
}
    
static void
DrawPrimitives(opengl_program *OpenGLProgram, triangle *Triangles, int Count, camera *Camera)
{
    glUseProgram(OpenGLProgram->Program);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle) * Count, Triangles, GL_STREAM_DRAW);
    glVertexAttribPointer(OpenGLProgram->Position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertex), (void*)offsetof(vertex, Position));
    glEnableVertexAttribArray(OpenGLProgram->Position);

    m4 ModelMatrix = XTranslate(0) * Scale(0.3f);
    glUniformMatrix4fv(OpenGLProgram->Model, 1, GL_TRUE, (GLfloat *)ModelMatrix.E);
    
    m4 ViewMatrix = LookAt(Camera->Position, Camera->Direction, Camera->UpDirection);

    glUniformMatrix4fv(OpenGLProgram->View, 1, GL_TRUE, (GLfloat *)ViewMatrix.E);
    
    glDrawArrays(GL_TRIANGLES, 0, Count * 3);
    
}

static void
DrawPrimitives(opengl_program *OpenGLProgram, point *Primitives, int Count, camera *Camera)
{
    glUseProgram(OpenGLProgram->Program);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point) * Count, Primitives, GL_STREAM_DRAW);
    glVertexAttribPointer(OpenGLProgram->Position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertex), (void*)offsetof(vertex, Position));
    glEnableVertexAttribArray(OpenGLProgram->Position);
    
    glUniformMatrix4fv(OpenGLProgram->Model, 1, GL_TRUE, (GLfloat *)Identity.E);

    m4 ViewMatrix = LookAt(Camera->Position, Camera->Direction, Camera->UpDirection);
    glUniformMatrix4fv(OpenGLProgram->View, 1, GL_TRUE, (GLfloat *)ViewMatrix.E);

    glUniformMatrix4fv(OpenGLProgram->Projection, 1, GL_TRUE, (GLfloat *)Identity.E);
    
    glDrawArrays(GL_POINTS, 0, Count);
}

static void
DrawPoint(opengl_program *OpenGLProgram, v3 Point, camera *Camera)
{
    Points[0].A.Position = Point;
    DrawPrimitives(OpenGLProgram, Points, 1, Camera);
}

static void
DrawPolygonMesh(opengl_program *OpenGLProgram, polygon_mesh *PolygonMesh, camera *Camera,
                m4 Model = Identity, v3 Color = BLUE)
{
    glUseProgram(OpenGLProgram->Program);

    vertex *Vertices = PolygonMesh->Vertices;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * PolygonMesh->VertexCount,
                 Vertices, GL_STATIC_DRAW);
    //Position
    glVertexAttribPointer(OpenGLProgram->Position, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, Position));
    glEnableVertexAttribArray(OpenGLProgram->Position);

    //Normal
    glVertexAttribPointer(OpenGLProgram->Normal, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, Normal));
    glEnableVertexAttribArray(OpenGLProgram->Normal);
    
    //Color
    glUniform3f(OpenGLProgram->Color, Color.r, Color.g, Color.b);

    //View
    m4 ViewMatrix = LookAt(Camera->Position, Camera->Direction, Camera->UpDirection);
    glUniformMatrix4fv(OpenGLProgram->View, 1, GL_TRUE, (GLfloat *)ViewMatrix.E);

    //Model
    glUniformMatrix4fv(OpenGLProgram->Model, 1, GL_TRUE, (GLfloat *)Model.E);

    //Projection
#if 1
    glUniformMatrix4fv(OpenGLProgram->Projection, 1, GL_TRUE, (GLfloat *)Identity.E);
#else
    glUniformMatrix4fv(OpenGLProgram->Projection, 1, GL_TRUE, (GLfloat *)Projection.E);
#endif
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * PolygonMesh->IndexCount,
                 PolygonMesh->Indices, GL_STATIC_DRAW); 

    glDrawElements(GL_TRIANGLES, PolygonMesh->IndexCount, GL_UNSIGNED_INT, 0); 
}

static void
DrawLightSource(polygon_mesh *Emiter, camera *Camera, m4 Model = Identity,
                v3 Color = WHITE)
{
    DrawPolygonMesh(&EmiterProgram, Emiter, Camera, Model, Color);
}

extern "C" __declspec(dllexport) void
GameLoop(input_state *InputState)
{
    arena *Arena = &PersistentArena;
    if(!Initialized)
    {
        Arena->Memory = (unsigned char *)malloc(Megabytes(3));
        Arena->Index = 0;
        Arena->MaxIndex = Megabytes(3);
        
        Sphere = LoadModel(Arena, "../code/models/sphere.ply");
        //Thorus = LoadModel(Arena, "../code/models/thorus.stl");
        Cube = LoadModel(Arena, "../code/models/cube.ply");

        GLProgram = CreateOpenGLProgram();
        EmiterProgram = CreateOpenGLProgram("../code/shaders/vertex.vs", "../code/shaders/emiter.fs");

        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        
        PointCount = 1;
        Points = PushArray(Arena, point, PointCount);

        GameState = {};
        
        GameState.Camera.Position = 0.9f * V3(0, 0, 0);
        GameState.Camera.Direction = V3(0, 0, 0);
        GameState.Camera.UpDirection = V3(0, 1, 0);

        Projection = Perspective(1, 1, 0.1, 10);
        Initialized = true;
     }

    
    Input = *InputState;
    float ZCameraOffset = 0.0f;
    float XCameraOffset = 0.0f;

    if(Input.PolygonMode == true)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if(Input.Keyboard.Right)
    {
        XCameraOffset += 0.1f;
    }
    if(Input.Keyboard.Left)
    {
        XCameraOffset -= 0.1f;
    }
    if(Input.Keyboard.Up)
    {
        ZCameraOffset -= 0.1f;
    }
    if(Input.Keyboard.Down)
    {
        ZCameraOffset += 0.1f;
    }
    
    glViewport(0, 0, 500, 500);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    camera *Camera = &GameState.Camera;
    Camera->Direction = NOZ(V3(InputState->Mouse.XOffset, InputState->Mouse.YOffset, -1));
    Camera->Position = Camera->Position + ZCameraOffset * Camera->Direction +
        XCameraOffset * Cross(Camera->Direction, Camera->UpDirection);

    DrawPolygonMesh(&GLProgram, &Sphere, Camera, Scale(0.1f));

    m4 Model = ZTranslate(-0.5) * XTranslate(0.5) * YTranslate(0.5) * Scale(0.1f);
    DrawLightSource(&Sphere, Camera, Model);

    Model = YTranslate(-0.5) * XTranslate(-0.5) * Scale(0.1f);
    DrawPolygonMesh(&GLProgram, &Sphere, Camera, Model, CORAL);

    Model = ZTranslate(-0.5) * XTranslate(-0.5) * Scale(0.1f);
    DrawPolygonMesh(&GLProgram, &Cube, Camera, Model, OLIVE);

    
}
