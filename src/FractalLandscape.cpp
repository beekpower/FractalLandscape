//
//  FractalLanscape.cpp
//  FractalLandscape
//

//other header files
#include <stdio.h>

#ifdef __linux__
#include "GL/freeglut.h"
#include "GL/gl.h"
#elif _WIN32
#include "GL\freeglut.h"
#elif __APPLE__
#include <freeglut.h>
#endif



#include "camera.h" //download from course website
#include "FractalTree.h"

#define MW 80
#define MH 80
GLfloat Mesh[MH][MW][3]; //make a mesh 100 x 100 with x, y and z
Vector3 Normals[MW][MH];

GLuint landscapeWinterTile;
GLuint landscapeFallTile;
GLuint landscapeSpringTile;
GLuint landscapeSummerTile;
GLuint trees_sp;
GLuint trees_su;
GLuint trees_au;
GLuint trees_wi;
GLuint all_trees;

GLuint cloud;

float fallMod = 0.0;
float fallMod2 = 0.0;

int lastx = 0;
float currentCameraSpeed = 0;
float lookSpeed = 0.5;

// Some handles for controlling what direction is being pressed
bool lookLeft = false;
bool lookRight = false;
bool lookUp = false;
bool lookDown = false;
bool rollLeft = false;
bool rollRight = false;

Camera cam; // global camera object
//quick and nasty normal calculation
Vector3 newell4(GLfloat pt[4][3])
{
    Vector3 m;
    for(int i = 0; i < 4 ; i++)
    {int next = (i== 3) ? 0 : i + 1;
        m.x += (pt[i][1] - pt[next][1]) *
        (pt[i][2] + pt[next][2]);
        m.y += (pt[i][2] - pt[next][2])
        * (pt[i][0] + pt[next][0]);
        m.z += (pt[i][0] - pt[next][0])
        * (pt[i][1] + pt[next][1]);
    }
    m.normalize();
    return m;
}
//calculate x, y and z coordinates for the mesh vertices
//y is the height
void createMesh()
{
    //set Mesh points x and z will range from [-1,1]
    float xinc = 2/(float)MH;
    float zinc = 2/ (float)MW;
    float x = -1;
    float z = -1;
    for(int i = 0; i < MH; i++)
    {
        for(int j = 0; j < MW; j++)
        {

            Mesh[i][j][0] = x;
            Mesh[i][j][2] = z;
            Mesh[i][j][1] = ((cos(x * 3) / 10.0 + sin((z+.5) * 3) / 10.0 + rand() % 5 / 1050.0) * 3) - 0.1;
			x += xinc;
        }
        z+= zinc;
        x = -1;
    }
    //calculate normals
    GLfloat pt[4][3];
    for(int i = 0; i < MH-1; i++)
    {
        for(int j = 0; j < MW-1; j++)
        {
            pt[0][0] = Mesh[i][j][0];
            pt[0][1] = Mesh[i][j][1];
            pt[0][2] = Mesh[i][j][2];
            pt[1][0] = Mesh[i+1][j][0];
            pt[1][1] = Mesh[i+1][j][1];
            pt[1][2] = Mesh[i+1][j][2];
            pt[2][0] = Mesh[i+1][j+1][0];
            pt[2][1] = Mesh[i+1][j+1][1];
            pt[2][2] = Mesh[i+1][j+1][2];
            pt[3][0] = Mesh[i][j+1][0];
            pt[3][1] = Mesh[i][j+1][1];
            pt[3][2] = Mesh[i][j+1][2];
            Normals[i][j] = newell4(pt);
        }
    }
}

