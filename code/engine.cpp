#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "platform.h"
#include "engine.h"

#include "helpers.cpp"
#include "opengl.cpp"
#include "math.h"
#include "ray_trace.cpp"

extern "C" __declspec(dllexport) void
ExportFunctions(exported_functions Functions)
{
    PlatformPrint = Functions.PlatformPrint;
    wglGetProcAddress = Functions.wglGetProcAddress;
}

static void
InitializeOpenGL()
{
    AssignOpenGLFunctions();

    #define GL_GETINTEGERV(N) {GLint T; glGetIntegerv(N, &T); printf(#N ": %d\n", T);}
#if 0
    GL_GETINTEGERV(GL_MAX_VERTEX_ATTRIBS);
    GL_GETINTEGERV(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
    GL_GETINTEGERV(GL_MAX_DRAW_BUFFERS);
#endif
    glPointSize(10);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}
    
static void
DrawPrimitives(opengl_program *OpenGLProgram, triangle *Triangles, int Count, camera *Camera)
{
    glUseProgram(OpenGLProgram->Program);
    //
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
DrawPolygonMesh(opengl_program *Program, polygon_mesh *PolygonMesh, camera *Camera,
                v4 LightSource, m4 Model = Identity, v3 Color = BLUE)
{
    glUseProgram(Program->Program);

    vertex *Vertices = PolygonMesh->Vertices;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * PolygonMesh->VertexCount,
                 Vertices, GL_STATIC_DRAW);
    // Position
    glVertexAttribPointer(Program->Position, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, Position));
    glEnableVertexAttribArray(Program->Position);

    // Normal
    glVertexAttribPointer(Program->Normal, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, Normal));
    glEnableVertexAttribArray(Program->Normal);
    
    // Color
    glUniform3f(Program->Color, Color.r, Color.g, Color.b);

    // LightSource
    LightSource = WorldSpaceToObjectSpace(Model) * LightSource;
    glUniform4f(Program->LightSource, LightSource.x, LightSource.y, LightSource.z, LightSource.w);

    // CameraPosition
    v4 CameraPosition = WorldSpaceToObjectSpace(Model) * V4(Camera->Position, 1);
    glUniform4f(Program->CameraPosition, CameraPosition.x, CameraPosition.y, CameraPosition.z, CameraPosition.w);
    
    // View
    m4 ViewMatrix = LookAt(Camera->Position, Camera->Direction, Camera->UpDirection);
    glUniformMatrix4fv(Program->View, 1, GL_TRUE, (GLfloat *)ViewMatrix.E);

    // Model
    glUniformMatrix4fv(Program->Model, 1, GL_TRUE, (GLfloat *)Model.E);

    // Projection
#if 0
    glUniformMatrix4fv(Program->Projection, 1, GL_TRUE, (GLfloat *)Identity.E);
#else
    glUniformMatrix4fv(Program->Projection, 1, GL_TRUE, (GLfloat *)Projection.E);
#endif
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * PolygonMesh->IndexCount,
                 PolygonMesh->Indices, GL_STATIC_DRAW); 

    glDrawElements(GL_TRIANGLES, PolygonMesh->IndexCount, GL_UNSIGNED_INT, 0); 
}

static void
DrawLightSource(opengl_program *Program, polygon_mesh *Emiter, camera *Camera,
                m4 Model = Identity, v3 Color = WHITE)
{
    DrawPolygonMesh(Program, Emiter, Camera, V4(0, 0, 0, 0), Model, Color);
}

