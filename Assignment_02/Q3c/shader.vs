#version 330

layout (location = 0) in vec3 aPos;
uniform mat4 gWorld;
out vec3 Pos;

void main()
{  
    
    gl_Position =gWorld* vec4(aPos, 1.0);
    Pos =aPos;
    
}
