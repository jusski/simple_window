#version 110

varying vec2 TexCoord;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main()
{
    vec3 Texel1 = texture2D(Texture1, TexCoord).rgb;
    vec3 Texel2 = texture2D(Texture2, TexCoord).rgb;
    Texel1 = Texel1 * Texel1;
    Texel2 = Texel2 * Texel2;
    vec3 Exponent = Texel1 + Texel2;
    vec3 Color = 1.0 - exp(-Exponent * 1.0);
    vec3 sRGB = sqrt(Color);
    gl_FragColor = vec4(sRGB, 1.0);
}
