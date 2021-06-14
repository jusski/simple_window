#version 110

varying vec2 TexCoord;
varying vec3 Normal;

uniform vec3 Color;

void main()
{
    //gl_FragColor = vec4(0.2, 0.5, 0.3, 1.0);
    vec3 LightEmmiter = vec3(1, 1, -1);
    //vec3 Color = vec3(0.2, 0.5, 0.3);
    float Specular = dot(Normal, LightEmmiter);
    vec3 ReflectedColor = Specular * Color;
    gl_FragColor = vec4(ReflectedColor, 1.0);
}