void generateWinterMesh()
{
    landscapeWinterTile = glGenLists(1);
    glNewList(landscapeWinterTile, GL_COMPILE );

    double min = 9999;
    double max = 0;
    glPushMatrix();
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    for(int i = 0; i < MH-1; i++)
    {
        for(int j = 0; j < MW-1; j++)
        {


            double yValue = Mesh[i][j][1];
            double xValue = Mesh[i][j][0];
            double zValue = Mesh[i][j][2];
            if(yValue > max) {
                max = yValue;
            }
            if(yValue < min) {
                min = yValue;
            }

            if(yValue > 0) {
                glColor3ub(230,232,227);
            } else if(yValue < 0 && yValue > -0.3) {
                // transition from seafloor to grass
                float adjustedRed = (fabs(yValue) * 850.0f);
                if(adjustedRed < 0) {
                    adjustedRed = 0;
                }
                float adjustedGreen = 102.0f + (fabs(yValue) * 445.0f);
                if(adjustedGreen < 102) {
                    adjustedGreen = 102;
                }
                float adjustedBlue = (fabs(yValue) * 210.0f);
                if(adjustedGreen < 0) {
                    adjustedGreen = 0;
                }
                glColor3ub(adjustedRed,adjustedGreen,adjustedBlue);
            } else if(yValue < -0.3) {
                // seafloor yellowish
                glColor3ub(239.67,227.47,59.2);
            } else {
                //glColor3f(0, yValue + 0.4, 0);
                // normal grass green
                glColor3ub(0,102,0);
            }
            // let's draw the land
            glBegin(GL_QUADS);
            glNormal3f(Normals[i][j].x,
                       Normals[i][j].y, Normals[i][j].z);
            glVertex3f(Mesh[i][j][0],Mesh[i][j][1],
                       Mesh[i][j][2]);
            glNormal3f(Normals[i][j+1].x, Normals[i][j+1].y,
                       Normals[i][j+1].z);
            glVertex3f(Mesh[i][j+1][0],Mesh[i][j+1][1],
                       Mesh[i][j+1][2]);
            glNormal3f(Normals[i+1][j+1].x,
                       Normals[i+1][j+1].y, Normals[i+1][j+1].z);
            glVertex3f(Mesh[i+1][j+1][0],Mesh[i+1][j+1][1],
                       Mesh[i+1][j+1][2]);
            glNormal3f(Normals[i+1][j].x, Normals[i+1][j].y,
                       Normals[i+1][j].z);
            glVertex3f(Mesh[i+1][j][0],Mesh[i+1][j][1],
                       Mesh[i+1][j][2]);
            glEnd();
        }
    }
    glPopMatrix();
    glEndList();
}

void generateFallMesh()
{
    landscapeFallTile = glGenLists(1);
    glNewList(landscapeFallTile, GL_COMPILE );

    double min = 9999;
    double max = 0;
    glPushMatrix();
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    for(int i = 0; i < MH-1; i++)
    {
        for(int j = 0; j < MW-1; j++)
        {


            double yValue = Mesh[i][j][1];
            double xValue = Mesh[i][j][0];
            double zValue = Mesh[i][j][2];
            if(yValue > max) {
                max = yValue;
            }
            if(yValue < min) {
                min = yValue;
            }

            if(yValue > 0 && yValue < .3) {
                //go from (0,102,0) -> (139,69,19)
                float adjustedRed = yValue * 850.0f;
                float adjustedGreen = 102.0f - (yValue * 82.5f);
                float adjustedBlue = yValue * 47.5f;
                glColor3ub(adjustedRed,adjustedGreen,adjustedBlue);
            } else if(yValue > 0.3) {
                glColor3ub(230,232,227);
            } else if(yValue < 0 && yValue > -0.3) {
                // transition from seafloor to grass
                float adjustedRed = (fabs(yValue) * 850.0f);
                if(adjustedRed < 0) {
                    adjustedRed = 0;
                }
                float adjustedGreen = 102.0f + (fabs(yValue) * 445.0f);
                if(adjustedGreen < 102) {
                    adjustedGreen = 102;
                }
                float adjustedBlue = (fabs(yValue) * 210.0f);
                if(adjustedGreen < 0) {
                    adjustedGreen = 0;
                }
                glColor3ub(adjustedRed,adjustedGreen,adjustedBlue);
            } else if(yValue < -0.3) {
                // seafloor yellowish
                glColor3ub(239.67,227.47,59.2);
            } else {
                //glColor3f(0, yValue + 0.4, 0);
                // normal grass green
                glColor3ub(0,102,0);
            }
            // let's draw the land
            glBegin(GL_QUADS);
            glNormal3f(Normals[i][j].x,
                       Normals[i][j].y, Normals[i][j].z);
            glVertex3f(Mesh[i][j][0],Mesh[i][j][1],
                       Mesh[i][j][2]);
            glNormal3f(Normals[i][j+1].x, Normals[i][j+1].y,
                       Normals[i][j+1].z);
            glVertex3f(Mesh[i][j+1][0],Mesh[i][j+1][1],
                       Mesh[i][j+1][2]);
            glNormal3f(Normals[i+1][j+1].x,
                       Normals[i+1][j+1].y, Normals[i+1][j+1].z);
            glVertex3f(Mesh[i+1][j+1][0],Mesh[i+1][j+1][1],
                       Mesh[i+1][j+1][2]);
            glNormal3f(Normals[i+1][j].x, Normals[i+1][j].y,
                       Normals[i+1][j].z);
            glVertex3f(Mesh[i+1][j][0],Mesh[i+1][j][1],
                       Mesh[i+1][j][2]);
            glEnd();
        }
    }
    glPopMatrix();
    glEndList();
}

