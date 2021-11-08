#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <bits/stdc++.h>
#include "file_utils.h"
#include "math_utils.h"
#include <assert.h>
#include "OFFReader.c"

#define ll long long int
/********************************************************************/
/*   Variables */

char *theProgramTitle = "Sample";
int theWindowWidth = 1500, theWindowHeight = 1000;
int theWindowPositionX = 40, theWindowPositionY = 40;
bool isFullScreen = true;
bool isAnimating = true;
float rotation = 0.00f, rotateLight = 0.00f, rotateX = 0, rotateY = 0, rotateZ = 0;
float translation = 0.2f;
float nextX = 0.f, nextY = 0.0f;
float x = 0.0f, y = -1.0f;
int choicelight = 0, choice = 0;
//int noOfPoints=500000;
GLuint VBO, VAO, IBO, IAO, VBCube, IBCube, VACube;
GLuint gWorldLocation, lightpos_location, view_location, objColor_location;
unsigned long noOfvertices, noOfIndices;
/*model*/
OffModel *model;
/* Constants */
const int ANIMATION_DELAY = 20; /* milliseconds between rendering */
const char *pVSFileName = "shader.vs";
const char *pFSFileName = "shader.fs";

/********************************************************************
  Utility functions

 */

float xmax, ymax, zmax, xinc, yinc, zinc,iso;
Matrix4f World;
float *vertices;
Vector3f *newvertices;
unsigned int *indices;
ll newindex = 0;
ll v = 50; ///Iso value or scalar value

ll thIndices[6][4] = {
	0, 1, 2, 4,
	0, 1, 3, 4,
	2, 3, 6, 4,
	3, 6, 7, 4,
	1, 3, 7, 4,
	1, 5, 7, 4

};

/***********  domainSearchTree *******/

struct dst
{
	int min_iso;
	int max_iso;
	ll orig_Index;
	dst *child[8];
};

struct Pair
{
	int minval;
	int maxval;
};

struct dst *getNode()
{
	struct dst *temp = new dst;
	for (int i = 0; i < 8; i++)
	{
		temp->child[i] = NULL;
	}
	return temp;
}

int getIndex(int c, int i, int j, int k)
{
	//	cout<<" i:"<<i<<" j:"<<j<<" k:"<<k;
	//	cout<<" xmax:"<<xmax<<" ymax:"<<ymax<<" zmax:"<<zmax;
	if (c == 0)
		return (i + xmax * j + xmax * ymax * k);
	if (c == 1)
		return ((i + 1) + xmax * j + xmax * ymax * k);
	if (c == 2)
		return (i + xmax * (j + 1) + xmax * ymax * k);
	if (c == 3)
		return ((i + 1) + xmax * (j + 1) + xmax * ymax * k);
	if (c == 4)
		return (i + xmax * j + xmax * ymax * (k + 1));
	if (c == 5)
		return ((i + 1) + xmax * j + xmax * ymax * (k + 1));
	if (c == 6)
		return (i + xmax * (j + 1) + xmax * ymax * (k + 1));
	if (c == 7)
		return ((i + 1) + xmax * (j + 1) + xmax * ymax * (k + 1));
	return -1;
}

/**********************************************/

/* post: compute frames per second and display in window's title bar */

// The error handling function is copied from youtube channel "The Cherno"
#define GL_CALL(x)   \
	GlClearErrors(); \
	x;               \
	assert(GlCheckErrors());

void GlClearErrors()
{
	while (glGetError() != GL_NO_ERROR)
		;
}

bool GlCheckErrors()
{
	bool flag = true;
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR)
	{
		cout << "Opengl Error: " << error << endl;
		flag = false;
	}
	return flag;
}

void computeFPS()
{
	static int frameCount = 0;
	static int lastFrameTime = 0;
	static char *title = NULL;
	int currentTime;

	if (!title)
		title = (char *)malloc((strlen(theProgramTitle) + 20) * sizeof(char));
	frameCount++;
	currentTime = glutGet((GLenum)(GLUT_ELAPSED_TIME));
	if (currentTime - lastFrameTime > 1000)
	{
		sprintf(title, "%s [ FPS: %4.2f ]",
				theProgramTitle,
				frameCount * 1000.0 / (currentTime - lastFrameTime));
		glutSetWindowTitle(title);
		lastFrameTime = currentTime;
		frameCount = 0;
	}
}

