#include "opengl.h"
#include "engine.h"
#include "math.h"
#include <string.h>

m4 Identity =
{
    V4(1, 0, 0, 0),
    V4(0, 1, 0, 0),
    V4(0, 0, 1, 0),
    V4(0, 0, 0, 1)
};
    
static triangle
Triangle(v3 A, v3 B, v3 C)
{
    triangle Result = {A, B, C};
    return(Result);
}

static void
PushVertex(arena *Arena, vertex Value)
{
    vertex *Vertex = PushStruct(Arena, vertex);
    *Vertex = Value;
}

static void
PushTriangle(arena *Arena, triangle Value)
{
    triangle *T = PushStruct(Arena, triangle);
    *T = Value;
}

static char *
ReadWholeFile(char *Path)
{
    char *Result = 0;
    FILE *File = fopen(Path, "rb") ;
    if(File)
    {
        fseek(File, 0, SEEK_END);
        int FileSize = ftell(File);
        fseek(File, 0, SEEK_SET);

        if(FileSize)
        {
            char *Data = (char *)malloc(FileSize + 1);
            if(Data)
            {
                size_t ReadCount = fread(Data, 1, FileSize, File);
                if(ReadCount == FileSize)
                {
                    Data[FileSize] = 0;
                    Result = Data;
                }
            }
        }
        fclose(File);
    }
    return(Result);
}

static unsigned char *
ReadWholeFileBinary(char *Path)
{
    unsigned char *Result = 0;
    FILE *File = fopen(Path, "rb") ;
    if(File)
    {
        fseek(File, 0, SEEK_END);
        int FileSize = ftell(File);
        fseek(File, 0, SEEK_SET);

        if(FileSize)
        {
            unsigned char *Data = (unsigned char *)malloc(FileSize);
            if(Data)
            {
                size_t ReadCount = fread(Data, 1, FileSize, File);
                if(ReadCount == FileSize)
                {
                    Result = Data;
                }
            }
        }
        fclose(File);
    }
    return(Result);
}

static bool
IsSpace(char Char)
{
    bool Result = ((Char == ' ') ||
                   (Char == '\r') ||
                   (Char == '\t') ||
                   (Char == '\n'));
    return(Result);
}

static bool
IsNotSpace(char Char)
{
    return(!IsSpace(Char));
}

static void
SkipSpaces(char **Cursor)
{
    while(IsSpace(*Cursor[0]))
    {
        *Cursor += 1;
    }
}

static void
SkipUntilSpace(char **Cursor)
{
    while(IsNotSpace(*Cursor[0]) && *Cursor[0] != '\0')
    {
        *Cursor += 1;    
    } 
}

static bool StringsMatch(char *First, char *Second)
{
    bool Result = false;

    while(*First == *Second)
    {
        First++;
        Second++;
    }
    if(*First == 0 || *Second == 0)
    {
        Result = true;
    }
    
    return(Result);
}

static int
StringLength(char *String)
{
    int Result = 0;

    while(*String++ != '\0')
    {
        Result++;
    }
    
    return(Result);
}

static bool
SearchForward(char **Cursor, char *Pattern)
{
    bool Result = false;

    for(;;)
    {
        while(*Cursor[0] != Pattern[0])
        {
            if(*Cursor[0] != '\0')
            {
                *Cursor += 1;    
            }
            else
            {
                break;
            }
            
        }
        if(StringsMatch(*Cursor, Pattern))
        {
            Result = true;
            break;
        }
        else
        {
            *Cursor += 1;
        }
    }

    return(Result);
}

static void
SearchForwardAndSkip(char **Cursor, char *Pattern)
{
    if(SearchForward(Cursor, Pattern))
    {
        *Cursor += StringLength(Pattern);
    }
}

static float
ReadFloat(char **Cursor)
{
    
    SkipSpaces(Cursor);
    float Result = (float)atof(*Cursor);
    SkipUntilSpace(Cursor);

    return(Result);
}

static int
ReadInt(char **Cursor)
{
    
    SkipSpaces(Cursor);
    int Result = atoi(*Cursor);
    SkipUntilSpace(Cursor);

    return(Result);
}

enum parse_state
{
    InvalidState,
    ParseState_NewLine,
    ParseState_EndOfFile,
    ParseState_Reading,
};

