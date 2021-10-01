#version 330

layout (location = 0) in vec3 aPos;
uniform mat4 gWorldCube;

void main(){
gl_Position = vec4(aPos*0.5, 1.0);
}