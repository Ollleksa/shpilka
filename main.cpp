#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
 
static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;
 
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,ar,1,100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}
 
void drawcondensator()
{
    glBegin(GL_QUADS);
 
        glColor3f(1,0,0);
        glNormal3d(0,0,-1);
        glVertex3d(1,1,0.1);
        glVertex3d(1,-1,0.1);
        glVertex3d(-1,-1,0.1);
        glVertex3d(-1,1,0.1);
 
        glColor3f(0,0,1);
        glNormal3d(0,0,1);
        glVertex3d(1,1,-0.1);
        glVertex3d(1,-1,-0.1);
        glVertex3d(-1,-1,-0.1);
        glVertex3d(-1,1,-0.1);
 
    glEnd();
}
 
static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*90.0;
 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1,0,0);
 
    glPushMatrix();
    gluLookAt(0,0,-5,0,0,0,0,1,0);
    glRotated(a,1,1,1);
    drawcondensator();
    glPopMatrix();
    glutSwapBuffers();
}
 
 
static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 : // esc for exit
        case 'q':
            glutLeaveGameMode();
            exit(0);
            break;
    }
 
    glutPostRedisplay();
}
 
static void idle(void)
{
    glutPostRedisplay();
}
 
const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 0.0f, 0.0f, 5.0f, 0.0f };
 
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutGameModeString( "800x600:32@60" ); //the settings for fullscreen mode
    glutEnterGameMode();
 
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
 
    glClearColor(0,0,0,1);
 
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
 
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
 
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
 
    glutMainLoop();
 
    return EXIT_SUCCESS;
}