static polygon_mesh
LoadMeshPLY(arena *Arena, char *FileContents)
{
    Assert(StringsMatch(FileContents, "ply"));

    char *Cursor = FileContents;
    Assert(SearchForward(&Cursor, "format ascii 1.0"))

    SearchForwardAndSkip(&Cursor, "element vertex");
    int VertexCount = ReadInt(&Cursor);

    SearchForwardAndSkip(&Cursor, "element face");
    int FaceCount = ReadInt(&Cursor);
    
    SearchForwardAndSkip(&Cursor, "end_header");

    polygon_mesh Result = {};
    Result.Vertices = (vertex *)PushSize(Arena, 0);
    Result.VertexCount = VertexCount;
    for(int Count = 0; Count < VertexCount; ++Count)
    {
        vertex *Vertex = PushStruct(Arena, vertex);

        Vertex->Position.x = ReadFloat(&Cursor);
        Vertex->Position.y = ReadFloat(&Cursor);
        Vertex->Position.z = ReadFloat(&Cursor);

        Vertex->Normal.x = ReadFloat(&Cursor);
        Vertex->Normal.y = ReadFloat(&Cursor);
        Vertex->Normal.z = ReadFloat(&Cursor);

        Vertex->TexCoord.x = ReadFloat(&Cursor);
        Vertex->TexCoord.y = ReadFloat(&Cursor);

        SearchForwardAndSkip(&Cursor, "\n");
    }

    Result.Indices = (int *)PushSize(Arena, 0);
    Result.IndexCount = FaceCount * 3;
    for(int Count = 0; Count < FaceCount; ++Count)
    {
        ReadInt(&Cursor); // Ignored

        PushInt(Arena, ReadInt(&Cursor));
        PushInt(Arena, ReadInt(&Cursor));
        PushInt(Arena, ReadInt(&Cursor));

        SearchForwardAndSkip(&Cursor, "\n");        
    }

    return(Result);
}
    
static int
LoadVerticesOBJ(arena *Arena, char *FileContents)
{
    parse_state State = ParseState_NewLine;
    int VertexCount = 0;

    for(char *Cursor = FileContents; *Cursor != '\0';)
    {
        switch(*Cursor)
        {
            case('\n') :
            {
                if(Cursor[1] != '\0')
                {
                    State = ParseState_NewLine;
                }
                else
                {
                    State = ParseState_EndOfFile;
                }
                Cursor++;
            } break;
            case('v') :
            {
                if(State == ParseState_NewLine &&
                   Cursor[1] == ' ')
                {
                    Cursor += 2;

                    vertex *Vertex = PushStruct(Arena, vertex);
                    Vertex->Position.x = ReadFloat(&Cursor);
                    Vertex->Position.y = ReadFloat(&Cursor);
                    Vertex->Position.z = ReadFloat(&Cursor);
                    VertexCount++;

                    State = ParseState_Reading;
                }
                else
                {
                    Cursor++;
                }
            } break;
            default:
                Cursor++;
        }
    }

    return(VertexCount);
}

#pragma pack(push, 1)
struct vertex_stl
{
    v3 Normal;
    v3 A;
    v3 B;
    v3 C;
    __int16 Ignored;
};
#pragma pack(pop)

static polygon_mesh
LoadMeshSTL(arena *Arena, unsigned char *FileContents)
{
    unsigned char *Cursor = FileContents;
    Cursor += 80; // Skip Header
    int *IntCursor = (int *)Cursor;
    int TriangleCount = *IntCursor;
    Cursor +=4;

    vertex_stl *VertexSTL = (vertex_stl *)Cursor;

    polygon_mesh Result = {};
    Result.Vertices = (vertex *)PushSize(Arena, 0);
    Result.VertexCount = TriangleCount * 3;
    
    for(int Count = 0; Count < TriangleCount; ++Count)
    {
        vertex *Vertex = PushStruct(Arena, vertex);
        Vertex->Position = VertexSTL->A;
        Vertex->Normal = VertexSTL->Normal;

        Vertex = PushStruct(Arena, vertex);
        Vertex->Position = VertexSTL->B;
        Vertex->Normal = VertexSTL->Normal;

        Vertex = PushStruct(Arena, vertex);
        Vertex->Position = VertexSTL->C;
        Vertex->Normal = VertexSTL->Normal;

        VertexSTL++;
    }

    Result.Indices = (int *)PushSize(Arena, 0);
    Result.IndexCount = Result.VertexCount;
    for(int Count = 0; Count < Result.IndexCount; ++Count)
    {
        PushInt(Arena, Count);
    }

    return(Result);
}

static polygon_mesh 
LoadModel(arena *Arena, char *Path)
{
    polygon_mesh Result = {};
    char *FileContents = ReadWholeFile(Path);
    if(FileContents)
    {
        Result = LoadMeshPLY(Arena, FileContents);
        free(FileContents);
    }
    
    return(Result);
}

