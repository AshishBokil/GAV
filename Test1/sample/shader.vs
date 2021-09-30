#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
uniform mat4 gWorld;
//in vec4 Color;
//out vec4 fColor;

void main()
{
    //gl_Position =gWorld* vec4(0.5 * aNormal, 1.0);/
    gl_Position = vec4(aPos*0.5, 1.0);
   // fColor = Color;
}
