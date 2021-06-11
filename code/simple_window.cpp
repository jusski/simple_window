#include "pch.h"
#include "platform.h"
//#include "engine.cpp"

type_Engine *Engine;
type_InitializeOpenGL *InitializeOpenGL;
    
static HDC DeviceContext;
static volatile bool Running = true;
static volatile bool Paused = false;
static volatile float XAngle = 0.0f;
static volatile float YAngle = 0.0f;

static int LastXCoordinate = 0;
static int LastYCoordinate = 0;
static bool MouseLeftClick = false;

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
        #if 0
        case(WM_NCHITTEST):
        {
            LRESULT Hit = DefWindowProc(hwnd, msg, wParam, lParam);
            if (Hit == HTCLIENT) Hit = HTCAPTION;
            return Hit;
        } break;
        #endif
        case(WM_LBUTTONUP):
        {
            MouseLeftClick = false;
        } break;
        case(WM_LBUTTONDOWN):
        {
            MouseLeftClick = true;
            LastXCoordinate = lParam & 0xFFFF;
            LastYCoordinate = (lParam >> 16) & 0xFFFF;
        } break;
        case(WM_MOUSEMOVE):
        {
            if(MouseLeftClick)
            {
                int XCoordinate = lParam & 0xFFFF;
                int YCoordinate = (lParam >> 16) & 0xFFFF;
                XAngle += 0.01f*(XCoordinate - LastXCoordinate);
                YAngle += 0.01f*(YCoordinate - LastYCoordinate);
                LastXCoordinate = XCoordinate;
                LastYCoordinate = YCoordinate;
            }
        } break;
        case(WM_KEYDOWN):
        {
            if (wParam == VK_ESCAPE)
            {
                Running = false;
            }
            if (wParam == VK_SPACE)
            {
                Paused = true;
            }
        } break;
        case(WM_KEYUP):
        {
            if(wParam == VK_SPACE)
            {
                Paused = false;
            }
        } break;
        case(WM_CLOSE):
        {
            DestroyWindow(hwnd);
            Running = false;
        } break;
        case(WM_DESTROY):
        {
            PostQuitMessage(0);
            Running = false;
        } break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

static void
ProcessKeyboardInput(input_state *InputState)
{
    MSG Message;
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE) > 0)
    {
        switch(Message.message)
        {
            case(WM_KEYUP):
            case(WM_KEYDOWN):
            case(WM_SYSKEYDOWN):
            case(WM_SYSKEYUP):
            {
                unsigned int KeyCode = (unsigned int)Message.wParam;
                bool IsDown = ((Message.lParam & (1 << 31)) == 0);
                //bool WasDown =  ((Message.lParam & (1 << 30)) != 0);
           
                if(KeyCode == 'K' && IsDown)
                {
                    InputState->Keyboard.Left = true;
                }
                if(KeyCode == 'O' && IsDown)
                {
                    InputState->Keyboard.Up = true;
                }
                if(KeyCode == 'L' && IsDown)
                {
                    InputState->Keyboard.Down = true;
                }
                if(KeyCode == VK_OEM_1 && IsDown)
                {
                    InputState->Keyboard.Right = true;
                }    
                
                
            } 
            default:
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);        
            }
        }
        
    }
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
    WindowClass.hbrBackground = 0;
    WindowClass.lpszMenuName  = NULL;
    WindowClass.lpszClassName = "SimpleOpenGLWindow";
    WindowClass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&WindowClass))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    Window = CreateWindowEx(0,
                            WindowClass.lpszClassName,
                            "Simple OpenGL Window",
                            WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
                            NULL, NULL, GetModuleHandle(0), NULL);
    SetWindowLong(Window, GWL_EXSTYLE, 0);
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
            input_state InputState = {};
            ProcessKeyboardInput(&InputState);
            if(!Paused)
            {
                Engine(&InputState, XAngle, YAngle);
                Sleep(30);
                SwapBuffers(DeviceContext);
                
            }
        }
    }
    else
    {
        printf("[ERROR] Failed to create window.\n");
    }
    printf("Finished.\n");
    return 0;
}