static polygon_mesh 
LoadModelSTL(arena *Arena, char *Path)
{
    polygon_mesh Result = {};
    unsigned char *FileContents = ReadWholeFileBinary(Path);
    if(FileContents)
    {
        Result = LoadMeshSTL(Arena, FileContents);
        free(FileContents);
    }
    
    return(Result);
}


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
            const char *Type = (ShaderType == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
            fprintf(stderr, "ERROR: %s::SHADER::COMPILATION_FAILED\n%s\n", Type, InfoLog);
        }
        free(ShaderSource);
    }
    return(Result);
}

static opengl_program
CreateOpenGLProgram(const char *VertexShaderSource, const char *FragmentShaderSource)
{
    opengl_program Result = {};

    GLuint VertexShader = CreateShader(GL_VERTEX_SHADER, VertexShaderSource);
    GLuint FragmentShader = CreateShader(GL_FRAGMENT_SHADER, FragmentShaderSource);

    GLuint Program = glCreateProgram();
    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);
    glLinkProgram(Program);

    int Success;
    glGetProgramiv(Program, GL_LINK_STATUS, &Success);
    if(Success)
    {
        Result.Program = Program;
        Result.Position = glGetAttribLocation(Program, "aPosition");
        Result.Normal = glGetAttribLocation(Program, "aNormal");
        Result.TexCoord = glGetAttribLocation(Program, "aTexCoord");


        Result.Color = glGetUniformLocation(Program, "Color");
        Result.Model = glGetUniformLocation(Program, "Model");
        Result.View = glGetUniformLocation(Program, "View");
        Result.Projection = glGetUniformLocation(Program, "Projection");
        Result.LightSource = glGetUniformLocation(Program, "LightSource");
        Result.CameraPosition = glGetUniformLocation(Program, "CameraPosition");
        
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

static opengl_program
CreateOpenGLProgram()
{
    return(CreateOpenGLProgram("../code/shaders/vertex.vs",  "../code/shaders/fragment.fs"));
}
static m4
LookAt(v3 CameraPosition, v3 CameraDirection, v3 Up)
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

static m4
LookAt(camera *Camera)
{
    return(LookAt(Camera->Position, Camera->Direction, Camera->UpDirection));
}


static m4
LookAt(v3 CameraPosition, v3 CameraDirection)
{
    m4 Result = LookAt(CameraPosition, CameraDirection, V3(0, 1, 0));
    return(Result);
}

static m4
Perspective(float l, float r, float t, float b, float n, float f)
{
    m4 Result =
    {
        2 * n / (r - l), 0, (r + l)/(r - l), 0,
        0, 2 * n / (t - b), (t + b)/(t - b), 0,
        0, 0, -(f + n)/(f - n), -2 * f * n/(f - n),
        0, 0, -1, 0
    };
    return(Result);
}

static m4
Perspective(float r, float t, float n, float f)
{
    m4 Result =
    {
        n/r, 0, 0, 0,
        0, n/t, 0, 0,
        0, 0, -(f+n)/(f-n), -2*f*n/(f-n),
        0, 0, -1, 0
    };
    return(Result);
}

static m4
WorldSpaceToObjectSpace(m4 WorldSpace)
{
    v4 R1 = WorldSpace.R1;
    v4 R2 = WorldSpace.R2;
    v4 R3 = WorldSpace.R3;
    v4 R4 = WorldSpace.R4;
    m4 Rotate =
    {
        R1.x, R2.x, R3.x, 0,
        R1.y, R2.y, R3.y, 0,
        R1.z, R2.z, R3.z, 0,
        0,    0,    0,    1
    };
    m4 Translate =
    {
        1, 0, 0, -R1.w,
        0, 1, 0, -R2.w,
        0, 0, 1, -R3.w,
        0, 0, 0, 1
    };

    m4 Result = Rotate * Translate;

    return(Result);
}

static void
OpenGLDriverInfo()
{
    FILE *File;
    File = fopen("opengl_driver_info.txt", "w");
    if(File)
    {
        fprintf(File, "GL_VENDOR %s \n", glGetString(GL_VENDOR));
        fprintf(File, "GL_RENDERER %s \n", glGetString(GL_RENDERER));
        fprintf(File, "GL_VERSION %s \n", glGetString(GL_VERSION));
        fprintf(File, "GL_SHADING_LANGUAGE_VERSION %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        char *Extensions = (char *)glGetString(GL_EXTENSIONS);
        char *Token = strtok(Extensions, " ");
        while(Token != 0)
        {
            fprintf(File, "\n%s", Token );
            Token = strtok(0, " ");
        }
        //fprintf(File, "GL_EXTENSIONS %s \n", glGetString(GL_EXTENSIONS));
        
        fclose(File);
    }
    
}
