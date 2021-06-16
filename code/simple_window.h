#pragma once

type_game_loop *GameLoop;
type_initialize_opengl *InitializeOpenGL;
    
static HDC DeviceContext;
static HANDLE Console;
static COORD ConsoleCursorPosition;

static volatile bool Running = true;
static volatile bool Paused = false;

static volatile int MouseWheel;
static volatile bool MouseLButton;
static volatile bool MouseRButton;
static volatile float XMouseOffset = 0.0f;
static volatile float YMouseOffset = 0.0f;
static int LastXCoordinate = 0;
static int LastYCoordinate = 0;
static bool MouseLeftClick = false;

typedef BOOL WINAPI type_wglChoosePixelFormatARB (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC WINAPI type_wglCreateContextAttribsARB (HDC hDC, HGLRC hShareContext, const int *attribList);

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_ALPHA_BITS_ARB                0x201B
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_SAMPLE_BUFFERS_ARB            0x2041
#define WGL_SAMPLES_ARB                   0x2042

