#version 330

#define ll long long int

uniform vec3 Incr;
layout (Pos) in;
layout (triangle_strip, max_vertices = 24) out;
uniform sampler3D ourTexture;
uniform float iso;


ll thIndices[6][4] = {
	0, 1, 2, 4,
	0, 1, 3, 4,
	2, 3, 6, 4,
	3, 6, 7, 4,
	1, 3, 7, 4,
	1, 5, 7, 4

};

static vec3 result(vec4 a, vec4 b)
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

static void find_triangles(vec4 th[])
{
	// cout<<"checkpoint3.1"<<endl;
	vec4 v1, v2, v3, v4;
	vec3 temp[8] = {0};
	int count = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = i + 1; j < 4; j++)
		{
			//for(int k=j+1;k<4;k++){

			v1 = th[i];
			v2 = th[j];
			//v3=vec4(	vertices[4*th[k]],vertices[4*th[k]+1],vertices[4*th[k]+2]	,vertices[4*th[k]+3]);
			// cout<<4*th[i]<<" "<<4*th[j]<<" "<<endl;
			//  cout<<"v1="<<v1.x<<" "<<v1.y<<" "<<v1.z<<" :"<<v1.w<<endl;
			//  cout<<"v2="<<v2.x<<" "<<v2.y<<" "<<v2.z<<" :"<<v2.w<<endl;

			if (v1.w >= v && v2.w < v || v1.w < v && v2.w >= v)
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
		gl_Position= vec4(temp[0],1);
		EmitVertex();
        gl_Position= vec4(temp[1],1);
		EmitVertex();
        gl_Position= vec4(temp[3],1);
		EmitVertex();
        gl_Position= vec4(temp[0],1);
		EmitVertex();
        gl_Position= vec4(temp[2],1);
		EmitVertex();
        gl_Position= vec4(temp[3],1);
		EmitVertex();

	
	}
	else if(count==3)
	{
		gl_Position= vec4(temp[0],1);
		EmitVertex();
        gl_Position= vec4(temp[1],1);
		EmitVertex();
        gl_Position= vec4(temp[2],1);
		EmitVertex();

	}
	//cout<<newindex<<endl;
	//	exit(0);
}

static void tetra(vec4 cell[])
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
    vec4 xincr=vec4(Pos.x,0,0,0);
    vec4 yincr=vec4(0,Pos.y,0,0);
    vec4 zincr=vec4(0,0,Pos.z,0);
	cell[0] = Pos;
	cell[1] = Pos + xincr;
	cell[2] = Pos + yincr;
	cell[3] = Pos + xincr + yincr;
	cell[4] = Pos + zincr ;
	cell[5] = Pos + xincr +zincr;
	cell[6] = Pos + yincr + zincr;
	cell[7] = Pos + xincr + yincr + zincr;

    tetra(cell);
}