extern "C" __declspec(dllexport) void
GameLoop(input_state *InputState)
{
    arena *Arena = &PersistentArena;

    static float Time = 0;
    Time += 0.01f;

    if(!Initialized)
    {
        InitializeOpenGL();
        Arena->Memory = (unsigned char *)malloc(Megabytes(3));
        Arena->Index = 0;
        Arena->MaxIndex = Megabytes(3);

        // Load Models
        Sphere = PushStruct(Arena, polygon_mesh);
        *Sphere = LoadModel(Arena, "../code/models/sphere.ply");
        Thorus = PushStruct(Arena, polygon_mesh);
        Cube = PushStruct(Arena, polygon_mesh);
        *Cube = LoadModel(Arena, "../code/models/cube.ply");
        BRDM = PushStruct(Arena, polygon_mesh);
        *BRDM = LoadModelSTL(Arena, "../code/models/BRDM.stl");

        // Create OpenGL Program
        GLProgram = PushStruct(Arena, opengl_program);
        *GLProgram = CreateOpenGLProgram("../code/shaders/vertex.vs", "../code/shaders/fragment.fs");
        EmiterProgram =PushStruct(Arena, opengl_program);
        *EmiterProgram = CreateOpenGLProgram("../code/shaders/vertex.vs", "../code/shaders/emiter.fs");
        // TODO move to object
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        GameState = {};
        GameState.Camera.Position = 0.9f * V3(0, 0, 1);
        GameState.Camera.Direction = V3(0, 0, -1);
        GameState.Camera.UpDirection = V3(0, 1, 0);
        
        Projection = Perspective(1, 1, 1, 500);

        Initialized = true;
     }

    GLint DEPTH_FUNC[10];
    glGetIntegerv(GL_DEPTH_FUNC, DEPTH_FUNC);
    PrintLine(DEPTH_FUNC[0]);

    PrintLine(glIsEnabled(GL_DEPTH_TEST));
    PrintLine(glIsEnabled(GL_CULL_FACE));
    PrintLine(glIsEnabled(GL_SCISSOR_TEST));
    PrintLine(glIsEnabled(GL_STENCIL_TEST));
    
    Input = *InputState;
    float ZCameraOffset = 0.0f;
    float XCameraOffset = 0.0f;
    float YCameraOffset = 0.0f;

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
        ZCameraOffset = 0.1f;
    }
    if(Input.Keyboard.Down)
    {
        ZCameraOffset -= 0.1f;
    }
    if(Input.Mouse.Wheel != 0)
    {
        ZCameraOffset += Input.Mouse.Wheel * 0.1f;
    }
    
    glViewport(0, 0, 500, 500);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    camera *Camera = &GameState.Camera;

    v3 CameraXAxis = NOZ(Cross(Camera->Direction, Camera->UpDirection));
    v3 CameraYAxis = NOZ(Cross(CameraXAxis, Camera->Direction));
    Camera->UpDirection = CameraYAxis;

    if(InputState->Mouse.LButton == false)
    {
        v3 DirectionOffset = InputState->Mouse.XOffset * CameraXAxis +
                             InputState->Mouse.YOffset * CameraYAxis;
        Camera->Direction += DirectionOffset; 
        Camera->Direction = NOZ(Camera->Direction);
    }
    else
    {
        XCameraOffset += InputState->Mouse.XOffset;
        YCameraOffset += InputState->Mouse.YOffset;
    }
    Camera->Position = Camera->Position + ZCameraOffset * Camera->Direction +
                       XCameraOffset * CameraXAxis + YCameraOffset * CameraYAxis;
    
    // Sun
    m4 WorldSpace = ZTranslate(-4.5) * XTranslate(4.5) * YTranslate(0.5) * Scale(0.1);
    DrawLightSource(EmiterProgram, Sphere, Camera, WorldSpace);
    v4 LightSource = WorldSpace * V4(0, 0, 0, 1);

    // Sphere
    WorldSpace = ZTranslate(-4.5);
    DrawPolygonMesh(GLProgram, Sphere, Camera, LightSource, WorldSpace);

    // Cube
    WorldSpace = ZTranslate(-3.5) * XTranslate(-1.5) *ZRotate(Time) * YRotate(Time);
    DrawPolygonMesh(GLProgram, Cube, Camera, LightSource, WorldSpace, BLUE);
#if 0
    // BRDM
    WorldSpace = XRotate(Time) * ZTranslate(-15) * XTranslate(-190) * YTranslate(-80);
    DrawPolygonMesh(GLProgram, BRDM, Camera, LightSource, WorldSpace, OLIVE);
#endif
    PrintLine(Camera->Position);
    PrintLine(Input.Mouse.Wheel);
}
