#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "platform.h"
#include "engine.h"

#include "helpers.cpp"
#include "opengl.cpp"
#include "math.h"
#include "ray_trace.cpp"

static void
Transform(object3d *Object, m4 ModelMatrix)
{
    Object->WorldSpaceTransform = ModelMatrix;
}

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
    glPointSize(2);
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
DrawPrimitives(opengl_program *OpenGLProgram, point *Primitives, int Count = 1)
{
    glUseProgram(OpenGLProgram->Program);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point) * Count, Primitives, GL_STREAM_DRAW);
    glVertexAttribPointer(OpenGLProgram->Position, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertex), (void*)offsetof(vertex, Position));
    glEnableVertexAttribArray(OpenGLProgram->Position);

    glUniform3f(OpenGLProgram->Color, 1.0f, 1.0f, 0.0f);
    glUniformMatrix4fv(OpenGLProgram->Model, 1, GL_TRUE, (GLfloat *)Identity.E);
    glUniformMatrix4fv(OpenGLProgram->View, 1, GL_TRUE, (GLfloat *)Identity.E);

    glUniformMatrix4fv(OpenGLProgram->Projection, 1, GL_TRUE, (GLfloat *)Identity.E);
    
    glDrawArrays(GL_POINTS, 0, Count);
}

static void
DrawPoint(opengl_program *OpenGLProgram, v3 Point)
{
    Points[0].A.Position = Point;
    DrawPrimitives(OpenGLProgram, Points);
}

