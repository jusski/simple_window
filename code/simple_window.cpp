#include "pch.h"
#include "platform.h"
//#include "engine.cpp"
#include "simple_window.h"

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
                XMouseOffset += 0.01f*(XCoordinate - LastXCoordinate);
                YMouseOffset += 0.01f*(LastYCoordinate - YCoordinate);
                LastXCoordinate = XCoordinate;
                LastYCoordinate = YCoordinate;
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
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case(WM_KEYUP):
            {
                unsigned int KeyCode = (unsigned int)Message.wParam;
                //bool IsDown = ((Message.lParam & (1 << 31)) == 0);
                //bool WasDown =  ((Message.lParam & (1 << 30)) != 0);

                if(KeyCode == 'K')
                {
                    InputState->Keyboard.Left = false;
                }
                if(KeyCode == 'O')
                {
                    InputState->Keyboard.Up = false;
                }
                if(KeyCode == 'L')
                {
                    InputState->Keyboard.Down = false;
                }
                if(KeyCode == VK_OEM_1)
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

                if(KeyCode == 'K')
                {
                    InputState->Keyboard.Left = true;
                }
                if(KeyCode == 'O')
                {
                    InputState->Keyboard.Up = true;
                }
                if(KeyCode == 'L')
                {
                    InputState->Keyboard.Down = true;
                }
                if(KeyCode == VK_OEM_1)
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
    HMODULE Library = LoadLibrary("engine.dll");
    GameLoop = (type_game_loop *)GetProcAddress(Library, "GameLoop");
    InitializeOpenGL = (type_initialize_opengl *)GetProcAddress(Library, "InitializeOpenGL");

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
                            WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
                            NULL, NULL, GetModuleHandle(0), NULL);
    if(Window)
    {
        printf("Creating Window.\n");
        ShowWindow(Window, SW_NORMAL);
        UpdateWindow(Window);

        DeviceContext = GetDC(Window);
        Win32InitializeOpenGL();
        InitializeOpenGL((type_wglGetProcAddress *)GetProcAddress(LoadLibrary("opengl32.dll"), "wglGetProcAddress"));

        ShowCursor(false);
        RECT Clip;
        GetWindowRect(Window, &Clip);
        ClipCursor(&Clip);
            
        while(Running)
        {
            input_state InputState = {};
            ProcessKeyboardInput(&InputState);
            InputState.Mouse.XOffset = XMouseOffset;
            InputState.Mouse.YOffset = YMouseOffset;
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
