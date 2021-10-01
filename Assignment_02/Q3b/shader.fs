#version 330
//in vec4 pos;


//in float Scalar;
in vec3 TexCoord;
uniform sampler3D ourTexture;


void main()
{
	float texture_color = texture(ourTexture, TexCoord).r;
	float scalar_color = 0.1;
	float diff = abs(texture_color - scalar_color);
	
	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	if(diff > 0.018){
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}