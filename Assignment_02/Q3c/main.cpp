#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

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
float rotation = 0.00f, rotateLight = 0.00f,rotateX=0,rotateY=0,rotateZ=0;
float translation = 0.2f;
float nextX = 0.f, nextY = 0.0f;
float x = 0.0f, y = -1.0f;
int choicelight = 0, choice = 0;
//int noOfPoints=500000;
GLuint VBO, VAO, IBO, IAO,VBCube,IBCube,VACube;
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

float xmax,ymax,zmax;
Matrix4f World;
float *vertices;
Vector3f *newvertices;
unsigned int *indices;
ll newindex = 0;
ll v = 50;		///Iso value or scalar value

ll thIndices[6][4] = {
	0, 1, 2, 4,
	0, 1, 3, 4,
	2, 3, 6, 4,
	3, 6, 7, 4,
	1, 3, 7, 4,
	1, 5, 7, 4

};

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
	else if(count==3)
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

static void tetra(ll cell[])
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

static void CreateVertexBuffer()
{
	FILE *input;
	unsigned int x, y, z;
	input = fopen("teapot.txt", "r");
	if (input == NULL)
	{
		cout << "error opening file";
		exit(0);
	}
	fscanf(input, "%d", &x);
	fscanf(input, "%d", &y);
	fscanf(input, "%d", &z);
	cout << x << " " << y << " " << z << endl;

	int max=(x>y)?(x>z?x:z):(y>z?y:z);
	xmax=(float)x/max;
	ymax=(float)y/max;
	zmax=(float)z/max;

	noOfvertices = x * y * z;
	vertices = new float[noOfvertices * 4];
	newvertices = new Vector3f[noOfvertices * 36];
	indices = new unsigned int[noOfvertices * 36];
	for (int k = 0; k < z; k++)
	{
		for (int j = 0; j < y; j++)
		{
			for (int i = 0; i < x; i++)
			{
				ll pos = k * y * x + j * x + i;
				vertices[4 * pos + 0] = ((float)i / (x - 1))*((float)x/max);
				vertices[4 * pos + 1] = ((float)j / (y - 1))*((float)y/max);
				vertices[4 * pos + 2] = ((float)k / (z - 1))*((float)z/max);
				fscanf(input, "%f", &vertices[4 * pos + 3]);
				//	data[t++]=vertices[4 * pos + 3];
			}
		}
	}


	ll newVIndex = 0;
	for (int k = 0; k < z - 1; k++)
	{

		for (int j = 0; j < y - 1; j++)
		{
			for (int i = 0; i < x - 1; i++)
			{
				ll cell[8];

				cell[0] = (i + x * j + x * y * k);
				cell[1] = ((i + 1) + x * j + x * y * k);
				cell[2] = (i + x * (j + 1) + x * y * k);
				cell[3] = ((i + 1) + x * (j + 1) + x * y * k);
				cell[4] = (i + x * j + x * y * (k + 1));
				cell[5] = ((i + 1) + x * j + x * y * (k + 1));
				cell[6] = (i + x * (j + 1) + x * y * (k + 1));
				cell[7] = ((i + 1) + x * (j + 1) + x * y * (k + 1));
				// for(int l=0;l<8;l++)cout<<cell[l]<<" ";
				// cout<<endl;
				tetra(cell);
			}
		}
	}
	//cout<<newVIndex<<endl;
	noOfIndices = newindex;
	int t = 0;
	for (int i = 0; i < newindex; i++)
	{

		if (!(newvertices[i].x >= 0 && newvertices[i].y >= 0 && newvertices[i].z >= 0))
		{
			t++;
			cout << indices[i] << endl;
			cout << newvertices[i].x << " " << newvertices[i].y << " " << newvertices[i].z << endl;
		}
	}
	cout << newindex <<" "<<t<< endl;
	// exit(0);


	///////Generate cube
	Vector3f cubevertices[8]={
		Vector3f(0,0,0),
		Vector3f((float)x/max,0,0),
		Vector3f(0,(float)y/max,0),
		Vector3f((float)x/max,(float)y/max,0),
		Vector3f(0,0,(float)z/max),
		Vector3f((float)x/max,0,(float)z/max),
		Vector3f(0,(float)y/max,(float)z/max),
		Vector3f((float)x/max,(float)y/max,(float)z/max)

	};

	unsigned int cubeindices[24]={
		0,1,
		0,2,
		0,4,
		1,3,
		1,5,
		2,3,
		2,6,
		3,7,
		4,5,
		4,6,
		5,7,
		6,7
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

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
	glGenVertexArrays(1, &VACube);
	glBindVertexArray(VACube);

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

	Matrix4f  scaleMat, translateMat, rotateMat, transformcube;

	transformcube.InitIdentity();

	translateMat.InitTranslationTransform(-xmax/2,-ymax/2,-zmax/2);
	transformcube=translateMat*transformcube;

	rotateMat.InitAxisRotateTransform(Vector3f(0,0,1),M_PI);
	transformcube=rotateMat*transformcube;

	scaleMat.InitScaleTransform(1.3f, 1.3f, 1.3f);
	 transformcube = scaleMat * transformcube;

	if(choice==1){
		rotateMat.InitAxisRotateTransform(Vector3f(1,0,0),rotateX);
		transformcube=rotateMat*transformcube;
	}

	if(choice==2){
		rotateMat.InitAxisRotateTransform(Vector3f(0,1,0),rotateY);
		transformcube=rotateMat*transformcube;
	}

	if(choice==3){
		rotateMat.InitAxisRotateTransform(Vector3f(0,0,1),rotateZ);
		transformcube=rotateMat*transformcube;
	
	}

	translateMat.InitTranslationTransform(0.0f, 0.0f, +1.8f);
	transformcube=translateMat*transformcube;

	Matrix4f persProjection;
	PersProjInfo proj(90.0f, 1.0f, 1.0f, +1.0f, -1.0f);
	persProjection.InitPersProjTransform(proj);
	transformcube = persProjection * transformcube;

	//////Cube
	
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &transformcube.m[0][0]);
	glUniform3f(objColor_location, 0,0,0);
	glBindVertexArray(VACube);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
	
	/////object
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &transformcube.m[0][0]);
	glUniform3f(objColor_location, 1,0,0);
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
		choice=1;
		rotateX+=0.05;
		break;

	case '2':
		choice=2;
		rotateY+=0.05;
		break;

	case '3':
		choice=3;
		rotateZ+=0.05;
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
