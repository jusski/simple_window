#version 110

varying vec2 TexCoord;
varying vec3 Normal;

uniform vec3 Color;
uniform sampler2D Texture;

void main()
{
#if 0 // Inverse
    vec4 Texel = texture2D(Texture, TexCoord);
    gl_FragColor = vec4(1.0 - Texel.xyz, 1.0);
#elif 1 // GrayScale
    vec4 Texel = texture2D(Texture, TexCoord);
    float Average = 0.2126 * Texel.r + 0.7152 * Texel.g + 0.0722 * Texel.b;
    gl_FragColor = vec4(Average, Average, Average, 1.0);
#else
    gl_FragColor = vec4(0.9, 0.3, 1.0, 1.0);
#endif
    
}
