#include "pch.h"
#include "platform.h"
//#include "engine.cpp"
#include "simple_window.h"

static void
Print(const char *Message, ...)
{
    SetConsoleCursorPosition(Console, ConsoleCursorPosition);
    va_list vl;
    va_start(vl, Message);
    vprintf(Message, vl);
    va_end(vl);
    ConsoleCursorPosition.Y += 1;
}


static void
Print(short Row, const char *Message, ...)
{
    COORD Coord = {};
    Coord.Y = Row;
    SetConsoleCursorPosition(Console, Coord);
    va_list vl;
    va_start(vl, Message);
    vprintf(Message, vl);
    va_end(vl);
}

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
        if(!wglMakeCurrent(DeviceContext, RenderingContext))
        {
            fprintf(stderr,"[ERROR] Failed to set OpenGL Context\n");
        }
    }
    else
    {
        fprintf(stderr, "[ERROR] Create OpenGL Rendering Context\n");
    }
    
}


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
        case(WM_MOUSEWHEEL):
        {
            MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam);
        } break;
        case(WM_LBUTTONDOWN):
        {
            MouseLButton = true;
        } break;
        case(WM_LBUTTONUP):
        {
            MouseLButton = false;
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
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case(WM_KEYUP):
            {
                unsigned int KeyCode = (unsigned int)Message.wParam;
                //bool IsDown = ((Message.lParam & (1 << 31)) == 0);
                //bool WasDown =  ((Message.lParam & (1 << 30)) != 0);

                if(KeyCode == 'K' || KeyCode == 'A' )
                {
                    InputState->Keyboard.Left = false;
                }
                if(KeyCode == 'O' || KeyCode == 'W' )
                {
                    InputState->Keyboard.Up = false;
                }
                if(KeyCode == 'L' || KeyCode == 'S' )
                {
                    InputState->Keyboard.Down = false;
                }
                if(KeyCode == VK_OEM_1 || KeyCode == 'D' )
                {
                    InputState->Keyboard.Right = false;
                }
                if(KeyCode == VK_SPACE)
                {
                    Paused = false;
                }
            } break; 
            case(WM_KEYDOWN):
            {
                unsigned int KeyCode = (unsigned int)Message.wParam;
                //bool IsDown = ((Message.lParam & (1 << 31)) == 0);
                //bool WasDown =  ((Message.lParam & (1 << 30)) != 0);
                if(KeyCode == 'K' || KeyCode == 'A' )
                {
                    InputState->Keyboard.Left = true;
                }
                if(KeyCode == 'O' || KeyCode == 'W' )
                {
                    InputState->Keyboard.Up = true;
                }
                if(KeyCode == 'L' || KeyCode == 'S' )
                {
                    InputState->Keyboard.Down = true;
                }
                if(KeyCode == VK_OEM_1 || KeyCode == 'D' )
                {
                    InputState->Keyboard.Right = true;
                }
                
                if (KeyCode == VK_ESCAPE)
                {
                    Running = false;
                }
                if (KeyCode == VK_SPACE)
                {
                    Paused = true;
                }
                if (KeyCode == 'P')
                {
                    InputState->PolygonMode = true;
                }
                
            } break;
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
    Console = GetStdHandle(STD_OUTPUT_HANDLE);

    HMODULE Library = LoadLibrary("engine.dll");
    GameLoop = (type_game_loop *)GetProcAddress(Library, "GameLoop");
    export_functions *ExportFunctions = (export_functions *)GetProcAddress(Library, "ExportFunctions");

    exported_functions Functions = {};
    Functions.PlatformPrint = Print;
    Functions.wglGetProcAddress = (type_wglGetProcAddress *)GetProcAddress(LoadLibrary("opengl32.dll"), "wglGetProcAddress");
    ExportFunctions(Functions);

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
        return(0);
    }

    Window = CreateWindowEx(0,
                            WindowClass.lpszClassName,
                            "Simple OpenGL Window",
                            WS_POPUP, 500, 200, 500, 500,
                            NULL, NULL, GetModuleHandle(0), NULL);
    if(Window)
    {
        ShowWindow(Window, SW_NORMAL);
        UpdateWindow(Window);

        DeviceContext = GetDC(Window);
        Win32InitializeOpenGL();
        
        ShowCursor(false);
        SetCursorPos(600, 300);
        while(Running)
        {
            ConsoleCursorPosition.Y = 0;
            input_state InputState = {};
            ProcessKeyboardInput(&InputState);

            POINT CursorPosition;
            GetCursorPos(&CursorPosition);
            SetCursorPos(600,300);
            InputState.Mouse.XOffset = 0.01f * (CursorPosition.x - 600);
            InputState.Mouse.YOffset = -0.01f * (CursorPosition.y - 300);
            InputState.Mouse.Wheel = MouseWheel / 120;
            InputState.Mouse.LButton = MouseLButton;
            MouseWheel = 0;
            if(!Paused)
            {
                GameLoop(&InputState);
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