static void
DrawScreenQuad(textured_quad_program *OpenGLProgram, GLuint Texture, m4 Model = Identity)
{
    glUseProgram(OpenGLProgram->Program);

    glBindBuffer(GL_ARRAY_BUFFER, OpenGLProgram->VBO);

    glVertexAttribPointer(OpenGLProgram->Position, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
    glEnableVertexAttribArray(OpenGLProgram->Position);

    glVertexAttribPointer(OpenGLProgram->TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(OpenGLProgram->TexCoord);

    glUniform3f(OpenGLProgram->Color, 1.0f, 0.0f, 0.0f);
    glUniformMatrix4fv(OpenGLProgram->Model, 1, GL_TRUE, (GLfloat *)Model.E);
    
    glBindTexture(GL_TEXTURE_2D, Texture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
}

static void
DrawQuad(textured_quad_program *Program, GLuint Texture, int X, int Y, int Width)
{
    float ScaleFactor = (float)Width / ScreenWidth;
    float XOffset = 2.0f*(float)X / ScreenWidth + (ScaleFactor - 1);
    float YOffset = 2.0f*(float)Y / ScreenHeight + (ScaleFactor - 1);
    m4 Model = YTranslate(YOffset) * XTranslate(XOffset) * Scale(ScaleFactor);
    DrawScreenQuad(Program, Texture, Model);
}

static void
DrawPolygonMesh(opengl_program *Program, polygon_mesh *PolygonMesh, camera *Camera,
                v4 LightSource, m4 Model = Identity, v3 Color = BLUE,
                GLuint VertexBufferObject = VBO, GLuint ElementBufferObject = EBO)
{
    glUseProgram(Program->Program);

    vertex *Vertices = PolygonMesh->Vertices;
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
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

    // LightSource //TODO Use parameter InverseTransform
    LightSource = WorldSpaceToObjectSpace(Model) * LightSource;
    glUniform4f(Program->LightSource, LightSource.x, LightSource.y, LightSource.z, LightSource.w);

    // CameraPosition //TODO Use parameter InverseTransform
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
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * PolygonMesh->IndexCount,
                 PolygonMesh->Indices, GL_STATIC_DRAW); 

    glDrawElements(GL_TRIANGLES, PolygonMesh->IndexCount, GL_UNSIGNED_INT, 0); 
}

static void
DrawObject(object3d *Object, camera *Camera, v4 LightSource)
{
    DrawPolygonMesh(Object->Program, Object->Mesh, Camera, LightSource,
                    Object->WorldSpaceTransform, Object->Color, Object->VBO, Object->EBO);
}

static void
DrawLightSource(opengl_program *Program, polygon_mesh *Emiter, camera *Camera,
                m4 Model = Identity, v3 Color = WHITE)
{
    DrawPolygonMesh(Program, Emiter, Camera, V4(0, 0, 0, 0), Model, Color);
}

static object3d *
CreateObject(arena *Arena, polygon_mesh *Mesh, m4 WorldSpaceTransform, v3 Color, opengl_program *Program)
{
    object3d *Object = PushStruct(Arena, object3d);
    Object->Mesh = Mesh;
    Object->WorldSpaceTransform = WorldSpaceTransform;
    Object->InverseTransform = WorldSpaceToObjectSpace(WorldSpaceTransform);
    Object->Color = Color;

    Object->Program = Program;
    glGenBuffers(1, &Object->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, Object->VBO);
    glBufferData(GL_ARRAY_BUFFER, Mesh->VertexCount * sizeof(vertex), Mesh->Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &Object->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Object->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Mesh->IndexCount * sizeof(int), Mesh->Indices, GL_STATIC_DRAW);

    
    // Calculate bounding sphere center and radius
    float MaxSquaredLength = 0.0f;
    vertex *EdgeVertex1 = 0;
    vertex *EdgeVertex2 = 0;
    for(int OuterIndex = 0; OuterIndex < Mesh->VertexCount; ++OuterIndex)
    {
        vertex *A = Mesh->Vertices + OuterIndex;
        for(int InnerIndex = 0; InnerIndex < Mesh->VertexCount; ++InnerIndex)
        {
            vertex *B = Mesh->Vertices + InnerIndex;
            v3 PointsDifference = B->Position - A->Position;
            float SquaredLength = Inner(PointsDifference, PointsDifference);
            if(SquaredLength > MaxSquaredLength)
            {
                MaxSquaredLength = SquaredLength;
                EdgeVertex1 = A;
                EdgeVertex2 = B;
            }
        }
        
    }
    Object->SquaredRadius = Square(sqrtf(MaxSquaredLength) / 2.0f);
    Object->Origin = EdgeVertex1->Position + 0.5f * (EdgeVertex2->Position - EdgeVertex1->Position);
    
    return(Object);
}

static void
CreateTestTexture(int Width, int Height)
{
    unsigned int *Image = (unsigned int *)malloc(Width * Height * sizeof(unsigned int));
    unsigned int *Pixel = Image;
    for(int Y = 0; Y < Height; ++Y)
    {
        unsigned int Color = Y % 2 ? 0 : -1;
        for(int X = 0; X < Width; ++X)
        {
            *Pixel++ = Color;
        }
    }

    glGenTextures(1, &TestTexture);
    glBindTexture(GL_TEXTURE_2D, TestTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D); 

    glBindTexture(GL_TEXTURE_2D, 0);
    free(Image);
}

static void
CreateTestTexture128(int Width, int Height)
{
    unsigned int *Image = (unsigned int *)malloc(Width * Height * sizeof(unsigned int));
    unsigned int *Pixel = Image;
    for(int Y = 0; Y < Height; ++Y)
    {
        for(int X = 0; X < Width; ++X)
        {
            *Pixel++ = 0x80808080;
        }
    }

    glGenTextures(1, &TestTexture128);
    glBindTexture(GL_TEXTURE_2D, TestTexture128);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D); 

    glBindTexture(GL_TEXTURE_2D, 0);
    free(Image);
}

static void
Initialize(arena *Arena)
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
        Cylinder = PushStruct(Arena, polygon_mesh);
        *Cylinder = LoadModel(Arena, "../code/models/cylinder.ply");
        BRDM = PushStruct(Arena, polygon_mesh);
        *BRDM = LoadModelSTL(Arena, "../code/models/BRDM.stl");

        // Create OpenGL Program
        
        GLProgram = PushStruct(Arena, opengl_program);
        *GLProgram = CreateGenericProgram("../code/shaders/vertex.vs", "../code/shaders/fragment.fs");
        EmiterProgram = PushStruct(Arena, opengl_program);
        *EmiterProgram = CreateGenericProgram("../code/shaders/vertex.vs", "../code/shaders/emiter.fs");
        TexturedQuadProgram = PushStruct(Arena, textured_quad_program);
        *TexturedQuadProgram = CreateTexturedQuadProgram("../code/shaders/textured_quad.vs", "../code/shaders/textured_quad.fs");
        PostProcessProgram = PushStruct(Arena, textured_quad_program);
        *PostProcessProgram = CreateTexturePostProcessProgram("../code/shaders/postprocessing.vs", "../code/shaders/postprocessing.fs");
        

        // TODO move to object
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        GameState = {};
        GameState.Camera.Position = 0.9f * V3(0, 0, 0);
        GameState.Camera.Direction = V3(0, 0, -1);
        GameState.Camera.UpDirection = V3(0, 1, 0);
        
        Projection = Perspective(1, 1, 1, 500);


        CubeObject = CreateObject(Arena, Cube, Identity, CORAL, GLProgram);
        SphereObject = CreateObject(Arena, Sphere, YTranslate(-4.5), BLUE, GLProgram);
        
        Objects3d[ObjectCount++] = CubeObject;
        Objects3d[ObjectCount++] = SphereObject;
        Objects3d[ObjectCount++] = CreateObject(Arena, Sphere, Identity, BLACK, GLProgram);

        // Test Texture
        CreateTestTexture(ScreenWidth, ScreenHeight);
        CreateTestTexture128(ScreenWidth, ScreenHeight);
        
}

