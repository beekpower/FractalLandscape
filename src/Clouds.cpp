#include"GL/glut.h"
#include "stdio.h"
#include "GL/gl.h"



void shape(int width,int height)
{
        glViewport(0,0,width,height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45,(double)width/(double)height,1,200);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();

    glPopMatrix();
    glPushMatrix();

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
    glPopMatrix(); 
    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(800,800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Clouds");
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(shape);       
	glEnable(GL_DEPTH_TEST);
    glutMainLoop();

return 0;
}
