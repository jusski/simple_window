#pragma once

#define KiloBytes(N) 1024L * N
#define Megabytes(N) 1024L * KiloBytes(N)
#define Assert(Statement) if(!(Statement)) __debugbreak();
#define ArrayCount(A) sizeof(A) / sizeof(A[0])
#define Min(A, B) A < B ? A : B
#define Max(A, B) A > B ? A : B
#define Pi 3.14159265359f


struct arena
{
    unsigned char *Memory;
    unsigned int Index;
    unsigned int MaxIndex;
};

#define PushStruct(Arena, Struct) (Struct *) PushSize(Arena, sizeof(Struct))
#define PushArray(Arena, Struct, Count) (Struct *) PushSize(Arena, sizeof(Struct) * Count)

static void *
PushSize(arena *Arena, unsigned int Size)
{
    Assert(Arena);
    Assert((Arena->Index + Size) < Arena->MaxIndex);
    void *Result = Arena->Memory + Arena->Index;
    Arena->Index += Size;

    return(Result);
}

static void
PushInt(arena *Arena, int Value)
{
    int *Result = PushStruct(Arena, int);
    *Result = Value;
}


struct keyboard
{
    bool Left;
    bool Right;
    bool Up;
    bool Down;
};

struct mouse
{
    float XOffset;
    float YOffset;
};

typedef void (type_print)(const char *, ...);
type_print *PlatformPrint;

struct input_state
{
    keyboard Keyboard;
    mouse Mouse;
    int MouseXOffset;
    int MouseYOffset;

    bool PolygonMode;
};


typedef void *(type_wglGetProcAddress)(const char *);
typedef void (type_initialize_opengl)(type_wglGetProcAddress *);
typedef void (type_game_loop)(input_state *InputState, type_print *);

