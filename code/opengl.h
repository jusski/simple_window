#pragma once

#define APIENTRY __stdcall
#define WINGDIAPI __declspec(dllimport)
#include <gl/gl.h>

type_wglGetProcAddress *wglGetProcAddress;

typedef signed long long int GLsizeiptr;
typedef char GLchar;

#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_STREAM_DRAW                    0x88E0
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4
#define GL_MAX_VERTEX_ATTRIBS             0x8869
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_DRAW_BUFFERS               0x8824
#define GL_ACTIVE_UNIFORMS                0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH      0x8B87
#define GL_ACTIVE_ATTRIBUTES              0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH    0x8B8A
#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_REPEAT                         0x2901
#define GL_TEXTURE_2D                     0x0DE1
#define GL_FLOAT                          0x1406
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_STENCIL_ATTACHMENT             0x8D20
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RENDERBUFFER                   0x8D41

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
typedef GLint type_glGetAttribLocation (GLuint program, const GLchar *name);
typedef void type_glUniform1f (GLint location, GLfloat v0);
typedef  void type_glUniform1i (GLint location, GLint v0);
typedef void type_glUniform2f (GLint location, GLfloat v0, GLfloat v1);
typedef void type_glUniform3f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void type_glUniform4f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void type_glGenerateMipmap (GLenum target);
typedef void type_glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void type_glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void type_glActiveTexture (GLenum texture);

typedef void type_glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void type_glFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void type_glBlitFramebuffer (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void type_glBindRenderbuffer (GLenum target, GLuint renderbuffer);
typedef void type_glGenRenderbuffers (GLsizei n, GLuint *renderbuffers);
typedef void type_glBindFramebuffer (GLenum target, GLuint framebuffer);
typedef void type_glGenFramebuffers (GLsizei n, GLuint *framebuffers);
typedef GLenum type_glCheckFramebufferStatus (GLenum target);
typedef void type_glDrawBuffers (GLsizei n, const GLenum *bufs);
typedef void type_glRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

#define OpenGLFunction(name) type_##name *name;
#define GetOpenGLFuncAddress(name) name = (type_##name *)wglGetProcAddress(#name);
#define GetOpenGLFuncAddressEXT(name) name = (type_##name *)wglGetProcAddress(#name"EXT");

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
OpenGLFunction(glUniform1f)
OpenGLFunction(glUniform2f)
OpenGLFunction(glUniform3f)
OpenGLFunction(glUniform4f)
OpenGLFunction(glGenerateMipmap)
OpenGLFunction(glUniformMatrix3fv)
OpenGLFunction(glUniformMatrix4fv)
OpenGLFunction(glFramebufferTexture2D)
OpenGLFunction(glFramebufferRenderbuffer)
OpenGLFunction(glBlitFramebuffer)
OpenGLFunction(glBindRenderbuffer)
OpenGLFunction(glGenRenderbuffers)
OpenGLFunction(glBindFramebuffer)
OpenGLFunction(glGenFramebuffers)
OpenGLFunction(glCheckFramebufferStatus)
OpenGLFunction(glDrawBuffers)
OpenGLFunction(glActiveTexture)
OpenGLFunction(glUniform1i)
OpenGLFunction(glRenderbufferStorage)

static void
AssignOpenGLFunctions()
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
    GetOpenGLFuncAddress(glUniform1f)
    GetOpenGLFuncAddress(glUniform2f)
    GetOpenGLFuncAddress(glUniform3f)
    GetOpenGLFuncAddress(glUniform4f)
    GetOpenGLFuncAddress(glUniformMatrix3fv)
    GetOpenGLFuncAddress(glUniformMatrix4fv)
    GetOpenGLFuncAddress(glDrawBuffers)
    GetOpenGLFuncAddress(glActiveTexture)
    GetOpenGLFuncAddress(glUniform1i)

    GetOpenGLFuncAddressEXT(glGenerateMipmap);
    GetOpenGLFuncAddressEXT(glFramebufferTexture2D);
    GetOpenGLFuncAddressEXT(glFramebufferRenderbuffer);
    GetOpenGLFuncAddressEXT(glBlitFramebuffer);
    GetOpenGLFuncAddressEXT(glBindRenderbuffer);
    GetOpenGLFuncAddressEXT(glGenRenderbuffers);
    GetOpenGLFuncAddressEXT(glBindFramebuffer);
    GetOpenGLFuncAddressEXT(glGenFramebuffers);
    GetOpenGLFuncAddressEXT(glCheckFramebufferStatus);
    GetOpenGLFuncAddressEXT(glRenderbufferStorage);
    
}