static void
ProcessInput(input_state *InputState)
{
    Input = *InputState;
    camera *Camera = &GameState.Camera;

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
    if(InputState->Mouse.RButton == true)
    {
        if(SelectedObject == 0)
        {
            SelectedObject = RayTrace(Camera->Position, Camera->Direction, Objects3d[0], ObjectCount);
            if(SelectedObject)
            {
                SelectedObjectColor = SelectedObject->Color;
                SelectedObject->Color = WHITE;
                v3 ObjectOrigin = (SelectedObject->WorldSpaceTransform * V4(SelectedObject->Origin, 1)).xyz;
                Camera->Direction = ObjectOrigin - Camera->Position;
            }        
        }
        
    }
    else
    {
        if(SelectedObject != 0)
        {
            SelectedObject->Color = SelectedObjectColor;
            SelectedObject = 0;
        }
    }
        
    v3 CameraXAxis = NOZ(Cross(Camera->Direction, Camera->UpDirection));
    v3 CameraYAxis = NOZ(Cross(CameraXAxis, Camera->Direction));
    Camera->UpDirection = CameraYAxis;

    if(InputState->Mouse.LButton == false && SelectedObject == 0)
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
    if(SelectedObject)
    {
        v3 ObjectOrigin = (SelectedObject->WorldSpaceTransform  * V4(SelectedObject->Origin, 1)).xyz;
        Camera->Direction =  ObjectOrigin - Camera->Position;
    }

}

static void DrawScene(camera *Camera)
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(BrightColorExtractionProgram.Program);

    // Draw Sun
    m4 WorldSpace = ZRotate(9*Time) * XTranslate(-1.5) * Scale(0.5);
    DrawLightSource(EmiterProgram, Sphere, Camera, WorldSpace);
    v4 LightSource = WorldSpace * V4(0, 0, 0, 1);

    // Draw Objects TODO static or pointers objects?
    Transform(CubeObject, ZTranslate(-3.5) * XTranslate(-1.5) *ZRotate(Time) * YRotate(Time));
    Transform(SphereObject, ZRotate(Time) * XTranslate(4));

    for(int Index = 0; Index < ObjectCount; ++Index)
    {
        DrawObject(Objects3d[Index], Camera, LightSource);
    }

    // Crosshair
    //DrawPoint(EmiterProgram, V3(0, 0, 0));
    
}