void generateSummerMesh()
{
    landscapeSummerTile = glGenLists(1);
    glNewList(landscapeSummerTile, GL_COMPILE );

    double min = 9999;
    double max = 0;
    glPushMatrix();
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    for(int i = 0; i < MH-1; i++)
    {
        for(int j = 0; j < MW-1; j++)
        {


            double yValue = Mesh[i][j][1];
            double xValue = Mesh[i][j][0];
            double zValue = Mesh[i][j][2];
            if(yValue > max) {
                max = yValue;
            }
            if(yValue < min) {
                min = yValue;
            }

             if(yValue > 0.4) {
                //glColor3ub(230,232,227);
            } else if(yValue < 0 && yValue > -0.3) {
                // transition from seafloor to grass
                float adjustedRed = (fabs(yValue) * 850.0f);
                if(adjustedRed < 0) {
                    adjustedRed = 0;
                }
                float adjustedGreen = 102.0f + (fabs(yValue) * 445.0f);
                if(adjustedGreen < 102) {
                    adjustedGreen = 102;
                }
                float adjustedBlue = (fabs(yValue) * 210.0f);
                if(adjustedGreen < 0) {
                    adjustedGreen = 0;
                }
                glColor3ub(adjustedRed,adjustedGreen,adjustedBlue);
            } else if(yValue < -0.3) {
                // seafloor yellowish
                glColor3ub(239.67,227.47,59.2);
            } else {
                //glColor3f(0, yValue + 0.4, 0);
                // normal grass green
                glColor3ub(0,102,0);
            }
            // let's draw the land
            glBegin(GL_QUADS);
            glNormal3f(Normals[i][j].x,
                       Normals[i][j].y, Normals[i][j].z);
            glVertex3f(Mesh[i][j][0],Mesh[i][j][1],
                       Mesh[i][j][2]);
            glNormal3f(Normals[i][j+1].x, Normals[i][j+1].y,
                       Normals[i][j+1].z);
            glVertex3f(Mesh[i][j+1][0],Mesh[i][j+1][1],
                       Mesh[i][j+1][2]);
            glNormal3f(Normals[i+1][j+1].x,
                       Normals[i+1][j+1].y, Normals[i+1][j+1].z);
            glVertex3f(Mesh[i+1][j+1][0],Mesh[i+1][j+1][1],
                       Mesh[i+1][j+1][2]);
            glNormal3f(Normals[i+1][j].x, Normals[i+1][j].y,
                       Normals[i+1][j].z);
            glVertex3f(Mesh[i+1][j][0],Mesh[i+1][j][1],
                       Mesh[i+1][j][2]);
            glEnd();
        }
    }
    glPopMatrix();
    glEndList();
}

