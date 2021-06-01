@echo off
ctime.exe -begin simple_window.ctm

set CFLAGS=-Od -nologo  -FC  -W4 -Wall -wd4731 -wd4201 -wd4514 -wd4100 -wd4505 -wd4189 -wd4127 -wd4738 -wd4710 -wd4711 -wd4820 -Oi -GR- -EHa- -Zi -MDd  -D_CRT_SECURE_NO_WARNINGS
set IMPORTS=user32.lib gdi32.lib opengl32.lib

cl %CFLAGS% simple_window.cpp %IMPORTS%

ctime.exe -end simple_window.ctm %errorlevel%



