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


struct keyboard
{
    bool Left;
    bool Right;
    bool Up;
    bool Down;
};

struct input_state
{
    keyboard Keyboard;
    int MouseXOffset;
    int MouseYOffset;
};


typedef void *(type_wglGetProcAddress)(const char *);
typedef void (type_InitializeOpenGL)(type_wglGetProcAddress *);
typedef void (type_Engine)(input_state *InputState, float, float);
