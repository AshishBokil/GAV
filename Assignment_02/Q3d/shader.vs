#version 330

layout (location = 0) in vec4 aPos;
uniform mat4 gWorld;

out vec4 Pos;
out vec3 Incr;
void main()
{  
    
    gl_Position =gWorld* vec4(aPos.xyz, 1.0);
    Pos =vec4(gl_Position.xyz,aPos.w);
    // Objcolor=objcolor;
  //  Incr=incr;
}
