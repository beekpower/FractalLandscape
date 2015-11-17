#include <stdio.h>   // use as needed for your system
#include <math.h>
#include <GL/glut.h>
#define getrandom(min, max) ((rand()%(int)(((max) + 1)-(min)))+ (min))

class Point2   //single point w/ floating point coordinates
{
public:
	float x, y;
	Point2() { x = y = 0.0f; } //constructor 1
	Point2(float xx, float yy) { x = xx; y = yy; } //constructor 2
	void set(float xx, float yy) { x = xx; y = yy; }
	float getX() { return x; }
	float getY() { return y; }
	void draw(void)
	{
		glBegin(GL_POINTS); //draw this point
		glVertex2f((GLfloat)x, (GLfloat)y);
		glEnd();
	}

};

enum LEAF_COLORS {GREEN, PINK, ORANGE, WHITE};
LEAF_COLORS leafColor;

Point2 CP;      //current turtle point
float CD = 90;  //current turtle direction

				//string production rules
char atom[25] = "FX";                      //starting string
char Fstr[30] = "FF-[---F+F+F+L]+[+++F-F-F+L]";;   //F production rule
char Xstr[25] = "FF+[+F]+[-F]";                       //X production rule
char Ystr[25] = "";                       //Y production rule
float angle = 22;                         //turn angle
int length = 20;                          //forward length

float leafScale = 3.5;

int count = 0;

//array of current turtle locations
//better to use a linked list for these :)
//These arrays will allow a maximum of 100 nested [] pairs.   Should be heaps!!
Point2 saveTut[100];
float saveAng[100];
int curr = 0;


//-----TURTLE DRAWING FUNCTIONS- TAKEN OUT OF CANVAS.H------------------
void moveTo(float x, float y)
{
	CP.set(x, y);
}

void drawLeaf()
{
	switch (leafColor)
	{
	case GREEN:
		glColor3f(0.1, 1, 0.1);
		break;

	case PINK:
		glColor3f(1.0, 0.5, 1.0);
		break;

	case ORANGE:
		glColor3f(1.0, 0.5, 0.0);
		break;

	case WHITE:
		glColor3f(1.0, 1.0, 1.0);
		break;
	}
	glPointSize(5);
	glPushMatrix();
	glTranslated(0.0, 0.0, 1.0);
	glBegin(GL_POLYGON);
	glVertex2f(CP.x - (8* leafScale), CP.y + (2* leafScale));
	glVertex2f(CP.x, CP.y + (10* leafScale));
	glVertex2f(CP.x + (8* leafScale), CP.y + (2* leafScale));
	glVertex2f(CP.x + (3* leafScale), CP.y - (6* leafScale));
	glVertex2f(CP.x - (3* leafScale), CP.y - (6* leafScale));
	glEnd();
	glPopMatrix();
	glColor3f(0.5, 0.25, 0);
}

void lineTo(float x, float y)
{
	glBegin(GL_LINES);
	glVertex2f((GLfloat)CP.getX(), (GLfloat)CP.getY());
	glVertex2f((GLfloat)x, (GLfloat)y); //draw the line
	glEnd();
	CP.set(x, y); //update current point to new point
	glFlush();
}

void turnTo(float angle) { CD = angle; }
void turn(float angle) { CD += angle; }
void forward(float dist, int isVisible)
{
	const float rpd = 0.017453393;
	float x1 = CP.x + dist*cos(rpd*CD);
	float y1 = CP.y + dist*sin(rpd*CD);
	y1 += getrandom(0, 9);
	x1 += getrandom(0, 8);
	if (isVisible)
		lineTo(x1, y1);
	else
		moveTo(x1, y1);
}

void saveTurtle()
{
	saveTut[curr].set(CP.x, CP.y);
	saveAng[curr] = CD;
	curr++;
}

void restoreTurtle()
{
	curr--;
	moveTo(saveTut[curr].getX(), saveTut[curr].getY());
	turnTo(saveAng[curr]);
}

//Draw L-System from String--------------------------------
void produceString(char *st, int order, int draw)
{
	float sx, sy, sa;
	glLineWidth(order + 3.2);
	while (*st != '\0')		// scan through each character
	{
		switch (*st)
		{
		case '[': saveTurtle(); break;
		case ']': restoreTurtle(); break;
		case '+': turn(-angle); break;	// right turn
		case '-': turn(angle); break;  // left turn
		case 'F': if (order > 0)
			produceString(Fstr, order - 1, draw);
				  else
					  forward(length, draw);
				  break;
		case 'X': if (order > 0)
			produceString(Xstr, order - 1, draw); break;
		case 'Y': if (order > 0)
			produceString(Ystr, order - 1, draw); break;
		case 'L':
			if (order <= 0)
				drawLeaf();
			break;
		}
		st++;
	}
}


//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
//void myDisplay(void)
//{
//	CP.set(0, 0);
//	glClearColor(0.0, 0.0, 0.0, 0.0);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glClear(GL_COLOR_BUFFER_BIT);
//	glColor3f(1, 1, 0);
//	glLineWidth(1);
//
//	//this time draw the curve
//	moveTo(0.0, 0.0);
//	turnTo(90);
//	produceString(atom, 3, 1);
//}