static Vector3f result(Vector4f a, Vector4f b)
{
	float ratio = v / (a.w + b.w);
	Vector3f diff = Vector3f(b.x - a.x, b.y - a.y, b.z - a.z);
	Vector3f ans;
	if (a.w > b.w)
	{
		ratio = 1 - ratio;
	}
	ans = Vector3f(a.x + ratio * diff.x, a.y + ratio * diff.y, a.z + ratio * diff.z);

	return ans;
}

static void find_triangles(ll th[])
{
	// cout<<"checkpoint3.1"<<endl;
	Vector4f v1, v2, v3, v4;
	Vector3f temp[8] = {0};
	int count = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = i + 1; j < 4; j++)
		{
			//for(int k=j+1;k<4;k++){

			v1 = Vector4f(vertices[4 * th[i]], vertices[4 * th[i] + 1], vertices[4 * th[i] + 2], vertices[4 * th[i] + 3]);
			v2 = Vector4f(vertices[4 * th[j]], vertices[4 * th[j] + 1], vertices[4 * th[j] + 2], vertices[4 * th[j] + 3]);
			//v3=Vector4f(	vertices[4*th[k]],vertices[4*th[k]+1],vertices[4*th[k]+2]	,vertices[4*th[k]+3]);
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
		newvertices[newindex] = temp[0];
		indices[newindex] = newindex;
		newindex++;
		newvertices[newindex] = temp[1];
		indices[newindex] = newindex;
		newindex++;
		newvertices[newindex] = temp[2];
		indices[newindex] = newindex;
		newindex++;
		newvertices[newindex] = temp[0];
		indices[newindex] = newindex;
		newindex++;
		newvertices[newindex] = temp[1];
		indices[newindex] = newindex;
		newindex++;
		newvertices[newindex] = temp[3];
		indices[newindex] = newindex;
		newindex++;
	}
	else if (count == 3)
	{
		newvertices[newindex] = temp[0];
		indices[newindex] = newindex;
		newindex++;
		newvertices[newindex] = temp[1];
		indices[newindex] = newindex;
		newindex++;
		newvertices[newindex] = temp[2];
		indices[newindex] = newindex;
		newindex++;
	}
	//cout<<newindex<<endl;
	//	exit(0);
}

static void tetra(ll* cell)
{
	//cout<<"checkpoint2"<<endl;
	for (int i = 0; i < 6; i++)
	{
		ll th[4] = {
			cell[thIndices[i][0]],
			cell[thIndices[i][1]],
			cell[thIndices[i][2]],
			cell[thIndices[i][3]]

		};
		// for(int j=0;j<4;j++)cout<<th[j]<<" ";
		// 	cout<<endl;
		find_triangles(th);
	}
}

void getCell(ll* cell,int i, int j, int k)
{
	//ll cell[8];

	cell[0] = (i + xmax * j + xmax * ymax * k);
	cell[1] = ((i + 1) + xmax * j + xmax * ymax * k);
	cell[2] = (i + xmax * (j + 1) + xmax * ymax * k);
	cell[3] = ((i + 1) + xmax * (j + 1) + xmax * ymax * k);
	cell[4] = (i + xmax * j + xmax * ymax * (k + 1));
	cell[5] = ((i + 1) + xmax * j + xmax * ymax * (k + 1));
	cell[6] = (i + xmax * (j + 1) + xmax * ymax * (k + 1));
	cell[7] = ((i + 1) + xmax * (j + 1) + xmax * ymax * (k + 1));

	// for(int i=0;i<8;i++){
	// 	cout<<cell[i]<<endl;
	// }
//	return cell;
}

void dstSearch(dst *root,float iso){
	if(root->child[0]==NULL){
		
	}
}

void printcell(ll* cell){
	//cout<<" pointer ="<<cell<<endl;
	for(int i=0;i<8;i++){
		cout<<cell[i]<<endl;
	}
}

void domainSearch(dst* root){
	
}

