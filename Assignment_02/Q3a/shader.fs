#version 330
//in vec4 pos;



in vec3 Objcolor;

void main()
{
	if(Objcolor.x==0){
		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	
}