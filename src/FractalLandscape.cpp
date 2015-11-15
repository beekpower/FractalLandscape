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

#define MW 80
#define MH 80
GLfloat Mesh[MH][MW][3]; //make a mesh 100 x 100 with x, y and z
Vector3 Normals[MW][MH];

GLuint landscapeTile;

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
//take the mesh and draw it using GL_QUADS
void generateMesh()
{
    landscapeTile = glGenLists(1);
    glNewList(landscapeTile, GL_COMPILE );
    
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
            
            if(yValue < 0 && yValue > -0.3) {
                //yValue += 1;
                // below sealevel
                //glColor3ub(205,133,63);
                
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
                glColor3ub(239.67,227.47,59.2);
            } else {
                //glColor3f(0, yValue + 0.4, 0);
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
//initialise the display settings
void myInit()
{
    createMesh();
    generateMesh();
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
    cam.slide(0,0, 1.5);
    cam.setShape(30.0f, 1.0f, 0.5f, 50.0f);
}
//glutDisplayFunc
//notice most of the set up values are not in here but in myInit
void displayMesh(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1,1,1);
    glPushMatrix();
        glTranslated(1.9,0, 0);
        glCallList(landscapeTile);
    glPopMatrix();
    glPushMatrix();
        glCallList(landscapeTile);
    glPopMatrix();
    glPushMatrix();
        glTranslated(-1.9,0, 0);
        glCallList(landscapeTile);
    glPopMatrix();
    
    glPushMatrix();
        glTranslated(0,0, -1.9);
        glCallList(landscapeTile);
    glPopMatrix();
    
    glPushMatrix();
        glTranslated(0,0, 1.9);
        glCallList(landscapeTile);
    glPopMatrix();
    
    glPushMatrix();
        glTranslated(1.9,0, 1.9);
        glCallList(landscapeTile);
    glPopMatrix();
    
    glPushMatrix();
        glTranslated(-1.9,0, 1.9);
        glCallList(landscapeTile);
    glPopMatrix();
    
    glPushMatrix();
        glTranslated(-1.9,0, -1.9);
        glCallList(landscapeTile);
    glPopMatrix();
    
    glPushMatrix();
        glTranslated(1.9,0, -1.9);
        glCallList(landscapeTile);
    glPopMatrix();

    glPushMatrix();
    glColor4f(0, 0, .8, 0.4);
    glTranslated(0,-0.25, 0);
    glScaled(23, .12, 23);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glFlush();
    glutSwapBuffers();
}

// Some handles for controlling what direction is being pressed
bool lookLeft = false;
bool lookRight = false;
bool lookUp = false;
bool lookDown = false;
bool rollLeft = false;
bool rollRight = false;

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

int lastx = 0;
float currentCameraSpeed = 0;

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
            currentCameraSpeed = -0.005; //forward movement amount
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

float lookSpeed = 0.3;
void idleFlying()
{
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
    glutKeyboardUpFunc(myKeyboardUp);
    glutMotionFunc(myMovedMouse);
    glutMouseFunc(myMouse);
    glutIdleFunc(idleFlying);
    //turn on the lights
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
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


