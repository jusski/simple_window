#include "opengl.h"
#include "engine.h"

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

static int
LoadVerticesPLY(arena *Arena, char *FileContents)
{
    Assert(StringsMatch(FileContents, "ply"));

    char *Cursor = FileContents;
    Assert(SearchForward(&Cursor, "format ascii 1.0"))

    SearchForwardAndSkip(&Cursor, "element vertex");
    int VertexCount = ReadInt(&Cursor);
    SearchForwardAndSkip(&Cursor, "end_header");
    
    for(int Count = 0; Count < VertexCount; ++Count)
    {
        vertex *Vertex = PushStruct(Arena, vertex);
        Vertex->x = ReadFloat(&Cursor);
        Vertex->y = ReadFloat(&Cursor);
        Vertex->z = ReadFloat(&Cursor);

        SearchForwardAndSkip(&Cursor, "\n");
    }

    return(VertexCount);
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
                    Vertex->x = ReadFloat(&Cursor);
                    Vertex->y = ReadFloat(&Cursor);
                    Vertex->z = ReadFloat(&Cursor);
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
    vertex Normal;
    vertex A;
    vertex B;
    vertex C;
    __int16 Ignored;
};
#pragma pack(pop)

static int
LoadTrianglesSTL(arena *Arena, unsigned char *FileContents)
{
    unsigned char *Cursor = FileContents;
    Cursor += 80; // Skip Header
    int *IntCursor = (int *)Cursor;
    int TriangleCount = *IntCursor;
    Cursor +=4;

    vertex_stl *VertexSTL = (vertex_stl *)Cursor;
    for(int Count = 0; Count < TriangleCount; ++Count)
    {
        triangle T = Triangle(VertexSTL->A, VertexSTL->B, VertexSTL->C);
        PushTriangle(Arena, T);
        VertexSTL++;
    }

    return(TriangleCount);
}

static polygon_mesh *
LoadModel(arena *Arena, char *Path)
{
    unsigned char *FileContents = ReadWholeFileBinary(Path);

    polygon_mesh *Result = PushStruct(Arena, polygon_mesh);
    Result->Triangles = (triangle *)PushSize(Arena, 0);
    Result->TriangleCount = LoadTrianglesSTL(Arena, FileContents);
    
    
    free(FileContents);
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

        Model = glGetUniformLocation(Program, "Model");
        View = glGetUniformLocation(Program, "View");
        Projection = glGetUniformLocation(Program, "Projection");
        
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