Pair *initDST(dst *root, float xa, float xb, float ya, float yb, float za, float zb)
{
	//cout<<"lenx="<<xb-xa<<" leny="<<yb-ya<<" lenz="<<zb-za<<endl;
	//if(xb-xa<xinc)return NULL;

	if (xb == 1 && (xb - xa) == xinc || yb == 1 && (yb - ya) == yinc || zb == 1 && (zb - za) == zinc)
	{
		int i = (int)xa / xinc, j = (int)ya / yinc, k = (int)za / zinc;
		int index = getIndex(0, i, j, k);
		struct Pair *p = new Pair;
		p->minval = 0;
		p->maxval = 0;
		root->min_iso = 0;
		root->max_iso = 0;
		root->orig_Index = index;
		return p;
	}
	if ((xb - xa) == xinc && (yb - ya) == yinc && (zb - za) == zinc)
	{

		int i = (int)(xa / xinc), j = (int)(ya / yinc), k = (int)(za / zinc);
		//	cout<<i<<" "<<j<<" "<<k<<endl;
		//int index=getIndex(0,i,j,k);
		int min = 9999, max = 0;
		struct Pair *p = new Pair;
		for (int l = 0; l < 8; l++)
		{
			int index = getIndex(l, i, j, k);
			// cout<<"index:"<<index<<" vertex#= "<<4*index+3<<" iso="<<vertices[4*index+3]<<endl;
			if (vertices[4 * index + 3] < min)
				min = vertices[4 * index + 3];
			if (vertices[4 * index + 3] > max)
				max = vertices[4 * index + 3];
			//	return p;
		}
		p->minval = min;
		p->maxval = max;
		root->min_iso = p->minval;
		root->max_iso = p->maxval;

		if(root->min_iso<=iso && root->max_iso>=iso){
			ll cell[8];
			getCell(cell,i,j,k);
			//printcell(cell);
			 tetra(cell);
		}		

		root->orig_Index = 4 * getIndex(0, i, j, k);
		//	cout<<"Origin index="<<root->orig_Index<<"\n\n";
		//cout<<root->min_iso<<" "<<root->max_iso<<endl;
		return p;
	}
	else
	{
		//root->child[0]=getNode();
		// initDST(root->child[0],xmin,xmax/2,ymin,ymax/2,zmin,zmax/2);

		Pair *p = new Pair;
		p->maxval = 0;
		p->minval = 999;
		for (int k = 0; k < 2; k++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int i = 0; i < 2; i++)
				{

					float xac = xa, xbc = xb, yac = ya, ybc = yb, zac = za, zbc = zb;
					int index = (i + 2 * j + 2 * 2 * k);
					root->child[index] = getNode();

					if (i == 1)
						xac += (xbc - xac) / 2;
					else
						xbc -= (xbc - xac) / 2;

					if (j == 1)
						yac += (ybc - yac) / 2;
					else
						ybc -= (ybc - yac) / 2;

					if (k == 1)
						zac += (zbc - zac) / 2;
					else
						zbc -= (zbc - zac) / 2;

					//cout<<"x=("<<xac<<","<<xbc<<")\tleny=("<<yac<<" "<<ybc<<")\tlenz=("<<zac<<" "<<zbc<<")\n";
					//exit(0);
					Pair *p1 = initDST(root->child[index], xac, xbc, yac, ybc, zac, zbc);
					if (p1->minval < p->minval)
						p->minval = p1->minval;
					if (p1->maxval > p->maxval)
						p->maxval = p1->maxval;
				}
			}
		}

		root->min_iso = p->minval;
		root->max_iso = p->maxval;
		root->orig_Index = getIndex(0, xa, ya, za);
		return p;
	}
}

void printdst(dst *root, int level)
{
	//cout<<root<<endl;

	if (root == NULL)
		return;
	cout << "level: " << level << "\tmin=" << root->min_iso << "\tmax=" << root->max_iso << "\torig_Index=" << root->orig_Index << "\n";
	for (int i = 0; i < 8; i++)
	{
		printdst(root->child[i], level + 1);
	}
}