void generateSpringMesh()
{
    landscapeSpringTile = glGenLists(1);
    glNewList(landscapeSpringTile, GL_COMPILE );

    double min = 9999;
    double max = 0;
    glPushMatrix();
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    for(int i = 0; i < MH-1; i++)
    {
        for(int j = 0; j < MW-1; j++)
        {


            double yValue = Mesh[i][j][1];
            double xValue = Mesh[i][j][0];
            double zValue = Mesh[i][j][2];
            if(yValue > max) {
                max = yValue;
            }
            if(yValue < min) {
                min = yValue;
            }

            if(yValue > 0 && yValue < .3) {
                //go from (0,102,0) -> (139,69,19)
                float adjustedRed = yValue * 850.0f;
                float adjustedGreen = 102.0f - (yValue * 82.5f);
                float adjustedBlue = yValue * 47.5f;
                glColor3ub(adjustedRed,adjustedGreen,adjustedBlue);
            }  else if(yValue > 0.3) {
                glColor3ub(230,232,227);
            } else if(yValue < 0 && yValue > -0.3) {
                // transition from seafloor to grass
                float adjustedRed = (fabs(yValue) * 850.0f);
                if(adjustedRed < 0) {
                    adjustedRed = 0;
                }
                float adjustedGreen = 102.0f + (fabs(yValue) * 445.0f);
                if(adjustedGreen < 102) {
                    adjustedGreen = 102;
                }
                float adjustedBlue = (fabs(yValue) * 210.0f);
                if(adjustedGreen < 0) {
                    adjustedGreen = 0;
                }
                glColor3ub(adjustedRed,adjustedGreen,adjustedBlue);
            } else if(yValue < -0.3) {
                // seafloor yellowish
                glColor3ub(239.67,227.47,59.2);
            } else {
                //glColor3f(0, yValue + 0.4, 0);
                // normal grass green
                glColor3ub(0,102,0);
            }
            // let's draw the land
            glBegin(GL_QUADS);
            glNormal3f(Normals[i][j].x,
                       Normals[i][j].y, Normals[i][j].z);
            glVertex3f(Mesh[i][j][0],Mesh[i][j][1],
                       Mesh[i][j][2]);
            glNormal3f(Normals[i][j+1].x, Normals[i][j+1].y,
                       Normals[i][j+1].z);
            glVertex3f(Mesh[i][j+1][0],Mesh[i][j+1][1],
                       Mesh[i][j+1][2]);
            glNormal3f(Normals[i+1][j+1].x,
                       Normals[i+1][j+1].y, Normals[i+1][j+1].z);
            glVertex3f(Mesh[i+1][j+1][0],Mesh[i+1][j+1][1],
                       Mesh[i+1][j+1][2]);
            glNormal3f(Normals[i+1][j].x, Normals[i+1][j].y,
                       Normals[i+1][j].z);
            glVertex3f(Mesh[i+1][j][0],Mesh[i+1][j][1],
                       Mesh[i+1][j][2]);
            glEnd();
        }
    }
    glPopMatrix();
    glEndList();
}

void produceTrees_summer()
{
	trees_su = glGenLists(1);
	glNewList(trees_su, GL_COMPILE);
	leafColor = GREEN;
	leafScale = 3.5;
	glColor3f(0.5, 0.5, 0);
	moveTo(0.0, 0.0);
	turnTo(90);
	produceString(Fstr, 2, 1);
	glEndList();
}

void produceTrees_spring()
{
	trees_sp = glGenLists(1);
	glNewList(trees_sp, GL_COMPILE);
	leafColor = PINK;
	leafScale = 2.0;
	glColor3f(0.5, 0.5, 0);
	moveTo(0.0, 0.0);
	turnTo(90);
	produceString(Fstr, 2, 1);
	glEndList();
}

void produceTrees_autumn()
{
	trees_au = glGenLists(1);
	glNewList(trees_au, GL_COMPILE);
	leafColor = ORANGE;
	leafScale = 3.5;
	glColor3f(0.5, 0.5, 0);
	moveTo(0.0, 0.0);
	turnTo(90);
	produceString(Fstr, 2, 1);
	glEndList();
}

