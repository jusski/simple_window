#version 110

attribute vec4 aPosition;
attribute vec2 aTexCoord;

varying vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = aPosition;
}
