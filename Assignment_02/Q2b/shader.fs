#version 330
//in vec4 pos;


//in float Scalar;
in vec3 TexCoord;
uniform sampler3D ourTexture;
uniform float objectcolor;

vec4 colormap(float x) {
	float d = clamp(x, 0.0, 1.0);
	//return vec4(d, d, d, 1.0);
    return vec4(d, d, d, 1.0);
}
void main()
{
    gl_FragColor = texture(ourTexture, TexCoord);
}
