#version 330
//in vec4 pos;


//in float Scalar;
in vec3 TexCoord;
uniform sampler3D ourTexture;
in vec3 Objcolor;

vec4 colormap(float x) {
	float d = clamp(x, 0.0, 1.0);
	//return vec4(d, d, d, 1.0);
    return vec4(d, d, d, 1.0);
}
void main()
{

    if(Objcolor.x==0){
		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else {
        gl_FragColor = texture(ourTexture, TexCoord);
	}
    
    
}
