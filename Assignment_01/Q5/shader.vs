#version 330

in vec4 Position;
uniform mat4 gWorld;
out vec4 pos;

void main()
{
    gl_Position = gWorld * Position;
    pos=gl_Position;
}