static void CreateVertexBuffer()
{
	//cin>>iso;
	iso=25;
	//cout<<"iso="<<iso<<endl;
	//exit(0);
	FILE *input;
	unsigned int x, y, z;
	input = fopen("fuel.txt", "r");
	if (input == NULL)
	{
		cout << "error opening file";
		exit(0);
	}
	fscanf(input, "%d", &x);
	fscanf(input, "%d", &y);
	fscanf(input, "%d", &z);
	cout << x << " " << y << " " << z << endl;
	xinc = (float)1 / x;
	yinc = (float)1 / y;
	zinc = (float)1 / z;
	xmax = x + 1;
	ymax = y + 1;
	zmax = z + 1;
	cout << " xinc:" << xinc << " yinc:" << yinc << " zinc:" << zinc << endl;
	cout << " xmax:" << xmax << " ymax:" << ymax << " zmax:" << zmax << endl;
	// int max=(x>y)?(x>z?x:z):(y>z?y:z);
	// xmax=(float)x/max;
	// ymax=(float)y/max;
	// zmax=(float)z/max;

	noOfvertices = (x + 1) * (y + 1) * (z + 1);
	vertices = new float[noOfvertices * 4];
	newvertices = new Vector3f[noOfvertices * 36];
	indices = new unsigned int[noOfvertices * 36];
	for (int k = 0; k <= z; k++)
	{
		for (int j = 0; j <= y; j++)
		{
			for (int i = 0; i <= x; i++)
			{
				ll pos = k * y * x + j * x + i;
				if (i == x || j == y || k == z)
				{
					vertices[4 * pos + 0] = ((float)i / (x - 1)) /**((float)x/max)*/;
					vertices[4 * pos + 1] = ((float)j / (y - 1)) /**((float)y/max)*/;
					vertices[4 * pos + 2] = ((float)k / (z - 1)) /**((float)z/max)*/;
					vertices[4 * pos + 3] = vertices[4 * (pos - 1) + 3];
					continue;
				}
				vertices[4 * pos + 0] = ((float)i / (x - 1)) /**((float)x/max)*/;
				vertices[4 * pos + 1] = ((float)j / (y - 1)) /**((float)y/max)*/;
				vertices[4 * pos + 2] = ((float)k / (z - 1)) /**((float)z/max)*/;
				fscanf(input, "%f", &vertices[4 * pos + 3]);
				//	data[t++]=vertices[4 * pos + 3];
			}
		}
	}

	dst *root = getNode();
	Pair *p = initDST(root, 0, 1, 0, 1, 0, 1);
	cout << " min=" << p->minval << " max=" << p->maxval << "\n\n";
	cout<<"newindex="<<newindex<<endl;
	//exit(0);
//	printdst(root, 0);


	ll newVIndex = 0;
	// for (int k = 0; k < z - 1; k++)
	// {

	// 	for (int j = 0; j < y - 1; j++)
	// 	{
	// 		for (int i = 0; i < x - 1; i++)
	// 		{

	// 			// for(int l=0;l<8;l++)cout<<cell[l]<<" ";
	// 			// cout<<endl;
	// 		//	tetra(getCell(i, j, k));
	// 		}
	// 	}
	// }
	//cout<<newVIndex<<endl;
	noOfIndices = newindex;
	int t = 0;
	// for (int i = 0; i < newindex; i++)
	// {

	// 	// if (!(newvertices[i].x >= 0 && newvertices[i].y >= 0 && newvertices[i].z >= 0))
	// 	// {
	// 	// 	t++;
	// 	// 	cout << indices[i] << endl;
	// 	// 	cout << newvertices[i].x << " " << newvertices[i].y << " " << newvertices[i].z << endl;
	// 	// }
	// 	cout << newvertices[i].x << " " << newvertices[i].y << " " << newvertices[i].z << endl;
	// }
	//cout << newindex << " " << t << endl;
	// exit(0);

	///////Generate cube
	Vector3f cubevertices[8] = {
		Vector3f(0, 0, 0),
		Vector3f(1, 0, 0),
		Vector3f(0, 1, 0),
		Vector3f(1, 1, 0),
		Vector3f(0, 0, 1),
		Vector3f(1, 0, 1),
		Vector3f(0, 1, 1),
		Vector3f(1, 1, 1)

	};

	unsigned int cubeindices[24] = {
		0, 1,
		0, 2,
		0, 4,
		1, 3,
		1, 5,
		2, 3,
		2, 6,
		3, 7,
		4, 5,
		4, 6,
		5, 7,
		6, 7};

	GL_CALL(glGenVertexArrays(1, &VAO));
	GL_CALL(glBindVertexArray(VAO));

	GL_CALL(glGenBuffers(1, &VBO));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, newindex * 3 * sizeof(float), newvertices, GL_STATIC_DRAW));

	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), 0));

	// GL_CALL(glEnableVertexAttribArray(1));
	// GL_CALL(glVertexAttribPointer(1, 1, GL_FLOAT, GL_TRUE, 4 * sizeof(float), (void *)(3 * sizeof(float))));

	GL_CALL(glGenBuffers(1, &IBO));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, newindex * sizeof(unsigned int), indices, GL_STATIC_DRAW));

	///For Cube
	GL_CALL(glGenVertexArrays(1, &VACube));
	GL_CALL(glBindVertexArray(VACube));

	GL_CALL(glGenBuffers(1, &VBCube));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBCube));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(float), cubevertices, GL_STATIC_DRAW));

	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), 0));

	GL_CALL(glGenBuffers(1, &IBCube));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBCube));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(unsigned int), cubeindices, GL_STATIC_DRAW));
}

