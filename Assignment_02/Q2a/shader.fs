#version 330
//in vec4 pos;


 in float Scalar;

vec4 getColor(float x){
    return vec4(x,0,255-x,1.0);
}
void main()
{
   // gl_FragColor=vec4(1.0,1.0,0,1.0);
    gl_FragColor=getColor(Scalar);
}
