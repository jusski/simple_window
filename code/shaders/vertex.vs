#version 110

attribute vec3 aPosition;
attribute vec3 aColor;

varying vec3 Color;

uniform float Time;

void main()
{
    Color = aColor;
    //gl_Position = vec4(aPosition, 1.0);
    gl_Position = vec4(aPosition.xyz, 1.0);
}