void produceTrees_winter()
{
	trees_wi = glGenLists(1);
	glNewList(trees_wi, GL_COMPILE);
	leafColor = WHITE;
	leafScale = 1.8;
	glColor3f(0.5, 0.5, 0);
	moveTo(0.0, 0.0);
	turnTo(90);
	produceString(Fstr, 2, 1);
	glEndList();
}

void drawTree(int leafSize, LEAF_COLORS type, float x, float y, float z)
{
	glPushMatrix();
	glScalef(0.0003, 0.0003, 0.0003);
	glTranslatef(x, y, z);
	switch (type)
	{
	case GREEN:
		glCallList(trees_su);
		glRotatef(90, 0, 1, 0);
		glCallList(trees_su);
		break;
	case PINK:
		glCallList(trees_sp);
		glRotatef(90, 0, 1, 0);
		glCallList(trees_sp);
		break;
	case ORANGE:
		glCallList(trees_au);
		glRotatef(90, 0, 1, 0);
		glCallList(trees_au);
		break;
	case WHITE:
		glCallList(trees_wi);
		glRotatef(90, 0, 1, 0);
		glCallList(trees_wi);
		break;
	default:
		break;
	}

	glPopMatrix();
}

void drawTrees()
{
	all_trees = glGenLists(1);
	glNewList(all_trees, GL_COMPILE);
	float xOffset = 0.0;
	float zOffset = 0.0;
	{
		drawTree(3.5, ORANGE, (1940 + xOffset), 500, (0 + zOffset));
		drawTree(3.5, ORANGE, (-1940 + xOffset), 500, (0 + zOffset));
		drawTree(3.5, ORANGE, (0 + xOffset), 500, (1940 + zOffset));
		drawTree(3.5, ORANGE, (0 + xOffset), 500, (-1940 + zOffset));
	}
	xOffset = -6350;
	{
		drawTree(3.5, GREEN, (1940 + xOffset), 500, (0 + zOffset));
		drawTree(3.5, GREEN, (-1940 + xOffset), 500, (0 + zOffset));
		drawTree(3.5, GREEN, (0 + xOffset), 500, (1940 + zOffset));
		drawTree(3.5, GREEN, (0 + xOffset), 500, (-1940 + zOffset));
	}
	xOffset = -6350;
	zOffset = -6350;
	{
		drawTree(3.5, PINK, (1940 + xOffset), 500, (0 + zOffset));
		drawTree(3.5, PINK, (-1940 + xOffset), 500, (0 + zOffset));
		drawTree(3.5, PINK, (0 + xOffset), 500, (1940 + zOffset));
		drawTree(3.5, PINK, (0 + xOffset), 500, (-1940 + zOffset));
	}
	xOffset = 0;
	zOffset = -6350;
	{
		drawTree(3.5, WHITE, (1940 + xOffset), 500, (0 + zOffset));
		drawTree(3.5, WHITE, (-1940 + xOffset), 500, (0 + zOffset));
		drawTree(3.5, WHITE, (0 + xOffset), 500, (1940 + zOffset));
		drawTree(3.5, WHITE, (0 + xOffset), 500, (-1940 + zOffset));
	}
	glEndList();
}

void generateCloud() {
  cloud = glGenLists(1);
  glNewList(cloud, GL_COMPILE);
    glBegin(GL_POLYGON);
      glColor3f(1.0,1.0,1.0);
      glVertex3f(1.0,0.7,-5.0);
      glVertex3f(1.3,1.0,-5.0);
      glVertex3f(0.7,1.5,-5.0);
      glVertex3f(0.0,1.5,-5.0);
      glVertex3f(-0.7,1.5,-5.0);
      glVertex3f(-1.4,1.5,-5.0);
      glVertex3f(-1.7,1.0,-5.0);
      glVertex3f(-1.5,0.7,-5.0);
      glVertex3f(-1.0,0.5,-5.0);
    glEnd();
  glEndList();
}

