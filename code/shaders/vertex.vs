#version 110

attribute vec4 aPosition;
attribute vec3 aNormal;
attribute vec2 aTexCoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

varying vec2 TexCoord;
varying vec3 Normal;

void main()
{
    mat4 MVP = Projection * View * Model;
    TexCoord = aTexCoord;
    Normal = aNormal;
    
    gl_Position = MVP * aPosition;
}
