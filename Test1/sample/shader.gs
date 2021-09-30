#version 330

void main()
{
gl_Position = gl_in[0].gl_Position;
EmitVertex();
 gl_Position = gl_in[1].gl_Position;
 EmitVertex();
 gl_Position = gl_in[2].gl_Position;
 EmitVertex();
 EndPrimitive();
}