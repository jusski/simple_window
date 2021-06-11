#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "platform.h"
#include "opengl.cpp"
#include "math.h"
#include "engine.h"

static m4 LookAt(v3 CameraPosition, v3 CameraDirection)
{
    v3 ZAxis = NOZ(-1.0f * CameraDirection);
    v3 YAxis = NOZ(Cross(ZAxis, V3(1, 0 , 0)));
    v3 XAxis = NOZ(Cross(YAxis, ZAxis));

    m4 Rotation = Rows(V4(XAxis, 0),
                       V4(YAxis, 0),
                       V4(ZAxis, 0),
                       V4(0,0,0, 1));

    v3 P = -1.0f * CameraPosition;
    m4 Translation = Columns(V4(1, 0, 0, 0),
                             V4(0, 1, 0, 0),
                             V4(0, 0, 1, 0),
                             V4(P, 1));
    
    m4 Result = Rotation * Translation;

    return(Result);
}

static m4 LookAt(v3 CameraPosition, v3 CameraDirection, v3 Up)
{
    v3 ZAxis = NOZ(-1.0f * CameraDirection);
    v3 XAxis = NOZ(Cross(Up, ZAxis));
    v3 YAxis = NOZ(Cross(ZAxis, XAxis));

    m4 Rotation = Rows(V4(XAxis, 0),
                       V4(YAxis, 0),
                       V4(ZAxis, 0),
                       V4(0,0,0, 1));

    v3 P = -1.0f * CameraPosition;
    m4 Translation = Columns(V4(1, 0, 0, 0),
                             V4(0, 1, 0, 0),
                             V4(0, 0, 1, 0),
                             V4(P, 1));
    
    m4 Result = Rotation * Translation;

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
DrawPrimitives(triangle *Triangles, int Count)
{
    glUseProgram(Program);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle) * Count, Triangles, GL_STREAM_DRAW);
    glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertex), (void*)offsetof(vertex, P));
    glEnableVertexAttribArray(Position);

    static float Time = 0.0f;
    Time += 0.01f;
    
    //m3 Rotation = Scale(0.3f) * YRotate(XAxisRotationAngle) * XRotate(YAxisRotationAngle);

    m4 ModelMatrix = XTranslate(0) * Scale(0.3f);
    glUniformMatrix4fv(Model, 1, GL_TRUE, (GLfloat *)ModelMatrix.E);
#if 0
    v3 CameraPosition = 0.9f * V3(sinf(Time), 0, cosf(Time));
    v3 Up = V3(sinf(Time), cosf(Time), 0);
    
    v3 CameraDirection = V3(0, 0, 0) - CameraPosition;
#else
    v3 CameraPosition = 0.9f * V3(LeftOffset, UpOffset, 1);
    v3 Up = V3(0, 1, 0);
    v3 CameraDirection = V3(0, 0, -1);
#endif

    
    m4 ViewMatrix = LookAt(CameraPosition, CameraDirection, Up);
    //ViewMatrix = LookAt(CameraPosition, CameraDirection);
    glUniformMatrix4fv(View, 1, GL_TRUE, (GLfloat *)ViewMatrix.E);
    
    glDrawArrays(GL_TRIANGLES, 0, Count * 3);
    
}

static void
DrawPrimitives(point *Primitives, int Count)
{
    glUseProgram(Program);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point) * Count, Primitives, GL_STREAM_DRAW);
    glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertex), (void*)offsetof(vertex, P));
    glEnableVertexAttribArray(Position);
    
    static float Time = 0.0f;
    Time += 0.01f;
  
    m4 ModelMatrix = XTranslate(LeftOffset) * Scale(0.3f);
    glUniformMatrix4fv(Model, 1, GL_TRUE, (GLfloat *)Identity.E);

    v3 CameraPosition = 0.8f * V3(sinf(Time), 0, cosf(Time));
    v3 CameraDirection = V3(0, 0, 0) - CameraPosition;
    m4 ViewMatrix = LookAt(CameraPosition, CameraDirection);

    glUniformMatrix4fv(View, 1, GL_TRUE, (GLfloat *)ViewMatrix.E);

    glDrawArrays(GL_POINTS, 0, Count);
}

static void
DrawPolygonMesh(polygon_mesh *PolygonMesh)
{
    DrawPrimitives(PolygonMesh->Triangles, PolygonMesh->TriangleCount);
    
}

extern "C" __declspec(dllexport) void
Engine(input_state *InputState, float XAngle, float YAngle)
{
    XAxisRotationAngle = XAngle; 
    YAxisRotationAngle = YAngle; 

    Input = *InputState;
    if(Input.Keyboard.Right)
    {
        LeftOffset += 0.1f;
    }
    if(Input.Keyboard.Left)
    {
        LeftOffset -= 0.1f;
    }
    if(Input.Keyboard.Up)
    {
        UpOffset += 0.1f;
    }
    if(Input.Keyboard.Down)
    {
        UpOffset -= 0.1f;
    }
    
    arena *Arena = &PersistentArena;
    if(!Program)
    {
        Arena->Memory = (unsigned char *)malloc(Megabytes(3));
        Arena->Index = 0;
        Arena->MaxIndex = Megabytes(3);
        
        Sphere = LoadModel(Arena, "../code/models/sphere.stl");
        Thorus = LoadModel(Arena, "../code/models/thorus.stl");

        Program = CreateOpenGLProgram();

        glGenBuffers(1, &VBO);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        PointCount = 1;
        Points = PushArray(Arena, point, PointCount);
    }

    glViewport(0, 0, 500, 500);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    static float Time = 0.0f;
    Time += 0.05f;
    
    DrawPrimitives(Points, PointCount);
    DrawPolygonMesh(Sphere);
    DrawPolygonMesh(Thorus);
}
