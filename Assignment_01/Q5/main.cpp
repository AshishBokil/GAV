#include <stdio.h>
#include <iostream>
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
/********************************************************************/
/*   Variables */



char *theProgramTitle = "Sample";
int theWindowWidth = 1500, theWindowHeight = 1000;
int theWindowPositionX = 40, theWindowPositionY = 40;
bool isFullScreen = true;
bool isAnimating = true;
float rotation = 0.00f;
float translation = 0.0f;
float nextX = 0.f, nextY = 0.0f;
float x = 0.0f, y = -1.0f;
Vector3f center = (0, 0, 0);
//int noOfPoints=500000;
GLuint VBO, VAO, VBO2, VAO2;
GLuint gWorldLocation;
int noVertices = 0;
/*model*/
OffModel *model;
Vector3f p[8];
/* Constants */
const int ANIMATION_DELAY = 20; /* milliseconds between rendering */
const char *pVSFileName = "shader.vs";
const char *pFSFileName = "shader.fs";
Vector3f spherePos=(0,0,0);
float x_t=0.05,y_t=0.1,z_t=0.05;

/********************************************************************
  Utility functions
 */
Matrix4f World;

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

static void CreateVertexBuffer()
{

	//For Sphere
	glGenVertexArrays(1, &VAO);
	cout << "VAO: " << VAO << endl;
	glBindVertexArray(VAO);

	model = readOffFile("sphere2.off");
	Vector3f vertices[3 * model->numberOfPolygons];
	cout << model->numberOfPolygons;
	noVertices = 3 * model->numberOfPolygons;
	for (int i = 0; i < model->numberOfPolygons; i++)
	{
		for (int j = 0; j < 3; j++)
		{

			vertices[3 * i + j] = Vector3f((model->vertices[(model->polygons[i]).v[j]]).x,
										   (model->vertices[(model->polygons[i]).v[j]]).y,
										   (model->vertices[(model->polygons[i]).v[j]]).z);
		}
	}

	GL_CALL(glGenBuffers(1, &VBO));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, 0));

	GL_CALL(glBindVertexArray(0));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

	//Initializing cube points
	p[0] = (-0.5, 0.5, -0.5);  // P0
	p[1] = (0.5, 0.5, -0.5);   // P1
	p[2] = (0.5, -0.5, -0.5);  // P2
	p[3] = (-0.5, -0.5, -0.5); // P3
	p[4] = (-0.5, 0.5, 0.5);   // P4
	p[5] = (0.5, 0.5, 0.5);	   // P5
	p[6] = (0.5, -0.5, 0.5);   // P6
	p[7] = (-0.5, -0.5, 0.5);  // P7
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
	//createCubeVertexBuffer();
	CompileShaders();

	/* set to draw in window based on depth  */
	glEnable(GL_DEPTH_TEST);
}
static void drawCube()
{
}


void updateSpherePos(){
	if(abs(spherePos.x)<0.25){
		spherePos.x+=x_t;
	}
	else{
		x_t*=-1;
		spherePos.x+=x_t;
	}
	if(abs(spherePos.y)<0.25){
		spherePos.y+=y_t;
	}
	else{
		y_t*=-1;
		spherePos.y+=y_t;
	}
	if(abs(spherePos.z)<0.25){
		spherePos.z+=z_t;
	}
	else{
		z_t*=-1;
		spherePos.z+=z_t;
	}
//	return spherePos;
}

static void onDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPointSize(5);
	Matrix4f transformcube, transformball, scaleMat, translateMat, rotateMat;
	transformcube.InitIdentity();
	transformball.InitIdentity();

	//scaleMat.InitScaleTransform(0.02f,0.02f,0.02f);
	//transform=scaleMat*transform;

	rotateMat.InitAxisRotateTransform(Vector3f(0, 1, 0), rotation);
	transformcube = rotateMat * transformcube;
	rotation += 0.02;

	translateMat.InitTranslationTransform(0.0f, 0.0f, +1.8f);
	transformcube=translateMat*transformcube;

	Matrix4f persProjection;
	PersProjInfo proj(90.0f, 1.0f, 1.0f, +1.0f, -1.0f); 
	persProjection.InitPersProjTransform(proj);
	transformcube = persProjection * transformcube;


	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &transformcube.m[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_POLYGON);		  //plane1
	glVertex3f(-0.5, 0.5, -0.5);  // P0
	glVertex3f(0.5, 0.5, -0.5);	  // P1
	glVertex3f(0.5, -0.5, -0.5);  // P2
	glVertex3f(-0.5, -0.5, -0.5); // P3
	glEnd();

	glBegin(GL_POLYGON);		 //plane2
	glVertex3f(0.5, 0.5, -0.5);	 // P1
	glVertex3f(0.5, -0.5, -0.5); // P2
	glVertex3f(0.5, -0.5, 0.5);	 // P6
	glVertex3f(0.5, 0.5, 0.5);	 // P5
	glEnd();

	glBegin(GL_POLYGON);		 //plane3
	glVertex3f(-0.5, 0.5, 0.5);	 // P4
	glVertex3f(0.5, 0.5, 0.5);	 // P5
	glVertex3f(0.5, -0.5, 0.5);	 // P6
	glVertex3f(-0.5, -0.5, 0.5); // P7
	glEnd();

	glBegin(GL_POLYGON);		  //plane4
	glVertex3f(-0.5, 0.5, -0.5);  // P0
	glVertex3f(-0.5, -0.5, -0.5); // P3
	glVertex3f(-0.5, -0.5, 0.5);  // P7
	glVertex3f(-0.5, 0.5, 0.5);	  // P4
	glEnd();

	glBegin(GL_POLYGON);		 //plane5
	glVertex3f(-0.5, 0.5, -0.5); // P0
	glVertex3f(0.5, 0.5, -0.5);	 // P1
	glVertex3f(0.5, 0.5, 0.5);	 // P5
	glVertex3f(-0.5, 0.5, 0.5);	 // P4
	glEnd();

	glBegin(GL_POLYGON);		  //plane6
	glVertex3f(0.5, -0.5, -0.5);  // P2
	glVertex3f(-0.5, -0.5, -0.5); // P3
	glVertex3f(-0.5, -0.5, 0.5);  // P7
	glVertex3f(0.5, -0.5, 0.5);	  // P6
	glEnd();

	
	////BALL
	GL_CALL(glBindVertexArray(VAO));
	scaleMat.InitScaleTransform(0.03f, 0.04f, 0.03f);
	transformball = scaleMat * transformball;


	 updateSpherePos();
	translateMat.InitTranslationTransform(spherePos.x, spherePos.y, spherePos.z);
	transformball=translateMat*transformball;	



	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &transformball.m[0][0]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, noVertices);
	GL_CALL(glBindVertexArray(0));

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
		//rotation+=1;
		///translation-=0.01;
		break;

	case '2':
		//	rotation+=1;
		//	translation+=0.01;
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
