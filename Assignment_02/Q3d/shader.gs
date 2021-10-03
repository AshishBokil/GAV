#version 330
#extension GL_ARB_arrays_of_arrays : enable
#extension GL_ARB_shading_language_420pack : enable

uniform vec3 Incr;
in vec4 Pos[];
layout (points) in;
layout (triangle_strip, max_vertices = 36) out;
uniform float iso;
uniform mat4 gWorld;

uniform sampler3D ourTexture;

out vec4 FragColor;

int thIndices[6][4] = int[6][4](
	int[4](0, 1, 2, 4),
	int[4](0, 1, 3, 4),
	int[4](2, 3, 6, 4),
	int[4](3, 6, 7, 4),
	int[4](1, 3, 7, 4),
	int[4](1, 5, 7, 4)
);

vec3 result(vec4 a, vec4 b)
{
	float ratio = iso / (a.w + b.w);
	vec3 diff = vec3(b.x - a.x, b.y - a.y, b.z - a.z);
	vec3 ans;
	if (a.w > b.w)
	{
		ratio = 1 - ratio;
	}
	ans = vec3(a.x + ratio * diff.x, a.y + ratio * diff.y, a.z + ratio * diff.z);
	

	return ans;
}

void find_triangles(vec4 th[4])
{
	// cout<<"checkpoint3.1"<<endl;
	vec4 v1, v2, v3, v4;
	vec3 temp[8];
	int count = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = i + 1; j < 4; j++)
		{
			//for(int k=j+1;k<4;k++){

			v1 = th[i];
			v2 = th[j];

		// gl_Position= vec4(0.0, 0.0, 0.0, 1.0);
		// EmitVertex();
		// gl_Position= vec4(0.0, 1.0, 0.0, 1.0);
		// EmitVertex();
		// gl_Position= vec4(1.0, 0.0, 0.0, 1.0);
		// EmitVertex();
		// EndPrimitive();

	// if(v2.w>0){
	// 	FragColor = vec4(v1.w)/255;
	// 	gl_Position= vec4(v1.xyz, 1.0);
	// 	EmitVertex();
	// 	FragColor = vec4(v2.w)/255;
	// 	gl_Position= vec4(v2.xyz, 1.0);
	// 	EmitVertex();
	// 	FragColor = vec4(0.0);
	// 	gl_Position= vec4(0.0, 0.0, 0.0, 1.0);
	// 	EmitVertex();
	// 	EndPrimitive();
	// }

		// 	if(v1.w == v2.w){

		// FragColor = vec4(v1.w)/255;
		// gl_Position= vec4(v1.xyz, 1.0);
		// EmitVertex();
		// FragColor = vec4(v2.w)/255;
		// gl_Position= vec4(v2.xyz, 1.0);
		// EmitVertex();
		// FragColor = vec4(1.0);
		// gl_Position= vec4(0.0, 0.0, 0.0, 1.0);
		// EmitVertex();
		// EndPrimitive();

		// 	}

			//v3=vec4(	vertices[4*th[k]],vertices[4*th[k]+1],vertices[4*th[k]+2]	,vertices[4*th[k]+3]);
			// cout<<4*th[i]<<" "<<4*th[j]<<" "<<endl;
			//  cout<<"v1="<<v1.x<<" "<<v1.y<<" "<<v1.z<<" :"<<v1.w<<endl;
			//  cout<<"v2="<<v2.x<<" "<<v2.y<<" "<<v2.z<<" :"<<v2.w<<endl;

			if (v1.w >= iso && v2.w < iso || v1.w < iso && v2.w >= iso)
			{

				temp[count] = result(v1, v2);
		
				count++;
			}
		
		}
	}
	// cout<<"checkpoint3.3"<<endl;
	// cout<<"lines in tetra "<<count<<endl;

	if (count == 4)
	{

		FragColor = vec4(1.0,0,0,1);
		gl_Position= gWorld*vec4(temp[0],1);
		EmitVertex();
        gl_Position= gWorld*vec4(temp[1],1);
		EmitVertex();
        gl_Position= gWorld*vec4(temp[3],1);
		EmitVertex();
        EndPrimitive();
        gl_Position= gWorld*vec4(temp[0],1);
		EmitVertex();
        gl_Position= gWorld*vec4(temp[2],1);
		EmitVertex();
        gl_Position= gWorld*vec4(temp[3],1);
		EmitVertex();

	
	}
	else if(count==3)
	{

		FragColor = vec4(1.0,0,0,1);
		gl_Position=  gWorld*vec4(temp[0],1);
		EmitVertex();
        gl_Position=  gWorld*vec4(temp[1],1);
		EmitVertex();
        gl_Position=  gWorld*vec4(temp[2],1);
		EmitVertex();

	}
	//cout<<newindex<<endl;
	//	exit(0);
}

void tetra(vec4 cell[8])
{
	//cout<<"checkpoint2"<<endl;
	for (int i = 0; i < 6; i++)
	{
		vec4 th[4] = {
			cell[thIndices[i][0]],
			cell[thIndices[i][1]],
			cell[thIndices[i][2]],
			cell[thIndices[i][3]]

		};
		// for(int j=0;j<4;j++)cout<<th[j]<<" ";
		// 	cout<<endl;
		find_triangles(th);
        EndPrimitive();
	}
}

void main(){
    
    vec4 cell[8];
    vec4 xincr=vec4(Incr.x,0,0,0);
    vec4 yincr=vec4(0,Incr.y,0,0);
    vec4 zincr=vec4(0,0,Incr.z,0);

	cell[0] = Pos[0];
	cell[1] = Pos[0] + xincr;
	cell[2] = Pos[0] + yincr;
	cell[3] = Pos[0] + xincr + yincr;
	cell[4] = Pos[0] + zincr ;
	cell[5] = Pos[0] + xincr +zincr;
	cell[6] = Pos[0] + yincr + zincr;
	cell[7] = Pos[0] + xincr + yincr + zincr;

	for(int i=0; i<8; i++){
		// if(cell[i].x<0.1 || cell[i].x>1.1){
		// 	 if(cell[0].x<-0.1 || cell[0].x>1.1){
		// 	FragColor = vec4(1.0);
		// 	gl_Position=vec4(0.0, 1.0, 0.0, 1.0);
		// 	EmitVertex();
		// 	gl_Position= vec4(1.0, 0.0, 0.0, 1.0);
		// 	EmitVertex();
		// 	gl_Position= vec4(0.0, 0.0, 0.0, 1.0);
		// 	EmitVertex();
		// 	EndPrimitive();
		// }
		cell[i].w = texture(ourTexture, cell[i].xyz).r * 255;
	}

	// gl_Position= vec4(cell[0].xyz, 1.0);
	// EmitVertex();
    // gl_Position= vec4(cell[1].xyz, 1.0);
	// EmitVertex();
    // gl_Position= vec4(cell[2].xyz, 1.0);
	// EmitVertex();
    // EndPrimitive();

    tetra(cell);
}