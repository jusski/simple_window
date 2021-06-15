#version 110

varying vec2 TexCoord;
varying vec3 Normal;

uniform vec3 Color;

void main()
{
    vec3 LightEmmiter = vec3(1, 1, -1);
    vec3 LightColor = vec3(1, 1, 1);

    float AmbientFactor = 0.1;
    vec3 AmbientColor = Color * AmbientFactor * LightColor;

    float DiffuseFactor = dot(Normal, LightEmmiter);
    vec3 DiffuseColor = Color * DiffuseFactor * LightColor;

    vec3 PhongColor = AmbientColor + DiffuseColor;
    gl_FragColor = vec4(PhongColor, 1.0);
}
