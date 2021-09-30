#version 330
//in vec4 pos;


in float Scalar;

vec4 colormap(float x) {
	float d = clamp(x, 0.0, 1.0);
	return vec4(d, d, d, 1.0);
}
void main()
{
   // gl_FragColor=vec4(1.0,1.0,0,1.0);
    gl_FragColor=colormap(Scalar/255);
}
