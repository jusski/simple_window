#version 110

varying vec2 TexCoord;

uniform sampler2D Texture;
uniform int Horizontal;

void main()
{
    float tex_offset = 1.0 / 512.0;
    vec3 result = texture2D(Texture, TexCoord).rgb * 0.227027;
    if(Horizontal == 1)
    {
        result += texture2D(Texture, TexCoord + vec2(tex_offset * 1.0, 0.0)).rgb * 0.1945946;
        result += texture2D(Texture, TexCoord + vec2(tex_offset * 2.0, 0.0)).rgb * 0.1216216;
        result += texture2D(Texture, TexCoord + vec2(tex_offset * 3.0, 0.0)).rgb * 0.054054;
        result += texture2D(Texture, TexCoord + vec2(tex_offset * 4.0, 0.0)).rgb * 0.016216;

        result += texture2D(Texture, TexCoord - vec2(tex_offset * 1.0, 0.0)).rgb * 0.1945946;
        result += texture2D(Texture, TexCoord - vec2(tex_offset * 2.0, 0.0)).rgb * 0.1216216;
        result += texture2D(Texture, TexCoord - vec2(tex_offset * 3.0, 0.0)).rgb * 0.054054;
        result += texture2D(Texture, TexCoord - vec2(tex_offset * 4.0, 0.0)).rgb * 0.016216;
    }
    else
    {
        result += texture2D(Texture, TexCoord + vec2(0.0, tex_offset * 1.0)).rgb * 0.1945946;
        result += texture2D(Texture, TexCoord + vec2(0.0, tex_offset * 2.0)).rgb * 0.1216216;
        result += texture2D(Texture, TexCoord + vec2(0.0, tex_offset * 3.0)).rgb * 0.054054;
        result += texture2D(Texture, TexCoord + vec2(0.0, tex_offset * 4.0)).rgb * 0.016216;

        result += texture2D(Texture, TexCoord - vec2(0.0, tex_offset * 1.0)).rgb * 0.1945946;
        result += texture2D(Texture, TexCoord - vec2(0.0, tex_offset * 2.0)).rgb * 0.1216216;
        result += texture2D(Texture, TexCoord - vec2(0.0, tex_offset * 3.0)).rgb * 0.054054;
        result += texture2D(Texture, TexCoord - vec2(0.0, tex_offset * 4.0)).rgb * 0.016216;
    }
    //result = vec3(1.0, 0, 1.0);
    gl_FragColor = vec4(result, 1.0);
}
