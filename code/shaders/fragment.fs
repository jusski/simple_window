#version 110

varying vec3 Color;

void main()
{
    //gl_FragColor = vec4(Coord.x, Coord.y, Color.y, 1.0);
    gl_FragColor = vec4(Color, 1.0);
}