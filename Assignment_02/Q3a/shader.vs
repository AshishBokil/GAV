#version 330

layout (location = 0) in vec3 aPos;
uniform mat4 gWorld;
uniform vec3 objcolor;

out vec3 Objcolor;


void main()
{  
    
    gl_Position =gWorld* vec4(aPos, 1.0);
     Objcolor=objcolor;
    
}
