#version 330

layout (location = 0) in vec3 aPos;
uniform mat4 gWorld;

out vec3 TexCoord;  
//out float Scalar;

void main()
{  
    
    gl_Position =gWorld* vec4(aPos, 1.0);
    // gl_Position = vec4(0.5 * aPos, 1.0);
    // FragPos = vec3(gl_Position);
    TexCoord = aPos;
   // Scalar =aScalar;
    
}