static void AddShader(GLuint ShaderProgram, const char *pShaderText, GLenum ShaderType)
{
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0)
	{
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	const GLchar *p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	GL_CALL(glShaderSource(ShaderObj, 1, p, Lengths));
	GL_CALL(glCompileShader(ShaderObj));
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	glAttachShader(ShaderProgram, ShaderObj);
}

using namespace std;

static void CompileShaders()
{
	GLuint ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0)
	{
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	string vs, fs;

	if (!ReadFile(pVSFileName, vs))
	{
		exit(1);
	}

	if (!ReadFile(pFSFileName, fs))
	{
		exit(1);
	}

	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = {0};

	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glUseProgram(ShaderProgram);
	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	lightpos_location = glGetUniformLocation(ShaderProgram, "lightPos");
	view_location = glGetUniformLocation(ShaderProgram, "viewPos");
	objColor_location = glGetUniformLocation(ShaderProgram, "objcolor");
}

/********************************************************************
 Callback Functions
 These functions are registered with the glut window and called 
 when certain events occur.
 */

void onInit(int argc, char *argv[])
/* pre:  glut window has been initialized
   post: model has been initialized */
{
	/* by default the back ground color is black */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	CreateVertexBuffer();
	CompileShaders();

	/* set to draw in window based on depth  */
	glEnable(GL_DEPTH_TEST);
}

static void onDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPointSize(5);

	Matrix4f scaleMat, translateMat, rotateMat, transformcube;

	transformcube.InitIdentity();

	translateMat.InitTranslationTransform(-0.5, -0.5, -0.5);
	transformcube = translateMat * transformcube;

	// rotateMat.InitAxisRotateTransform(Vector3f(0, 0, 1), M_PI);
	// transformcube = rotateMat * transformcube;

	// scaleMat.InitScaleTransform(1.3f, 1.3f, 1.3f);
	// transformcube = scaleMat * transformcube;

	if (choice == 1)
	{
		rotateMat.InitAxisRotateTransform(Vector3f(1, 0, 0), rotateX);
		transformcube = rotateMat * transformcube;
	}

	if (choice == 2)
	{
		rotateMat.InitAxisRotateTransform(Vector3f(0, 1, 0), rotateY);
		transformcube = rotateMat * transformcube;
	}

	if (choice == 3)
	{
		rotateMat.InitAxisRotateTransform(Vector3f(0, 0, 1), rotateZ);
		transformcube = rotateMat * transformcube;
	}

	translateMat.InitTranslationTransform(0.0f, 0.0f, +1.8f);
	transformcube = translateMat * transformcube;

	Matrix4f persProjection;
	PersProjInfo proj(90.0f, 1.0f, 1.0f, +1.0f, -1.0f);
	persProjection.InitPersProjTransform(proj);
	transformcube = persProjection * transformcube;

	//////Cube

	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &transformcube.m[0][0]);
	glUniform3f(objColor_location, 0, 0, 0);
	glBindVertexArray(VACube);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);

	/////object
	scaleMat.InitScaleTransform(1, 1, (zmax-1)/zmax);
	transformcube = scaleMat * transformcube;

	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &transformcube.m[0][0]);
	glUniform3f(objColor_location, 1, 0, 0);
	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, noOfIndices, GL_UNSIGNED_INT, nullptr);

	/* check for any errors when rendering */
	GLenum errorCode = glGetError();
	if (errorCode == GL_NO_ERROR)
	{
		/* double-buffering - swap the back and front buffers */
		glutSwapBuffers();
	}
	else
	{
		fprintf(stderr, "OpenGL rendering error %d\n", errorCode);
	}
}

/* pre:  glut window has been resized
 */
static void onReshape(int width, int height)
{
	glViewport(0, 0, width, height);
	if (!isFullScreen)
	{
		theWindowWidth = width;
		theWindowHeight = height;
	}
	// update scene based on new aspect ratio....
}

/* pre:  glut window is not doing anything else
   post: scene is updated and re-rendered if necessary */
