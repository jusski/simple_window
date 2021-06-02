#pragma once
GLuint VBO;
GLuint Program = 0;

float vertices[] =
{
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f,  0.5f, 0.0f
};

const char *VertexShaderSource =
"#version 110 \n"
"attribute vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *FragmentShaderSource =
"#version 110 \n"
"void main()\n"
"{\n"
"   gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
"}\0";

