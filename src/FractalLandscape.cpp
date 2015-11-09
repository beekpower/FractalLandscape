//
//  FractalLanscape.cpp
//  FractalLandscape
//

//other header files
#include <stdio.h>
#include <freeglut.h>
#include "util.h"
#include "camera.h" //download from course website

#define MW 100
#define MH 100
GLfloat Mesh[MH][MW][3]; //make a mesh 100 x 100 with x, y and z
Vector3 Normals[MW][MH];
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
            Mesh[i][j][1] = cos(x*10)/10.0 +
            sin(z*10)/10.0 + rand()%5/200.0;
            x+= xinc;
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
//take the mesh and draw it using GL_QUADS
void drawMesh()
{
    glPushMatrix();
    for(int i = 0; i < MH-1; i++)
    {
        for(int j = 0; j < MW-1; j++)
        {
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
}
//initialise the display settings
void myInit()
{
    createMesh();
    GLfloat lightIntensity[] = {0.9, 0.9, 0.9, 1.0f};
    GLfloat lightPosition[]={0.0f,1.0f, 0.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
    Point3 eye, look;
    Vector3 up;
    eye.set(1,1,1); //set the eye location
    look.set(0,0,0); //set the look at coord
    up.set(0,1,0); //specify the up vector
    cam.set(eye, look, up); // make the initial camera
    cam.setShape(30.0f, 1.0f, 0.5f, 50.0f);
}
//glutDisplayFunc
//notice most of the set up values are not in here but in myInit
void displayMesh(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1,1,1);
    drawMesh();
    glFlush();
    glutSwapBuffers();
}


void myKeyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
            if(key == 'a') {
                cam.slide(0,0, 0.1);
            } else if(key == 'f') {
                cam.slide(0,0, -0.1);
            }
    }
    glutPostRedisplay(); // draw it again
}

int lastx = 0;
int move = 0;

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
            lastx=x;
            move = -0.1; //forward movement amount
        }
        else if(button == GLUT_RIGHT_BUTTON)
        {
            move = 0.1; //backward movement amount
        }
    }
    else
        move = 0.0;
}

void myFly()
{
    cam.slide(0,0,move);
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(800,800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("fly");
    glutDisplayFunc(displayMesh);
    glutKeyboardFunc(myKeyboard);
    glutMotionFunc(myMovedMouse);
    glutMouseFunc(myMouse);
    //turn on the lights
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor(0.1f,0.1f,0.1f,0.0f);
    glViewport(0,0, 600, 600);
    myInit();
    glutMainLoop();
    return 1;
}


