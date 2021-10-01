#version 330

in vec3 aPos;
uniform mat4 gWorld;
//in vec4 Color;
//out vec4 fColor;

void main()
{
    gl_Position = vec4(aPos*0.5, 1.0);
}
