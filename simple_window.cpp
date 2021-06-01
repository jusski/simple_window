#include "pch.h"
const char g_szClassName[] = "myWindowClass";

static HDC DeviceContext;
static volatile bool Running = true;

static void
Win32InitializeOpenGL()
{
    //HDC DeviceContext2 = GetDC(Window);
    
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
        printf("Failed to retrieve PixelFormatDescriptor\n");
        exit(0);
    }
    if(!SetPixelFormat(DeviceContext, PixelFormatIndex, &SuggestedPixelFormat))
    {
        printf("Failed to set PixelFormat\n");
        exit(0);
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
            printf("FAILED: Make Current OpenGL Context\n");
            exit(0);
        }
    }
    else
    {
        printf("FAILED: Create OpenGL Rendering Context\n");
        exit(0);
    }
    
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
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
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
    
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = GetModuleHandle(0);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "The title of my window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
        NULL, NULL, GetModuleHandle(0), NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    printf("Creating Window.\n");
    ShowWindow(hwnd, SW_NORMAL);
    UpdateWindow(hwnd);

    DeviceContext = GetDC(hwnd);
    Win32InitializeOpenGL();

    while(Running)
    {
        if(GetMessage(&Msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
        glViewport(0, 0, 300, 200);
        glClearColor(0.5f, 0.5f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        SwapBuffers(DeviceContext);
    }
    printf("Finished.\n");
    return 0;
}
