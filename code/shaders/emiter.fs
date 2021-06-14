#version 110

varying vec2 TexCoord;
varying vec3 Normal;

uniform vec3 Color;

void main()
{
    gl_FragColor = vec4(Color, 1.0);
}
