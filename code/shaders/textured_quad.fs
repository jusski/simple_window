#version 110

varying vec2 TexCoord;

uniform sampler2D Texture;
void main()
{

    vec4 Texel = texture2D(Texture, TexCoord);
    gl_FragColor = Texel;
}
