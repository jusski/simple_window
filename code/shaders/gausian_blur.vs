#version 110

attribute vec3 aPosition;
attribute vec2 aTexCoord;

varying vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = vec4(aPosition, 1.0);
}
