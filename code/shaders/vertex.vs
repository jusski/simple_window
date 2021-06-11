#version 110

attribute vec4 aPosition;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    mat4 MVP = Projection * View * Model;
    gl_Position = View * Model * aPosition;
}
