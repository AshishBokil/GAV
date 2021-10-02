#version 330
//in vec4 pos;



in vec3 Pos;

void main()
{

	gl_FragColor = vec4(1.0-Pos.x, 0.0, 1-Pos.z, 1.0);
	
}