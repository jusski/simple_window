#pragma once

#define APIENTRY __stdcall
#define WINGDIAPI __declspec(dllimport)
#include <gl/gl.h>
typedef void* (type_wglGetProcAddress)(const char *);
type_wglGetProcAddress *wglGetProcAddress;

typedef signed long long int GLsizeiptr;
typedef char GLchar;

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4

typedef void type_glBindBuffer (GLenum target, GLuint buffer);
typedef void type_glGenBuffers (GLsizei n, GLuint *buffers);
typedef void type_glBufferData (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void type_glAttachShader (GLuint program, GLuint shader);
typedef void type_glBindAttribLocation (GLuint program, GLuint index, const GLchar *name);
typedef void type_glCompileShader (GLuint shader);
typedef GLuint type_glCreateProgram (void);
typedef GLuint type_glCreateShader (GLenum type);
typedef void type_glEnableVertexAttribArray (GLuint index);
typedef GLint type_glGetAttribLocation (GLuint program, const GLchar *name);
typedef GLint type_glGetUniformLocation (GLuint program, const GLchar *name);
typedef void type_glLinkProgram (GLuint program);
typedef void type_glShaderSource (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void type_glUseProgram (GLuint program);
typedef void type_glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void type_glValidateProgram (GLuint program);
typedef void type_glGetProgramiv (GLuint program, GLenum pname, GLint *params);
typedef void type_glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void type_glGetShaderiv (GLuint shader, GLenum pname, GLint *params);
typedef void type_glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void type_glDeleteShader (GLuint shader);

#define OpenGLFunction(name) type_##name *##name;
#define GetOpenGLFuncAddress(name) name = (type_##name *)glGetProcAddress(#name);

OpenGLFunction(glBindBuffer)
OpenGLFunction(glGenBuffers)
OpenGLFunction(glBufferData)
OpenGLFunction(glAttachShader)
OpenGLFunction(glBindAttribLocation)
OpenGLFunction(glCompileShader)
OpenGLFunction(glCreateProgram)
OpenGLFunction(glCreateShader)
OpenGLFunction(glEnableVertexAttribArray)
OpenGLFunction(glGetAttribLocation)
OpenGLFunction(glGetUniformLocation)
OpenGLFunction(glLinkProgram)
OpenGLFunction(glShaderSource)
OpenGLFunction(glUseProgram)
OpenGLFunction(glGetShaderiv)
OpenGLFunction(glGetShaderInfoLog)
OpenGLFunction(glGetProgramiv)
OpenGLFunction(glGetProgramInfoLog)
OpenGLFunction(glVertexAttribPointer)
OpenGLFunction(glDeleteShader)

static void
AssignOpenGLFunctions(type_wglGetProcAddress *glGetProcAddress)
{
    GetOpenGLFuncAddress(glBindBuffer);
    GetOpenGLFuncAddress(glGenBuffers);
    GetOpenGLFuncAddress(glBufferData);
    GetOpenGLFuncAddress(glAttachShader);
    GetOpenGLFuncAddress(glBindAttribLocation);
    GetOpenGLFuncAddress(glCompileShader);
    GetOpenGLFuncAddress(glCreateProgram);
    GetOpenGLFuncAddress(glCreateShader);
    GetOpenGLFuncAddress(glEnableVertexAttribArray);
    GetOpenGLFuncAddress(glGetAttribLocation);
    GetOpenGLFuncAddress(glGetUniformLocation);
    GetOpenGLFuncAddress(glLinkProgram);
    GetOpenGLFuncAddress(glShaderSource);
    GetOpenGLFuncAddress(glUseProgram);
    GetOpenGLFuncAddress(glGetShaderiv);
    GetOpenGLFuncAddress(glGetShaderInfoLog);
    GetOpenGLFuncAddress(glGetProgramiv);
    GetOpenGLFuncAddress(glGetProgramInfoLog);
    GetOpenGLFuncAddress(glVertexAttribPointer);
    GetOpenGLFuncAddress(glDeleteShader);
}
