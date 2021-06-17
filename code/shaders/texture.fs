#version 110

varying vec2 TexCoord;
varying vec3 Normal;

uniform vec3 Color;
uniform sampler2D Texture;

void main()
{
    vec4 Texel = texture2D(Texture, TexCoord);
    gl_FragColor = vec4(Texel.xyz, 1.0);
}
