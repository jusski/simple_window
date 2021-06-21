#version 110

attribute vec4 aPosition;
attribute vec2 aTexCoord;

uniform mat4 Model;

varying vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = Model * aPosition;
}
