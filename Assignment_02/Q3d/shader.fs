#version 330
//in vec4 pos;



//in vec3 Pos;
uniform vec3 objcolor;


void main()
{
	if(objcolor.x==1){
	gl_FragColor = vec4(1.0, 0.0, 0, 1.0);
	//gl_FragColor = vec4(1.0, 0.0, 0, 1.0);
	}
	else gl_FragColor = vec4(1.0, 1.0, 1, 1.0);
}