//initialise the display settings
void myInit()
{
    createMesh();
    generateWinterMesh();
    generateFallMesh();
    generateSummerMesh();
    generateSpringMesh();
	produceTrees_spring();
	produceTrees_summer();
	produceTrees_autumn();
	produceTrees_winter();
  generateCloud();
	drawTrees();
    GLfloat lightIntensity[] = { 0.8, 0.8, 0.8, 1.0f};
	GLfloat lightIntensity2[] = { 0.8, 0.8, 0.8, 1.0f };
    GLfloat lightPosition[] = {0.0f, 1.0f, 0.0f, 0.0f};
	GLfloat lightPosition2[] = { 0.0f, -1.0f, 0.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightIntensity2);
    Point3 eye, look;
    Vector3 up;
    eye.set(1,1,1); //set the eye location
    look.set(0,0,0); //set the look at coord
    up.set(0,1,0); //specify the up vector
    cam.set(eye, look, up); // make the initial camera
    cam.slide(0,0, 1.5);
    cam.setShape(30.0f, 1.0f, 0.5f, 50.0f);
}

void drawFallingLeaves()
{
	glPushMatrix();
	glColor3f(0.5, 0.25, 0.0);
	glScalef(0.0003, 0.000025, 0.0003);
	glTranslatef(2150, (3750 * 2) - fallMod, 0);
	glutSolidSphere(20, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5, 0.25, 0.0);
	glScalef(0.0003, 0.000025, 0.0003);
	glTranslatef(1950, (3700 * 2) - fallMod, 0);
	glutSolidSphere(20, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5, 0.25, 0.0);
	glScalef(0.0003, 0.000025, 0.0003);
	glTranslatef(2050, (3850 * 2) - fallMod2, 150);
	glutSolidSphere(20, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5, 0.25, 0.0);
	glScalef(0.0003, 0.000025, 0.0003);
	glTranslatef(2050, (3900 * 2) - fallMod2, -150);
	glutSolidSphere(20, 10, 10);
	glPopMatrix();
}

void processCam() {
  fallMod += 20.0;
  fallMod2 += 30.0;
  if (fallMod >= 1600.0)
    fallMod = 0.0;
  if (fallMod2 >= 1600.0)
    fallMod2 = 0.0;

    cam.slide(0,0,currentCameraSpeed);

    // Let's see if we need to move in and look in any direction
    if(lookLeft) {
        cam.yaw(-lookSpeed);
    } else if(lookRight) {
        cam.yaw(lookSpeed);
    }

    if(lookUp) {
        cam.pitch(-lookSpeed);
    } else if(lookDown) {
        cam.pitch(lookSpeed);
    }

    if(rollLeft) {
        cam.roll(-lookSpeed * 2);
    } else if(rollRight) {
        cam.roll(lookSpeed * 2);
    }	fallMod += 20.0;
      fallMod2 += 30.0;
      if (fallMod >= 1600.0)
        fallMod = 0.0;
      if (fallMod2 >= 1600.0)
        fallMod2 = 0.0;

        cam.slide(0,0,currentCameraSpeed);

        // Let's see if we need to move in and look in any direction
        if(lookLeft) {
            cam.yaw(-lookSpeed);
        } else if(lookRight) {
            cam.yaw(lookSpeed);
        }

        if(lookUp) {
            cam.pitch(-lookSpeed);
        } else if(lookDown) {
            cam.pitch(lookSpeed);
        }

        if(rollLeft) {
            cam.roll(-lookSpeed * 2);
        } else if(rollRight) {
            cam.roll(lookSpeed * 2);
        }
}
//glutDisplayFunc
//notice most of the set up values are not in here but in myInit
void displayMesh(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1,1,1);
    glPushMatrix();
        glTranslated(1.9,0, 0);
        glCallList(cloud);
        glCallList(landscapeWinterTile);
    glPopMatrix();
    glPushMatrix();
        glCallList(landscapeFallTile);
    glPopMatrix();
    glPushMatrix();
        glTranslated(-1.9,0, 0);
        glCallList(landscapeSummerTile);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,0, -1.9);
        glCallList(landscapeFallTile);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,0, 1.9);
        glCallList(landscapeFallTile);
    glPopMatrix();

    glPushMatrix();
        glTranslated(1.9,0, 1.9);
        glCallList(landscapeWinterTile);
    glPopMatrix();

    glPushMatrix();
        glTranslated(-1.9,0, 1.9);
        glCallList(landscapeSummerTile);
    glPopMatrix();

    glPushMatrix();
        glTranslated(-1.9,0, -1.9);
        glCallList(landscapeSummerTile);
    glPopMatrix();

    glPushMatrix();
        glTranslated(1.9,0, -1.9);
        glCallList(landscapeWinterTile);
    glPopMatrix();

    glPushMatrix();
        glTranslated(-3.6,0, -1.9);
        glCallList(landscapeSpringTile);
    glPopMatrix();

    glPushMatrix();
        glTranslated(-3.6,0, 0);
        glCallList(landscapeSpringTile);
    glPopMatrix();

    glPushMatrix();
        glTranslated(-3.6,0, 1.9);
        glCallList(landscapeSpringTile);
    glPopMatrix();

    glPushMatrix();
    glColor4f(0, 0, .8, 0.6);
    glTranslated(0,-0.25, 0);
    glScaled(23, .12, 23);
    glutSolidCube(1.0);
    glPopMatrix();

	glCallList(all_trees);

	drawFallingLeaves();


  processCam();
  glFlush();
  glutPostRedisplay();
}



