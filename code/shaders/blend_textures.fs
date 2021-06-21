#version 110

varying vec2 TexCoord;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main()
{
    vec4 Texel1 = texture2D(Texture1, TexCoord);
    vec4 Texel2 = texture2D(Texture2, TexCoord);
    gl_FragColor = Texel1 + Texel2;
//    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
