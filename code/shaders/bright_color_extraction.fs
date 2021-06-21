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

    // Ambient
    float AmbientFactor = 0.1;
    vec3 AmbientColor = Color * AmbientFactor * LightColor;

    // Diffuse
    float DiffuseFactor = max(dot(Normal, LightEmmiter), 0.0);
    vec3 DiffuseColor = Color * DiffuseFactor * LightColor;

    // Specular
    vec3 CameraDirection = normalize(CameraPosition.xyz - Position);
    vec3 Reflection = reflect(-LightEmmiter, Normal);
    float SpecularFactor = pow(max(dot(Reflection, CameraDirection), 0.0), 128.0);
    vec3 SpecularColor = Color * 0.5 * SpecularFactor * LightColor;

    vec3 PhongColor = AmbientColor + DiffuseColor + SpecularColor;

    gl_FragData[0] = vec4(PhongColor, 1.0);
    float brightness = dot(PhongColor, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        gl_FragData[1] = vec4(PhongColor, 1.0);
    else
        gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);
}