// Keyboard for handling keyboard key down presses
void myKeyboard(unsigned char key, int x, int y)
{/*
        if(key == 's') {
            cam.slide(0,0, 0.1);
        } else if(key == 'w') {
            cam.slide(0,0, -0.1);
        } */
        if(key == 'w') {
            lookUp = true;
        } else if(key == 's') {
            lookDown = true;
        } else if(key == 'a') {
            lookLeft = true;
        } else if(key == 'd') {
            lookRight = true;
        }

        if(key == 'q') {
            rollLeft = true;
        } else if(key == 'e') {
            rollRight = true;
        }

        if(key == 'h') {
            Point3 eye, look;
            Vector3 up;
            eye.set(1,1,1); //set the eye location
            look.set(0,0,0); //set the look at coord
            up.set(0,1,0); //specify the up vector
            cam.set(eye, look, up); // make the initial camera
            cam.slide(0,0, 1.5);
            cam.setShape(30.0f, 1.0f, 0.5f, 50.0f);
        }
    glutPostRedisplay(); // draw it again
}

void myKeyboardUp(unsigned char key, int x, int y) {
    if(key == 'w') {
        lookUp = false;
    } else if(key == 's') {
        lookDown = false;
    } else if(key =='a') {
        lookLeft = false;
    } else if(key =='d') {
        lookRight = false;
    }

    if(key == 'q') {
        rollLeft = false;
    } else if(key == 'e') {
        rollRight = false;
    }
}

void myMovedMouse(int x, int y)
{
    cam.yaw((x - lastx)/10.0); //divide by 20.0 to make it slower
    lastx = x;
    glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN)
    {
        if(button == GLUT_LEFT_BUTTON)
        {
            lastx = x;
            currentCameraSpeed = -0.035; //forward movement amount
        }
        else if(button == GLUT_RIGHT_BUTTON)
        {
            currentCameraSpeed = 0.01; //backward movement amount
            if(currentCameraSpeed > 0) {
                currentCameraSpeed = 0;
            }
        }
    }

    glutPostRedisplay(); // draw it again
}



void idleFlying()
{

  processCam();
  //  glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(800,800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("fly, you fools");
    glutDisplayFunc(displayMesh);
    glutKeyboardFunc(myKeyboard);
    glutKeyboardUpFunc(myKeyboardUp);
    glutMotionFunc(myMovedMouse);
    glutMouseFunc(myMouse);
    glutIdleFunc(idleFlying);
    //turn on the lights
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor((135.0f/255.0f),(206.0f/255.0f),1,0.0f);
    glColor3f(255.0, 255.0, 255.0);
    glViewport(0,0, 600, 600);
    myInit();
    glutMainLoop();
    return 1;
}
