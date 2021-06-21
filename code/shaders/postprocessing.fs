#version 110

varying vec2 TexCoord;

uniform vec3 Color;
uniform sampler2D Texture;

void main()
{
#if 0 // Inverse
    vec4 Texel = texture2D(Texture, TexCoord);
    gl_FragColor = vec4(1.0 - Texel.xyz, 1.0);
#elif 0 // GrayScale
    vec4 Texel = texture2D(Texture, TexCoord);
    float Average = 0.2126 * Texel.r + 0.7152 * Texel.g + 0.0722 * Texel.b;
    gl_FragColor = vec4(Average, Average, Average, 1.0);
#elif 0 // Box Blur
    float PixelX = 1.0/512.0;
    float PixelY = 1.0/512.0;

    float KernelWidth = 10.0;
    float HalfWidth = KernelWidth / 2.0;
    
    vec3 Color = vec3(0);
    float TotalWeight = KernelWidth * KernelWidth;
    
    for (float i = 0.0; i < KernelWidth; i += 1.0)
    {
        for (float j = 0.0; j < KernelWidth; j += 1.0)
        {
            vec2 Offset = vec2(i - HalfWidth, j - HalfWidth);
            vec4 Sample = texture2D(Texture, TexCoord + Offset * PixelX);

            Color += Sample.rgb;

        }
    }
    gl_FragColor.rgb = Color / TotalWeight;
#elif 1 // Gausian blur
    float tex_offset = 1.0 / 512.0;
    vec3 result = texture2D(Texture, TexCoord).rgb * 0.227027;
    
    result += texture2D(Texture, TexCoord + vec2(tex_offset * 1.0, 0.0)).rgb * 0.1945946;
    result += texture2D(Texture, TexCoord + vec2(tex_offset * 2.0, 0.0)).rgb * 0.1216216;
    result += texture2D(Texture, TexCoord + vec2(tex_offset * 3.0, 0.0)).rgb * 0.054054;
    result += texture2D(Texture, TexCoord + vec2(tex_offset * 4.0, 0.0)).rgb * 0.016216;

    result += texture2D(Texture, TexCoord - vec2(tex_offset * 1.0, 0.0)).rgb * 0.1945946;
    result += texture2D(Texture, TexCoord - vec2(tex_offset * 2.0, 0.0)).rgb * 0.1216216;
    result += texture2D(Texture, TexCoord - vec2(tex_offset * 3.0, 0.0)).rgb * 0.054054;
    result += texture2D(Texture, TexCoord - vec2(tex_offset * 4.0, 0.0)).rgb * 0.016216;

    gl_FragColor = vec4(result, 1.0);
#else
    gl_FragColor = vec4(0.9, 0.3, 1.0, 1.0);
#endif
    
}
