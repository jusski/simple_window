@echo off
pushd build
ctime.exe -begin ../simple_window.ctm

set CFLAGS=-Od -Zi -FC -GS -W4 /MD /D_CRT_SECURE_NO_WARNINGS -nologo -wd4201 -wd4305 -wd4505
set IMPORTS=user32.lib gdi32.lib opengl32.lib

set INCLUDES=
set SOURCES=../code/engine.cpp

cl /LD %CFLAGS% %INCLUDES%  %SOURCES% %IMPORTS%
 cl %CFLAGS% ../code/simple_window.cpp  %IMPORTS%

ctime.exe -end ../simple_window.ctm %errorlevel%
popd



