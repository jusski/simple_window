#include "pch.h"
//#include "engine.cpp"

typedef void *(type_wglGetProcAddress)(const char *);
typedef void (type_InitializeOpenGL)(type_wglGetProcAddress *);
typedef void (type_Engine)(void);

type_Engine *Engine;
type_InitializeOpenGL *InitializeOpenGL;
    
static HDC DeviceContext;
static volatile bool Running = true;

static void
Win32InitializeOpenGL()
{
    PIXELFORMATDESCRIPTOR DesiredPixelFormat = {};
    DesiredPixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    DesiredPixelFormat.nVersion = 1;
    DesiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    DesiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
    DesiredPixelFormat.cColorBits = 24;
    DesiredPixelFormat.cAlphaBits = 8;
    DesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
    
    int PixelFormatIndex = ChoosePixelFormat(DeviceContext,
                                             &DesiredPixelFormat);

    PIXELFORMATDESCRIPTOR SuggestedPixelFormat = {};
    PixelFormatIndex = 6;
    if(!DescribePixelFormat(DeviceContext, PixelFormatIndex,
                            sizeof(PIXELFORMATDESCRIPTOR), &SuggestedPixelFormat))
    {
        fprintf(stderr, "[ERROR] Failed to retrieve PixelFormatDescriptor\n");
    }
    if(!SetPixelFormat(DeviceContext, PixelFormatIndex, &SuggestedPixelFormat))
    {
        fprintf(stderr, "[ERROR] Failed to set PixelFormat\n");
    }

    HGLRC RenderingContext = wglCreateContext(DeviceContext);
    if (RenderingContext)
    {
        if(wglMakeCurrent(DeviceContext, RenderingContext))
        {
            printf("Creating OpenGL rendering context\n");
        }
        else
        {
            fprintf(stderr,"[ERROR] Failed to set OpenGL Context\n");
        }
    }
    else
    {
        fprintf(stderr, "[ERROR] Create OpenGL Rendering Context\n");
    }
    
}

POINT Cursor;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_NCHITTEST: {
            LRESULT Hit = DefWindowProc(hwnd, msg, wParam, lParam);
            if (Hit == HTCLIENT) Hit = HTCAPTION;
            return Hit;
        } break;
        case WM_KEYDOWN:
        {
            if (wParam == VK_ESCAPE)
            {
                Running = false;
            }
        } break;
        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            Running = false;
        } break;
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            Running = false;
        } break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int main()
{
    HMODULE Library = LoadLibrary("engine.dll");
    Engine = (type_Engine *)GetProcAddress(Library, "Engine");
    InitializeOpenGL = (type_InitializeOpenGL *)GetProcAddress(Library, "InitializeOpenGL");

    WNDCLASSEX WindowClass = {};
    HWND Window;
    
    WindowClass.cbSize        = sizeof(WNDCLASSEX);
    WindowClass.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc   = WndProc;
    WindowClass.cbClsExtra    = 0;
    WindowClass.cbWndExtra    = 0;
    WindowClass.hInstance     = GetModuleHandle(0);
    WindowClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    WindowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    WindowClass.hbrBackground = 0; //(HBRUSH)(COLOR_WINDOW+1);
    WindowClass.lpszMenuName  = NULL;
    WindowClass.lpszClassName = "SimpleOpenGLWindow";
    WindowClass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&WindowClass))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    Window = CreateWindowEx(WS_EX_CLIENTEDGE,
                            WindowClass.lpszClassName,
                            "Simple OpenGL Window",
                            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                            NULL, NULL, GetModuleHandle(0), NULL);

    if(Window)
    {
        printf("Creating Window.\n");
        ShowWindow(Window, SW_NORMAL);
        UpdateWindow(Window);

        DeviceContext = GetDC(Window);
        Win32InitializeOpenGL();
        InitializeOpenGL((type_wglGetProcAddress *)GetProcAddress(LoadLibrary("opengl32.dll"), "wglGetProcAddress"));
        
        while(Running)
        {
            MSG Msg;
            if(PeekMessage(&Msg, 0, 0, 0, PM_REMOVE) > 0)
            {
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            }
            Engine();
            Sleep(30);
            SwapBuffers(DeviceContext);
        }
    }
    else
    {
        printf("[ERROR] Failed to create window.\n");
    }
    printf("Finished.\n");
    return 0;
}
