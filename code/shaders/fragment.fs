#version 110

varying vec2 TexCoord;
varying vec3 Normal;
varying vec3 Position;

uniform vec3 Color;
uniform vec4 LightSource;
uniform vec4 CameraPosition;

void main()
{
    vec3 LightEmmiter = normalize(LightSource.xyz - Position);
    vec3 LightColor = vec3(1, 1, 1);

    float AmbientFactor = 0.1;
    vec3 AmbientColor = Color * AmbientFactor * LightColor;

    float DiffuseFactor = max(dot(Normal, LightEmmiter), 0.0);
    vec3 DiffuseColor = Color * DiffuseFactor * LightColor;

    vec3 CameraDirection = normalize(CameraPosition.xyz - Position);
    vec3 Reflection = reflect(-LightEmmiter, Normal);
    float SpecularFactor = pow(max(dot(Reflection, CameraDirection), 0.0), 64.0);
    vec3 SpecularColor = 0.5 * SpecularFactor * LightColor;

    vec3 PhongColor = AmbientColor + DiffuseColor + SpecularColor;
    gl_FragColor = vec4(PhongColor, 1.0);
}