static void
RenderBloomEffect(camera *Camera)
{
    GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    // Bright Color extraction
    glBindFramebuffer(GL_FRAMEBUFFER, BrightColorExtractionFBO);
    glDrawBuffers(2, DrawBuffers);
    DrawScene(Camera);

    // Gausian Blur
    glUseProgram(GausianBlurProgram.Program);
    glDrawBuffers(1, DrawBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, PingPongFBO[1]);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBindBuffer(GL_ARRAY_BUFFER, GausianBlurProgram.VBO);
    glVertexAttribPointer(GausianBlurProgram.Position, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
    glEnableVertexAttribArray(GausianBlurProgram.Position);
    glVertexAttribPointer(GausianBlurProgram.TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(GausianBlurProgram.TexCoord);
    //glUniform1i(GausianBlurProgram.Horizontal, 1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#if 0
    GLuint Texture = ColorBuffer[1];
    int Loop = 2;
    int CurrentFBO = 1;
    int Horizontal = 1;
    //while(Loop-->0)
    {
        //glDrawBuffers(1, DrawBuffers);
    
        glBindTexture(GL_TEXTURE_2D, Texture);
        glBindFramebuffer(GL_FRAMEBUFFER, PingPongFBO[0]);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        //GL_TEXTURE_2D, PingPongColorBuffer[0], 0);
        //glDisable(GL_DEPTH_TEST);
        //glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        glUniform1i(GausianBlurProgram.Horizontal, Horizontal);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        Texture = PingPongColorBuffer[1];
        CurrentFBO = 1 - CurrentFBO;
        Horizontal = 1 - Horizontal;
    }
#endif
#if 0
    glBindTexture(GL_TEXTURE_2D, 0);

    // Render Texture to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    
    glUseProgram(BlendTexturesProgram.Program);

    glBindBuffer(GL_ARRAY_BUFFER, BlendTexturesProgram.VBO);

    glVertexAttribPointer(BlendTexturesProgram.Position, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
    glEnableVertexAttribArray(BlendTexturesProgram.Position);

    glVertexAttribPointer(BlendTexturesProgram.TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(BlendTexturesProgram.TexCoord);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ColorBuffer[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, PingPongColorBuffer[0]);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glActiveTexture(GL_TEXTURE0);
#endif
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawScreenQuad(TexturedQuadProgram, PingPongColorBuffer[1]);
    
}

static void
Render(camera *Camera)
{
    glViewport(0, 0, ScreenWidth, ScreenHeight);

    RenderBloomEffect(Camera);
}

static void
InitBloomEffect()
{
    GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    
    // bright color extraction
    BrightColorExtractionProgram = CreateBrightColorExtractionProgram("../code/shaders/bright_color_extraction.vs", "../code/shaders/bright_color_extraction.fs");
    glGenFramebuffers(1, &BrightColorExtractionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, BrightColorExtractionFBO);
    glGenTextures(2, ColorBuffer);
    for(int ColorBufferIndex = 0; ColorBufferIndex < 2; ++ColorBufferIndex)
    {
        glBindTexture(GL_TEXTURE_2D, ColorBuffer[ColorBufferIndex]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ScreenWidth, ScreenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + ColorBufferIndex,
                               GL_TEXTURE_2D, ColorBuffer[ColorBufferIndex], 0);
    }
    
    glDrawBuffers(2, DrawBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Gausian Blur 2 ping pong framebuffers
    GausianBlurProgram = CreateGausianBlurProgram("../code/shaders/gausian_blur.vs", "../code/shaders/gausian_blur.fs");
    glGenFramebuffers(1, PingPongFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, PingPongFBO[0]);
    glGenTextures(2, PingPongColorBuffer);
    for(int Index = 0; Index < 1; ++Index)
    {
        glBindTexture(GL_TEXTURE_2D, PingPongColorBuffer[Index]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ScreenWidth, ScreenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, PingPongColorBuffer[Index], 0);
        glDrawBuffers(1, DrawBuffers);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Blending both textures
    BlendTexturesProgram = CreateBlendTexturesProgram("../code/shaders/blend_textures.vs", "../code/shaders/blend_textures.fs");
}

extern "C" __declspec(dllexport) void
GameLoop(input_state *InputState)
{
    arena *Arena = &PersistentArena;

    Time += 0.01f;

    if(!Initialized)
    {
        Initialize(Arena);
        InitBloomEffect();
        Initialized = true;
    }
    // Process Input and Camera
    ProcessInput(InputState);
    
    //Rendering Starts Here
    Render(&GameState.Camera);
}
