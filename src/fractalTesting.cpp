/*	Brian Sardinha
 *	Testing L-system Fractal Vegetation
 *
 */

///////////////DRAW CYLINDER AT LINE 110 DEALS WITH CALCULATING DRAWING B/W 2 POINTS////////////////////////

///////////////FORWARD DEALS AT LINE 164 WITH CALCULATING THE NEXT POINT IN THE TREE TO DRAW TO////////////////////////

#include "GL\freeglut.h"
#include "GL\gl.h"
#include <cstdio>
#include <cmath>

class Point2   //single point w/ floating point coordinates
{
public:
	float x, y, z;
	Point2() { x = y = z = 0.0f; } //constructor 1
	Point2(float xx, float yy, float zz) { x = xx; y = yy; z = zz; } //constructor 2
	void set(float xx, float yy, float zz) { x = xx; y = yy; z = zz; }
	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }
};

class Angle3
{
public:
	float xa;
	float ya;
	float za;
	Angle3() { xa = ya = za = 0.0; }
	void setA(float xx, float yy, float zz) { xa = xx; ya = yy; za = zz; }
};

/*******************************************************************************************************/

Point2 CP;      //current turtle point
Angle3 CD;  //current turtle direction

//string production rules
char atom[25] = "F";                      //starting string
char Fstr[30] = "F<F-[-F>+F]+[^+Fv-F]";   //F production rule
char Xstr[25] = "";                       //X production rule
char Ystr[25] = "";                       //Y production rule
float angle = 22;                         //turn angle
int length = 5;

Point2 saveTut[100];
Angle3 saveAng[100];
int curr = 0;
const float thickness = 0.5;

/*******************************************************************************************************/

float x = 0.0, y = 0.0, z = 0.0, xmod = 1.0, ymod = 3.0, zmod = 1.0;
const float modifier = 2.0;

void init()
{
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//setup the light
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glViewport(0, 0, 600, 600);

	GLfloat lightIntensity[] = { 1.9f, 1.9f, 1.9f, 1.0f };
	GLfloat lightPosition[] = { 2.0f,5.0f,8.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//bounding view box
	glOrtho(-100, 100, -100, 100, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//viewer location and orientation
	gluLookAt(1.3, 1.3, 3, 0.3, 0.25, 0, 0.0, 1.0, 0.0);
}

void myMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			glRotated(10, 1, 0, 0);
			glutPostRedisplay();
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			glRotated(-10, 1, 0, 0);
			glutPostRedisplay();
		}
	}
}

//exit program when ESC pressed
void myKeyboard(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(-1);
}

/****************************************************************************************************/

void drawCylinder(float x1, float y1, float z1, float x2, float y2, float z2)
{
	//calculate distance b/w vec 1 and 2
	//calculate rotation angles
	//translate and rotate accordingly
	//draw the cylinder using glut and values
	float xvec = x2 - x1;
	float yvec = y2 - y1;
	float zvec = z2 - z1;
	float height = sqrt((xvec * xvec) + (yvec * yvec) + (zvec * zvec));
	float theta1 = atan(yvec / zvec);
	float theta2 = atan(xvec / zvec);
	theta1 = 360 * (theta1 / (2 * 3.1415926));
	theta2 = 360 * ( (90 - theta2) / (2 * 3.1415926));

	glPushMatrix();
	{
		glTranslatef(x1, y1, z1);
		glRotatef(theta1, 1.0, 0.0, 0.0);
		glRotatef(theta2, 0.0, 1.0, 0.0);
		glutSolidCylinder(thickness, height, 12, 12);
	}
	glPopMatrix();
}

/****************************************************************************************************/

/*******************************************************************************************************
			TURTLE FUNCTIONS
*******************************************************************************************************/
void moveTo(float x, float y, float z)
{
	CP.set(x, y, z);
}

void lineTo(float x, float y, float z)
{
	//printf("current - x: %f, y: %f, z: %f\n", x, y, z);
	drawCylinder(CP.x, CP.y, CP.z, x, y, z);
	CP.set(x, y, z);

}



void turnToX(float angle) { CD.xa = angle; }
void turnToY(float angle) { CD.ya = angle; }
void turnToZ(float angle) { CD.za = angle; }
void turnX(float angle) { CD.xa += angle; }
void turnY(float angle) { CD.ya += angle; }
void turnZ(float angle) { CD.za += angle; }

void forward(float dist, int isVisible)
{
	const float rpd = 0.017453393;
	float x1 = CP.x + dist*cos(CD.xa);
	float y1 = CP.y + dist*cos(CD.ya);
	float z1 = CP.z + dist*sin(CD.za);

	printf("x1: %f, y1: %f, z1: %f\n", CP.x, CP.y, CP.z);
	if (isVisible)
		lineTo(x1, y1, z1);
	else
		moveTo(x1, y1, z1);
}

void saveTurtle()
{
	saveTut[curr].set(CP.x, CP.y, CP.z);
	saveAng[curr].setA(CD.xa, CD.ya, CD.za);
	curr++;
}

void restoreTurtle()
{
	curr--;
	moveTo(saveTut[curr].getX(), saveTut[curr].getY(), saveTut[curr].getZ());
	turnToX(saveAng[curr].xa);
	turnToY(saveAng[curr].ya);
	turnToZ(saveAng[curr].za);
}

/*******************************************************************************************************/

void produceString(char *st, int order, int draw)
{
	float sx, sy, sa;
	while (*st != '\0')		// scan through each character
	{
		switch (*st)
		{
			//printf("in switch\n");
		case '[':
			saveTurtle(); 
			//printf("1 case\n");
			break;

		case ']':
			restoreTurtle();
			//printf("2 case\n");
			break;

		case '+': 
			turnX(-angle); 
			//printf("3 case\n");
			break;

		case '-':
			turnX(angle);
			//printf("4 case\n");
			break;

		case '^': turnY(-angle); break;
		case 'v': turnY(angle); break;
		case '<': turnZ(-angle); break;
		case '>': turnZ(angle); break;
		case 'F':
			//printf("F case\n");
			if (order > 0)
				produceString(Fstr, order - 1, draw);
			else
				forward(length, 1);
			break;
		case 'X': if (order > 0)
			produceString(Xstr, order - 1, draw); break;
		case 'Y': if (order > 0)
			produceString(Ystr, order - 1, draw); break;
		}
		st++;
	}
}



void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0, 0, 0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3f(0.5, 0.25, 0.0);
	moveTo(0.0, 0.0, 0.0);
	//turnToX(0);
	//turnToY(90);
	//turnToZ(0);
	produceString(Fstr, 2, 1);

	//drawCylinder(-20, -20, 0, 20, 20, 0);

	glFlush();
	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1200, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("A 3D Tree... Maybe...");
	glutDisplayFunc(display);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouse);
	init();
	glutMainLoop();
	return 0;
}