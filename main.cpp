
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>

GLfloat  movkx = 0.0 ,movky = 5.0 ,movkz = 50.0 ;
float s = 2;
float angle = 0.0;
int state = 1;

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
case 'q':
    movkx =movkx - 0.5  ;
    glutPostRedisplay() ;//Cette fonction indique que la fenêtre courante doit être réaffiché.
    break ;
case 'd':
    movkx =movkx + 0.5 ;
    glutPostRedisplay() ;
    break ;

case 'y':
    movky =movky - 0.5  ;
    glutPostRedisplay() ;
    break ;
case 'u':
    movky =movky + 0.5 ;
    glutPostRedisplay() ;
    break ;
case 's':
    movkz =movkz - 0.5  ;
    glutPostRedisplay() ;
    break ;
case 'z':
    movkz =movkz + 0.5 ;
    glutPostRedisplay() ;
    break ;
case 27 :
    exit(1) ;
  }
 }
void maCamera()
{
glMatrixMode( GL_PROJECTION );          /* (activer la matrice de projection)*/
glLoadIdentity();                       /* La matrice de projection est la matrice identitée */

//     Activer la projection en perspective avec fovy, aspect, zNear et zFar
//     gluPerspective (fovy, aspect , zNear , zFar)
//     fovy: angle du champ de vision (dans [0°, 180°])
//     aspect : rapport largeur/hauteur du plan de devant
//     zNear et zFar: distances du point de vue aux plans de clipping.
gluPerspective( 60.0, 1, 0.1, 78.0 );

//la fonction gluLookAt(), permettant de positionner lobservateur, sa direction de regard ainsi que lorientation de sa « tête ».
//gluLookAt( GLdouble Px, GLdouble Py, GLdouble Pz, // position de lappareil
//           GLdouble Cx, GLdouble Cy, GLdouble Cz, // point visé dans la scène
//           GLdouble Hx, GLdouble Hy, GLdouble Hz) // haut de lappareil
//    gluLookAt(px,py,pz,px+sin(angle),py,pz+cos(angle),0.0,1.0,0.0);

//gluLookAt( movkx ,movky , movkz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
    gluLookAt(movkx,movky,movkz,0,0,0,0,1,0);
}


void reshape(int w, int h){
    glViewport(0,0,w,h);
    /*
    glMatrixMode(GL_PROJECTION);
    
    glLoadIdentity();
    gluPerspective(60,1,2.0,35.0);
    
    */
    maCamera();
    glMatrixMode(GL_MODELVIEW);
}

void init(){
    glClearColor(0.22,0.25,0.34,1.0);
    glEnable(GL_DEPTH_TEST);
}

float x_position= 0;
void timer(int){
    glutPostRedisplay();
    glLoadIdentity();
    
    angle += 2;
    s += 3;
    switch(state){
        case 1:
            if(x_position < 10){
                x_position += .12;
            }else{
                state = -1;
            };
            break;
        case -1: 
            if(x_position > -20){
                x_position -= .12;
            }else{
                state = 1;
            };
            break;
    }
    glutTimerFunc(1000/60,timer,0);
    
}

void myGame(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0,0,-14);
    glRotatef(angle,1.0,1.0,1.0);
    glPointSize(7);
    glTranslatef(0,0,2);
    glColor3f(1,1,1);
    glutSolidSphere(4,33,33);
    glLoadIdentity();
    glColor3f(0.32,0.35,0.44);
    glutSolidSphere(4.1,55,55);
    /*
   glBegin(GL_QUADS);

glColor3f(   1.0,  1.0, 1.0 );
glVertex3f(  0.5, -0.5, 0.5 );
glVertex3f(  0.5,  0.5, 0.5 );
glVertex3f( -0.5,  0.5, 0.5 );
glVertex3f( -0.5, -0.5, 0.5 );

// Purple side - RIGHT
glColor3f(  1.0,  0.0,  1.0 );
glVertex3f( 0.5, -0.5, -0.5 );
glVertex3f( 0.5,  0.5, -0.5 );
glVertex3f( 0.5,  0.5,  0.5 );
glVertex3f( 0.5, -0.5,  0.5 );

// Green side - LEFT
glColor3f(   0.0,  1.0,  0.0 );
glVertex3f( -0.5, -0.5,  0.5 );
glVertex3f( -0.5,  0.5,  0.5 );
glVertex3f( -0.5,  0.5, -0.5 );
glVertex3f( -0.5, -0.5, -0.5 );

// Blue side - TOP
glColor3f(   0.0,  0.0,  1.0 );
glVertex3f(  0.5,  0.5,  0.5 );
glVertex3f(  0.5,  0.5, -0.5 );
glVertex3f( -0.5,  0.5, -0.5 );
glVertex3f( -0.5,  0.5,  0.5 );

// Red side - BOTTOM
glColor3f(   1.0,  0.0,  0.0 );
glVertex3f(  0.5, -0.5, -0.5 );
glVertex3f(  0.5, -0.5,  0.5 );
glVertex3f( -0.5, -0.5,  0.5 );
glVertex3f( -0.5, -0.5, -0.5 );
glEnd();
 */
    glFlush();
}



int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(200,100);
    glutInitWindowSize(700,1000);

    glutCreateWindow("first shoot");
    glutDisplayFunc(myGame);
    glutReshapeFunc(reshape); 
    glutKeyboardFunc(keyboard);

    glutTimerFunc(1,timer,0);
    init();

    glutMainLoop();

    return EXIT_SUCCESS;

}
