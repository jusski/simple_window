#version 110

attribute vec3 aPosition;
uniform mat3 RotationMatrix;

void main()
{
    gl_Position = vec4(RotationMatrix * aPosition, 1.0);
}