static void onIdle()
{
	static int oldTime = 0;
	if (isAnimating)
	{
		int currentTime = glutGet((GLenum)(GLUT_ELAPSED_TIME));
		/* Ensures fairly constant framerate */
		if (currentTime - oldTime > ANIMATION_DELAY)
		{
			// do animation....
			rotation += 0.00;

			oldTime = currentTime;
			/* compute the frame rate */
			computeFPS();
			/* notify window it has to be repainted */
			glutPostRedisplay();
		}
	}
}

/* pre:  mouse is dragged (i.e., moved while button is pressed) within glut window
   post: scene is updated and re-rendered  */
static void onMouseMotion(int x, int y)
{
	/* notify window that it has to be re-rendered */

	glutPostRedisplay();
}

/* pre:  mouse button has been pressed while within glut window
   post: scene is updated and re-rendered */
static void onMouseButtonPress(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// Left button pressed
		//	glutPostRedisplay();
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		// Left button un pressed
	}
	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}

/* pre:  key has been pressed
   post: scene is updated and re-rendered */
static void onAlphaNumericKeyPress(unsigned char key, int x, int y)
{
	switch (key)
	{
		/* toggle animation running */
	case 'a':
		isAnimating = !isAnimating;
		break;
		/* reset */
	case '1':
		choice = 1;
		rotateX += 0.05;
		break;

	case '2':
		choice = 2;
		rotateY += 0.05;
		break;

	case '3':
		choice = 3;
		rotateZ += 0.05;
		break;

	case 'r':
		rotation = 0;
		break;
		/* quit! */
	case 'Q':
	case 'q':
	case 27:
		exit(0);
	}

	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}

/* pre:  arrow or function key has been pressed
   post: scene is updated and re-rendered */
static void onSpecialKeyPress(int key, int x, int y)
{
	/* please do not change function of these keys */
	switch (key)
	{
		/* toggle full screen mode */
	case GLUT_KEY_F1:
		isFullScreen = !isFullScreen;
		if (isFullScreen)
		{
			theWindowPositionX = glutGet((GLenum)(GLUT_WINDOW_X));
			theWindowPositionY = glutGet((GLenum)(GLUT_WINDOW_Y));
			glutFullScreen();
		}
		else
		{
			glutReshapeWindow(theWindowWidth, theWindowHeight);
			glutPositionWindow(theWindowPositionX, theWindowPositionY);
		}
		break;
	}

	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}

/* pre:  glut window has just been iconified or restored 
   post: if window is visible, animate model, otherwise don't bother */
static void onVisible(int state)
{
	if (state == GLUT_VISIBLE)
	{
		/* tell glut to show model again */
		glutIdleFunc(onIdle);
	}
	else
	{
		glutIdleFunc(NULL);
	}
}

static void InitializeGlutCallbacks()
{
	/* tell glut how to display model */
	glutDisplayFunc(onDisplay);
	/* tell glutwhat to do when it would otherwise be idle */
	glutIdleFunc(onIdle);
	/* tell glut how to respond to changes in window size */
	glutReshapeFunc(onReshape);
	/* tell glut how to handle changes in window visibility */
	glutVisibilityFunc(onVisible);
	/* tell glut how to handle key presses */
	glutKeyboardFunc(onAlphaNumericKeyPress);
	glutSpecialFunc(onSpecialKeyPress);
	/* tell glut how to handle the mouse */
	glutMotionFunc(onMouseMotion);
	glutMouseFunc(onMouseButtonPress);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	/* request initial window size and position on the screen */
	glutInitWindowSize(theWindowWidth, theWindowHeight);
	glutInitWindowPosition(theWindowPositionX, theWindowPositionY);
	/* request full color with double buffering and depth-based rendering */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	/* create window whose title is the name of the executable */
	glutCreateWindow(theProgramTitle);

	InitializeGlutCallbacks();

	// Must be done after glut is initialized!
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	printf("GL version: %s\n", glGetString(GL_VERSION));

	/* initialize model */
	onInit(argc, argv);

	/* give control over to glut to handle rendering and interaction  */
	glutMainLoop();

	/* program should never get here */

	return 0;
}

// 2 2 2
// 255
// 0
// 0
// 255
// 0
// 255
// 255
// 0

// 3 3 3
// 255
// 0
// 0
// 255
// 0
// 255
// 255
// 0
// 255
// 0
// 0
// 255
// 0
// 255
// 255
// 0
// 255
// 0
// 0
// 255
// 0
// 255
// 255
// 0
// 255
// 0
// 0
