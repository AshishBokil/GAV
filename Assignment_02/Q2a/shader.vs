#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aScalar;
uniform mat4 gWorld;
uniform vec3 objcolor;

// out vec3 FragPos;  
out float Scalar;
out vec3 Objcolor;

void main()
{  
    
    gl_Position =gWorld* vec4(aPos, 1.0);
    // gl_Position = vec4(0.5 * aPos, 1.0);
    // FragPos = vec3(gl_Position);
    Scalar = aScalar;
    Objcolor=objcolor;
    
}
