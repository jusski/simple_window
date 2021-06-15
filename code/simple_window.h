#pragma once

type_game_loop *GameLoop;
type_initialize_opengl *InitializeOpenGL;
    
static HDC DeviceContext;
static HANDLE Console;
static COORD ConsoleCursorPosition;

static volatile bool Running = true;
static volatile bool Paused = false;

static volatile float XMouseOffset = 0.0f;
static volatile float YMouseOffset = 0.0f;
static int LastXCoordinate = 0;
static int LastYCoordinate = 0;
static bool MouseLeftClick = false;
