
#include <stdio.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <cerrno>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <math.h>
#define DEF_D 5
float mov_key_x = 0.0;
float mov_key_y = 5.0; 
float mov_key_z = 250.0;
float look_x = 0.0; 
float look_y = 0.0 ;
float look_z = 0.0;

//

float y = 1, x = -30, z = 100; // initially 5 units south of origin
float deltaMove = 0.0; // initially camera doesn't move
// Camera direction
float lx = 0.0, lz = -1; // camera points initially along y-axis
//float angle = 0.0; // angle of rotation for the camera direction
//float deltaAngle = 0.0; // additional angle change when dragging
float angle = 0.0; // angle of rotation for the camera direction
float deltaAngle = 0.0; // additional angle change when dragging

// Mouse drag control
int isDragging = 0; // true when dragging
int xDragStart = 0; // records the x-coordinate when dragging starts
int move=0;
int vertmove=0;

unsigned char header[54];        // Each BMP file begins by a 54-bytes header
unsigned int dataPos;            // Position in the file where the actual data begins
unsigned int width, height;
unsigned int imageSize;            // = width*height*3
unsigned char * data=NULL;        // Actual RGB data


void draw_board()
{
    glColor3f(0.177,0.564,1);
    glBegin(GL_QUADS);
        glVertex3f(0,0,0);
        glVertex3f(1,0,0);
        glVertex3f(1,2,0);
        glVertex3f(0,2,0);
        glVertex3f(9,0,0);
        glVertex3f(10,0,0);
        glVertex3f(10,2,0);
        glVertex3f(9,2,0);
    glEnd();
    glColor3f(0.690,0.878,0.901);
    glBegin(GL_QUADS);
        glVertex3f(0,2,0);
        glVertex3f(10,2,0);
        glVertex3f(10,4,0);
        glVertex3f(0,4,0);
    glEnd();
}

GLuint loadBMP_custom(const char * imagepath)
{
    // Open the file
    FILE * file = fopen(imagepath,"rb");
    if (!file)
    {
        printf("Image could not be opened\n");
        printf("Error %d \n", errno);
        return 0;
    }
    if ( fread(header, 1, 54, file)!=54 )
    { // If not 54 bytes read : problem
        printf("Not a correct BMP file\n");
        return false;
    }
    if ( header[0]!='B' || header[1]!='M' )
    {
        printf("Not a correct BMP file\n");
        return 0;
    }
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);
    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)
        imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)
        dataPos=54; // The BMP header is done that way
    // Create a buffer
    data = new unsigned char [imageSize];
    // Read the actual data from the file into the buffer
    fread(data,1,imageSize,file);
    //Everything is in memory now, the file can be closed
    fclose(file);
}


//
void keyboard(unsigned char key,int x,int y){
	switch(key){
		case 'z':mov_key_z =  mov_key_z+1.5;
			glutPostRedisplay() ;
			break;
		case 's':mov_key_z =  mov_key_z-1.5;
			glutPostRedisplay() ;
			break;
		case 'q':mov_key_x =  mov_key_x-1.5;
			glutPostRedisplay() ;
			break;
		case 'd':mov_key_x = mov_key_x+1.5;
			glutPostRedisplay() ;
			break;
		case 'p':mov_key_y = mov_key_y+1.5;
			glutPostRedisplay() ;
			break;
		case 'm': mov_key_z =  mov_key_z-1.5;
			glutPostRedisplay() ;
			break;
	}
}
void specialKeyboard(int key,int x, int y){
	switch(key){
		case GLUT_KEY_RIGHT:look_x = look_x+1;
			glutPostRedisplay() ;
			break;
		case GLUT_KEY_LEFT:look_x = look_x-1;
			glutPostRedisplay() ;
			break;
		case GLUT_KEY_UP:look_y =  look_y+1;
			glutPostRedisplay() ;
			break;
		case GLUT_KEY_DOWN:look_y = look_y-1;
			glutPostRedisplay() ;
			break;
	}
}
void initialScene(){
	glClearColor(0.22,0.25,0.34,1.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
    
}
void theCamera(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0,(double)400/400,0.1,1010.0);
	gluLookAt(mov_key_x,mov_key_y,mov_key_z,look_x,look_y,look_z,0.0,1.0,0.0);
	//gluLookAt( -10 ,5 , 50, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
}
void drawcube(int x_offset, int z_offset, int color)
{
	/* this function draws a cube centerd at (x_offset, z_offset)
	x and z _big are the back and rightmost points, x and z _small are
	the front and leftmost points */
	float x_big = (float)x_offset + 5;
	float z_big = (float)z_offset + 5;
	float x_small = (float)x_offset - 5;
	float z_small = (float)z_offset - 5;
	switch(color)
	{
	case 1:
		glColor3f(1.0,0.0,0.0);
		break;
	case 2:
		glColor3f(0.0,1.0,0.0);
		break;
	case 3:
		glColor3f(0.0,0.0,1.0);
		break;
	}
	glBegin(GL_QUADS);
		glVertex3f(x_small,10.0,z_big); /* front */
		glVertex3f(x_small,0.0,z_big);
		glVertex3f(x_big,0.0,z_big);
		glVertex3f(x_big,10.0,z_big);

		glVertex3f(x_big,10.0,z_small); /* back */
		glVertex3f(x_big,0.0,z_small);
		glVertex3f(x_small,0.0,z_small);
		glVertex3f(x_small,10.0,z_small);

		glVertex3f(x_big,10.0,z_big); /* right */
		glVertex3f(x_big,0.0,z_big);
		glVertex3f(x_big,0.0,z_small);
		glVertex3f(x_big,10.0,z_small);

		glVertex3f(x_small,10.0,z_small); /* left */
		glVertex3f(x_small,0.0,z_small);
		glVertex3f(x_small,0.0,z_big);
		glVertex3f(x_small,10.0,z_big);

		glVertex3f(x_small,10.0,z_big); /* top */
		glVertex3f(x_big,10.0,z_big);
		glVertex3f(x_big,10.0,z_small);
		glVertex3f(x_small,10.0,z_small);

		glVertex3f(x_small,0.0,z_small); /* bottom */
		glVertex3f(x_big,0.0,z_small);
		glVertex3f(x_big,0.0,z_big);
		glVertex3f(x_small,0.0,z_big);
	glEnd();
}
void DrawLeg(float xPos, float yPos, float zPos)
{
        glPushMatrix();
 
                /*      Sets color to yellow*/
                glColor3f(1.0f, 1.0f, 0.0f);
                glTranslatef(xPos, yPos, zPos);
 
                /*      Creates 1 x 5 x 1 cube*/
                glScalef(1.0f, 5.0f, 1.0f);
                drawcube(0.0, 0.0, 0.0);
 
        glPopMatrix();
}
void table(){
	glBegin(GL_QUADS);

	//Front
	//
	glColor3f(.6,.2,0);
	glVertex3f(-4.0f, -0.2f, 2.0f);
	glVertex3f(2.0f, -0.2f, 2.0f);
	glVertex3f(2.0f, 0.2f, 2.0f);
	glVertex3f(-4.0f, 0.2f, 2.0f);

	//Back
	//;
	glVertex3f(-4.0f, -0.2f, -2.0f);
	glVertex3f(-4.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, -0.2f, -2.0f);

	//Right
	//
	glVertex3f(2.0f, -0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, 2.0f);
	glVertex3f(2.0f, -0.2f, 2.0f);


	//Left
	;
	glVertex3f(-4.0f, -0.2f, -2.0f);
	glVertex3f(-4.0f, -0.2f, 2.0f);
	glVertex3f(-4.0f, 0.2f, 2.0f);
	glVertex3f(-4.0f, 0.2f, -2.0f);

	//top
	//

	glVertex3f(2.0f, 0.2f, 2.0f);
	glVertex3f(-4.0f, 0.2f, 2.0f);
	glVertex3f(-4.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);

	//bottom
	;

	glVertex3f(2.0f, -0.2f, 2.0f);
	glVertex3f(-4.0f, -0.2f, 2.0f);
	glVertex3f(-4.0f, -0.2f, -2.0f);
	glVertex3f(2.0f, -0.2f, -2.0f);

	//table front leg
	//front
	//
	glColor3f(.8f, .6f, 0.0f);
	glVertex3f(1.8f, -0.2f, 1.6f);
	glVertex3f(1.4f, -0.2f, 1.6f);
	glVertex3f(1.4f, -3.0f, 1.6f);
	glVertex3f(1.8f, -3.0f, 1.6f);

	//back
	//;

	glVertex3f(1.8f, -0.2f, 1.2f);
	glVertex3f(1.4f, -0.2f, 1.2f);
	glVertex3f(1.4f, -3.0f, 1.2f);
	glVertex3f(1.8f, -3.0f, 1.2f);

	//right
	//

	glVertex3f(1.8f, -0.2f, 1.6f);
	glVertex3f(1.8f, -0.2f, 1.2f);
	glVertex3f(1.8f, -3.0f, 1.2f);
	glVertex3f(1.8f, -3.0f, 1.6f);

	//left
	//;

	glVertex3f(1.4f, -0.2f, 1.6f);
	glVertex3f(1.4f, -0.2f, 1.2f);
	glVertex3f(1.4f, -3.0f, 1.2f);
	glVertex3f(1.4f, -3.0f, 1.6f);

	//back leg back
	//front
	//;
	glVertex3f(1.8f, -0.2f, -1.2f);
	glVertex3f(1.4f, -0.2f, -1.2f);
	glVertex3f(1.4f, -3.0f, -1.2f);
	glVertex3f(1.8f, -3.0f, -1.2f);

	//back
	//;

	glVertex3f(1.8f, -0.2f, -1.6f);
	glVertex3f(1.4f, -0.2f, -1.6f);
	glVertex3f(1.4f, -3.0f, -1.6f);
	glVertex3f(1.8f, -3.0f, -1.6f);

	//right
	//

	glVertex3f(1.8f, -0.2f, -1.6f);
	glVertex3f(1.8f, -0.2f, -1.2f);
	glVertex3f(1.8f, -3.0f, -1.2f);
	glVertex3f(1.8f, -3.0f, -1.6f);

	//left
	//

	glVertex3f(1.4f, -0.2f, -1.6f);
	glVertex3f(1.4f, -0.2f, -1.2f);
	glVertex3f(1.4f, -3.0f, -1.2f);
	glVertex3f(1.4f, -3.0f, -1.6f);

	//leg left front
	//

	
	glVertex3f(-3.8f, -0.2f, 1.6f);
	glVertex3f(-3.4f, -0.2f, 1.6f);
	glVertex3f(-3.4f, -3.0f, 1.6f);
	glVertex3f(-3.8f, -3.0f, 1.6f);

	//back
	//;

	glVertex3f(-3.8f, -0.2f, 1.2f);
	glVertex3f(-3.4f, -0.2f, 1.2f);
	glVertex3f(-3.4f, -3.0f, 1.2f);
	glVertex3f(-3.8f, -3.0f, 1.2f);

	//right


	glVertex3f(-3.8f, -0.2f, 1.6f);
	glVertex3f(-3.8f, -0.2f, 1.2f);
	glVertex3f(-3.8f, -3.0f, 1.2f);
	glVertex3f(-3.8f, -3.0f, 1.6f);

	//left
	;

	glVertex3f(-3.4f, -0.2f, 1.6f);
	glVertex3f(-3.4f, -0.2f, 1.2f);
	glVertex3f(-3.4f, -3.0f, 1.2f);
	glVertex3f(-3.4f, -3.0f, 1.6f);

	//left leg back front

	//front
	;
	//glColor3f(1,1,1);
	glVertex3f(-3.8f, -0.2f, -1.2f);
	glVertex3f(-3.4f, -0.2f, -1.2f);
	glVertex3f(-3.4f, -3.0f, -1.2f);
	glVertex3f(-3.8f, -3.0f, -1.2f);

	//back
	;

	glVertex3f(-3.8f, -0.2f, -1.6f);
	glVertex3f(-3.4f, -0.2f, -1.6f);
	glVertex3f(-3.4f, -3.0f, -1.6f);
	glVertex3f(-3.8f, -3.0f, -1.6f);

	//right


	glVertex3f(-3.8f, -0.2f, -1.6f);
	glVertex3f(-3.8f, -0.2f, -1.2f);
	glVertex3f(-3.8f, -3.0f, -1.2f);
	glVertex3f(-3.8f, -3.0f, -1.6f);

	//left


	glVertex3f(-3.4f, -0.2f, -1.6f);
	glVertex3f(-3.4f, -0.2f, -1.2f);
	glVertex3f(-3.4f, -3.0f, -1.2f);
	glVertex3f(-3.4f, -3.0f, -1.6f);



	glEnd();
}

void chair(){
	glColor3f(0.4f, 0.302f, 0.0f);
	glBegin(GL_QUADS);

	//Front
	//
	glVertex3f(-2.0f, -0.2f, 2.0f);
	glVertex3f(2.0f, -0.2f, 2.0f);
	glVertex3f(2.0f, 0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, 2.0f);

	//Right
	//
	glVertex3f(2.0f, -0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, 2.0f);
	glVertex3f(2.0f, -0.2f, 2.0f);

	//Back
	//;
	glVertex3f(-2.0f, -0.2f, -2.0f);
	glVertex3f(-2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, -0.2f, -2.0f);

	//Left
	;
	glVertex3f(-2.0f, -0.2f, -2.0f);
	glVertex3f(-2.0f, -0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, -2.0f);

	//top
	//

	glVertex3f(2.0f, 0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);

	//bottom
	;

	glVertex3f(2.0f, -0.2f, 2.0f);
	glVertex3f(-2.0f, -0.2f, 2.0f);
	glVertex3f(-2.0f, -0.2f, -2.0f);
	glVertex3f(2.0f, -0.2f, -2.0f);

	//table front leg
	//front
	//

	glVertex3f(1.8f, -0.2f, 1.6f);
	glVertex3f(1.4f, -0.2f, 1.6f);
	glVertex3f(1.4f, -3.0f, 1.6f);
	glVertex3f(1.8f, -3.0f, 1.6f);

	//back
	//;

	glVertex3f(1.8f, -0.2f, 1.2f);
	glVertex3f(1.4f, -0.2f, 1.2f);
	glVertex3f(1.4f, -3.0f, 1.2f);
	glVertex3f(1.8f, -3.0f, 1.2f);

	//right
	//

	glVertex3f(1.8f, -0.2f, 1.6f);
	glVertex3f(1.8f, -0.2f, 1.2f);
	glVertex3f(1.8f, -3.0f, 1.2f);
	glVertex3f(1.8f, -3.0f, 1.6f);

	//left
	//;

	glVertex3f(1.4f, -0.2f, 1.6f);
	glVertex3f(1.4f, -0.2f, 1.2f);
	glVertex3f(1.4f, -3.0f, 1.2f);
	glVertex3f(1.4f, -3.0f, 1.6f);

	//back leg back
	//front
	//;

	glVertex3f(1.8f, -0.2f, -1.2f);
	glVertex3f(1.4f, -0.2f, -1.2f);
	glVertex3f(1.4f, -3.0f, -1.2f);
	glVertex3f(1.8f, -3.0f, -1.2f);

	//back
	//;

	glVertex3f(1.8f, -0.2f, -1.6f);
	glVertex3f(1.4f, -0.2f, -1.6f);
	glVertex3f(1.4f, -3.0f, -1.6f);
	glVertex3f(1.8f, -3.0f, -1.6f);

	//right
	//

	glVertex3f(1.8f, -0.2f, -1.6f);
	glVertex3f(1.8f, -0.2f, -1.2f);
	glVertex3f(1.8f, -3.0f, -1.2f);
	glVertex3f(1.8f, -3.0f, -1.6f);

	//left
	//

	glVertex3f(1.4f, -0.2f, -1.6f);
	glVertex3f(1.4f, -0.2f, -1.2f);
	glVertex3f(1.4f, -3.0f, -1.2f);
	glVertex3f(1.4f, -3.0f, -1.6f);

	//leg left front
	//

	glVertex3f(-1.8f, -0.2f, 1.6f);
	glVertex3f(-1.4f, -0.2f, 1.6f);
	glVertex3f(-1.4f, -3.0f, 1.6f);
	glVertex3f(-1.8f, -3.0f, 1.6f);

	//back
	//;

	glVertex3f(-1.8f, -0.2f, 1.2f);
	glVertex3f(-1.4f, -0.2f, 1.2f);
	glVertex3f(-1.4f, -3.0f, 1.2f);
	glVertex3f(-1.8f, -3.0f, 1.2f);

	//right


	glVertex3f(-1.8f, -0.2f, 1.6f);
	glVertex3f(-1.8f, -0.2f, 1.2f);
	glVertex3f(-1.8f, -3.0f, 1.2f);
	glVertex3f(-1.8f, -3.0f, 1.6f);

	//left
	;

	glVertex3f(-1.4f, -0.2f, 1.6f);
	glVertex3f(-1.4f, -0.2f, 1.2f);
	glVertex3f(-1.4f, -3.0f, 1.2f);
	glVertex3f(-1.4f, -3.0f, 1.6f);

	//left leg back front

	//front
	;

	glVertex3f(-1.8f, -0.2f, -1.2f);
	glVertex3f(-1.4f, -0.2f, -1.2f);
	glVertex3f(-1.4f, -3.0f, -1.2f);
	glVertex3f(-1.8f, -3.0f, -1.2f);

	//back
	;

	glVertex3f(-1.8f, -0.2f, -1.6f);
	glVertex3f(-1.4f, -0.2f, -1.6f);
	glVertex3f(-1.4f, -3.0f, -1.6f);
	glVertex3f(-1.8f, -3.0f, -1.6f);

	//right


	glVertex3f(-1.8f, -0.2f, -1.6f);
	glVertex3f(-1.8f, -0.2f, -1.2f);
	glVertex3f(-1.8f, -3.0f, -1.2f);
	glVertex3f(-1.8f, -3.0f, -1.6f);

	//left


	glVertex3f(-1.4f, -0.2f, -1.6f);
	glVertex3f(-1.4f, -0.2f, -1.2f);
	glVertex3f(-1.4f, -3.0f, -1.2f);
	glVertex3f(-1.4f, -3.0f, -1.6f);

	//chair back
	//front





	//;
	//chair upper part 
	glColor3f(0.902, 0.902, 0);
	glVertex3f(-1.8f, 1.2f, -1.8f);
	glVertex3f(1.8f, 1.2f, -1.8f);
	glVertex3f(1.8f, 3.5f, -1.8f);
	glVertex3f(-1.8f, 3.5f, -1.8f);

	glVertex3f(-1.8f, 1.2f, -0.6f);
	glVertex3f(1.8f, 1.2f, -0.6f);
	glVertex3f(1.8f, 3.5f, -0.6f);
	glVertex3f(-1.8f, 3.5f, -0.6f);

	//chair upper side
	glColor3f(1, 1, 0.4);
	glVertex3f(-1.8f, 1.2f, -1.80f);
	glVertex3f(-1.8f, 1.2f, -0.6f);
	glVertex3f(-1.8f, 3.5f, -0.6f);
	glVertex3f(-1.8f, 3.5f, -1.8f);

	glVertex3f(1.8f, 1.2f, -1.80f);
	glVertex3f(1.8f, 1.2f, -0.6f);
	glVertex3f(1.8f, 3.5f, -0.6f);
	glVertex3f(1.8f, 3.5f, -1.8f);

	//chiar upper top 
	glColor3f(1, 1, 0);
	glVertex3f(-1.8f, 3.5f, -1.80f);
	glVertex3f(-1.8f, 3.5f, -0.6f);
	glVertex3f(1.8f, 3.5f, -0.6f);
	glVertex3f(1.8f, 3.5f, -1.8f);

	glVertex3f(-1.8f, 1.2f, -1.80f);
	glVertex3f(-1.8f, 1.2f, -0.6f);
	glVertex3f(1.8f, 1.2f, -0.6f);
	glVertex3f(1.8f, 1.2f, -1.8f);

	// chair top legs 

	glColor3f(0.2, 0.2, 0.0);
	//side walls
	glVertex3f(-1.2f, 1.2f, -1.20f);
	glVertex3f(-1.2f, 1.2f, -0.6f);
	glVertex3f(-1.2f, .2f, -0.6f);
	glVertex3f(-1.2f, .2f, -1.2f);

	glVertex3f(-.8f, 1.2f, -1.20f);
	glVertex3f(-.8f, 1.2f, -0.6f);
	glVertex3f(-.8f, .2f, -0.6f);
	glVertex3f(-.8f, .2f, -1.2f);

	//froont walls adnd back walls
	glVertex3f(-1.2f, 1.2f, -1.2f);
	glVertex3f(-0.8f, 1.2f, -1.2f);
	glVertex3f(-0.8f, .2f, -1.2f);
	glVertex3f(-1.2f, .2f, -1.2f);

	glVertex3f(-1.2f, 1.2f, -0.6f);
	glVertex3f(-0.8f, 1.2f, -0.6f);
	glVertex3f(-0.8f, .2f, -0.6f);
	glVertex3f(-1.2f, .2f, -0.6f);

	//side walls
	glVertex3f(1.2f, 1.2f, -1.20f);
	glVertex3f(1.2f, 1.2f, -0.6f);
	glVertex3f(1.2f, .2f, -0.6f);
	glVertex3f(1.2f, .2f, -1.2f);

	glVertex3f(.8f, 1.2f, -1.20f);
	glVertex3f(.8f, 1.2f, -0.6f);
	glVertex3f(.8f, .2f, -0.6f);
	glVertex3f(.8f, .2f, -1.2f);

	//froont walls adnd back walls
	glColor3f(0.2, 0.2, 0.0);
	glVertex3f(1.2f, 1.2f, -1.2f);
	glVertex3f(0.8f, 1.2f, -1.2f);
	glVertex3f(0.8f, .2f, -1.2f);
	glVertex3f(1.2f, .2f, -1.2f);

	glVertex3f(1.2f, 1.2f, -0.6f);
	glVertex3f(0.8f, 1.2f, -0.6f);
	glVertex3f(0.8f, .2f, -0.6f);
	glVertex3f(1.2f, .2f, -0.6f);

	//glVertex3f()

	glEnd();

}
void window(){
		glColor3f(0.6f, 0.7333f, 1.0f);
	glBegin(GL_QUADS);
	glVertex3f(-9.99f, 2.0f, 6.0f);
	glVertex3f(-9.99f, 5.0f, 6.0f);
	glVertex3f(-9.99f, 5.0f, 2.0f);
	glVertex3f(-9.99f, 2.0f, 2.0f);
	glEnd();

	glColor3f(0.149f, 0.149f, 0.149f);
	glLineWidth(10.0f);
	glBegin(GL_LINES);
	//left
	glVertex3f(-9.98f, 2.0f, 6.0f);
	glVertex3f(-9.98f, 5.0f, 6.0f);
	//top
	glVertex3f(-9.98f, 5.0f, 6.0f);
	glVertex3f(-9.98f, 5.0f, 2.0f);
	//right
	glVertex3f(-9.98f, 2.0f, 2.0f);
	glVertex3f(-9.98f, 5.0f, 2.0f);
	//bottom
	glVertex3f(-9.98f, 2.0f, 6.0f);
	glVertex3f(-9.98f, 2.0f, 2.0f);
	glEnd();

	//Cross
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex3f(-9.98f, 2.0f, 4.0f);
	glVertex3f(-9.98f, 5.0f, 4.0f);

	glVertex3f(-9.98f, 3.5f, 6.0f);
	glVertex3f(-9.98f, 3.5f, 2.0f);
	glEnd();
}

void windowSill(){
		glColor3f(0.149f, 0.149f, 0.149f);
	glBegin(GL_QUADS);
	glVertex3f(-9.99f, 2.0f, -6.0f);
	glVertex3f(-9.99f, 2.0f, -2.0f);
	glVertex3f(-9.5f, 2.0f, -2.0f);
	glVertex3f(-9.5f, 2.0f, -6.0f);

	glColor3f(0.2f, 0.2f, 0.2f);
	glVertex3f(-9.5f, 2.0f, -6.0f);
	glVertex3f(-9.5f, 2.0f, -2.0f);
	glVertex3f(-9.5f, 1.8f, -2.0f);
	glVertex3f(-9.5f, 1.8f, -6.0f);

	glColor3f(0.24f, 0.24f, 0.24f);
	glVertex3f(-9.99f, 2.0f, -6.0f);
	glVertex3f(-9.5f, 2.0f, -6.0f);
	glVertex3f(-9.5f, 1.8f, -6.0f);
	glVertex3f(-9.99f, 1.8f, -6.0f);

	glVertex3f(-9.99f, 2.0f, -2.0f);
	glVertex3f(-9.5f, 2.0f, -2.0f);
	glVertex3f(-9.5f, 1.8f, -2.0f);
	glVertex3f(-9.99f, 1.8f, -2.0f);

	glEnd();
}

void drawCupboard() {

	glColor3f(0.302f, 0.0f, 0.302f);
	glBegin(GL_QUADS);

    // Back
	glVertex3f(-1.5f, 0.5f, -1.5f);
	glVertex3f(-1.5f, 4.5f, -1.5f);
	glVertex3f(1.5, 4.5f, -1.5f);
	glVertex3f(1.5f, 0.5f, -1.5f);

    // Left Side
    glVertex3f(-1.5f, 0.5f, -1.5f);
	glVertex3f(-1.5f, 4.5f, -1.5f);
	glVertex3f(-1.5, 4.5f, 0.5f);
	glVertex3f(-1.5f, 0.5f, 0.5f);

    // Right Side
    glVertex3f(1.5f, 0.5f, -1.5f);
	glVertex3f(1.5f, 4.5f, -1.5f);
	glVertex3f(1.5, 4.5f, 0.5f);
	glVertex3f(1.5f, 0.5f, 0.5f);

    // Front
    glVertex3f(-1.5f, 0.5f, 0.5f);
	glVertex3f(-1.5f, 4.5f, 0.5f);
	glVertex3f(1.5, 4.5f, 0.5f);
	glVertex3f(1.5f, 0.5f, 0.5f);

    glColor3f(0.2f, 0.0f, 0.2f);
    // Left Support
	glVertex3f(-1.5f, 0.0f, -1.5f);
	glVertex3f(-1.5f, 0.5f, -1.5f);
	glVertex3f(-1.2, 0.5f, -1.5f);
	glVertex3f(-1.2f, 0.0f, -1.5f);

    glVertex3f(-1.5f, 0.0f, -1.5f);
	glVertex3f(-1.5f, 0.5f, -1.5f);
	glVertex3f(-1.5, 0.5f, 0.5f);
	glVertex3f(-1.5f, 0.0f, 0.5f);

    glVertex3f(-1.2f, 0.0f, -1.5f);
	glVertex3f(-1.2f, 0.5f, -1.5f);
	glVertex3f(-1.2, 0.5f, 0.5f);
	glVertex3f(-1.2f, 0.0f, 0.5f);

    glVertex3f(-1.5f, 0.0f, 0.5f);
	glVertex3f(-1.5f, 0.5f, 0.5f);
	glVertex3f(-1.2, 0.5f, 0.5f);
	glVertex3f(-1.2f, 0.0f, 0.5f);

    // Right Support
	glVertex3f(1.5f, 0.0f, -1.5f);
	glVertex3f(1.5f, 0.5f, -1.5f);
	glVertex3f(1.2f, 0.5f, -1.5f);
	glVertex3f(1.2f, 0.0f, -1.5f);

    glVertex3f(1.5f, 0.0f, -1.5f);
	glVertex3f(1.5f, 0.5f, -1.5f);
	glVertex3f(1.5f, 0.5f, 0.5f);
	glVertex3f(1.5f, 0.0f, 0.5f);

    glVertex3f(1.2f, 0.0f, -1.5f);
	glVertex3f(1.2f, 0.5f, -1.5f);
	glVertex3f(1.2f, 0.5f, 0.5f);
	glVertex3f(1.2f, 0.0f, 0.5f);

    glVertex3f(1.5f, 0.0f, 0.5f);
	glVertex3f(1.5f, 0.5f, 0.5f);
	glVertex3f(1.2f, 0.5f, 0.5f);
	glVertex3f(1.2f, 0.0f, 0.5f);

    glEnd();

    //lines
    glColor3f(0.149f, 0.149f, 0.149f);
    glLineWidth(5.0f);
    glBegin(GL_LINES);

    glVertex3f(-1.4f, 0.6f, 0.501f);
	glVertex3f(-1.4f, 4.4f, 0.501f);

	glVertex3f(-1.4f, 4.4f, 0.501f);
	glVertex3f(1.4f, 4.4f, 0.501f);

    glVertex3f(1.4f, 0.6f, 0.501f);
	glVertex3f(1.4f, 4.4f, 0.501f);

    glVertex3f(-1.4f, 0.6f, 0.501f);
	glVertex3f(1.4f, 0.6f, 0.501f);

    //middle line
    glVertex3f(0.0f, 0.6f, 0.501f);
	glVertex3f(0.0f, 4.4f, 0.501f);

    glEnd();

    // Handles
    glColor3f(0.5f, 0.5f, 0.5f);
    glPushMatrix();
    glTranslatef( 0.1f, 2.5f, 0.5f);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glutSolidTorus(0.03f, 0.1f, 100, 100);
    glPopMatrix();

    glPushMatrix();
    glTranslatef( -0.1f, 2.5f, 0.5f);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glutSolidTorus(0.03f, 0.1f, 100, 100);
    glPopMatrix();
}
float rotationAngle = 0.0f;

void drawFan(){
    glTranslatef(0, 5, -5.0);
    glScalef(5, 5, 5);
    //glRotatef(90, 1, 0, 0);
    //Cube
    glPushMatrix();
    glColor3f(1, 1, 1);
    glScalef(1, 1.2, 1);
    glScalef(.5, .4, .5);
    glTranslatef(0, -2.0, 0);
    glScalef(1.0f, 0.7f, 1.0f);
    glTranslatef(0, -0.5, 0);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

    glPushMatrix();
    glTranslatef(0, -1.05, 0);
    glScalef(0.15, .15, .15);
    glTranslatef(0.0f, -2.2f, 0.0f);
    glBegin(GL_QUAD_STRIP);
    for (int j = 0; j <= 360; j += DEF_D) {
        glColor3f(0.302, 0.149, 0.0);
        glVertex3f(cos(j), +1, sin(j));
        glColor3f(0.6, 0.302, 0.0);
        glVertex3f(cos(j), -1, sin(j));
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glColor3f(.3, .3, .3);
    glRotatef(90, 1, 0, 0);
    glTranslatef(0, 0, 1.55);
    glScalef(.25, .25, .25);
    glutSolidTorus(.1, .72, 25, 25);
    glPopMatrix();

    glPushMatrix();

    glPushMatrix();
    glColor3f(0.0, 0.0, 0.4);
    glTranslatef(-1, -1.55, 0);
    glScalef(2.5, .1, .75);
    glutSolidCube(.5);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -1.55, 0);
    glScalef(0.05, .11, .05);
    glRotatef(90, 1, 0, 0);
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, -5);
    glScalef(1, 1, 2.4);
    glBegin(GL_QUAD_STRIP);
    for (int j = 0; j <= 360; j += DEF_D) {
        glColor3f(.3, .3, .3);
        glVertex3f(cos(j), +1, sin(j));
        glColor3f(.7, .7, .7);
        glVertex3f(cos(j), -1, sin(j));
    }
    glEnd();
    glPopMatrix();

    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.4);
    glTranslatef(-1, -1.55, 0);
    glScalef(2.5, .1, .75);
    glutSolidCube(.5);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -1.55, 0);
    glScalef(0.05, .11, .05);
    glRotatef(90, 1, 0, 0);
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, -5);
    glScalef(1, 1, 2.4);
    glBegin(GL_QUAD_STRIP);
    for (int j = 0; j <= 360; j += DEF_D) {
        glColor3f(.3, .3, .3);
        glVertex3f(cos(j), +1, sin(j));
        glColor3f(.7, .7, .7);
        glVertex3f(cos(j), -1, sin(j));
    }
    glEnd();
    glPopMatrix();

    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 0, 1, 0);

    glPushMatrix();
    glColor3f(0.0, 0.0, 0.4);
    glTranslatef(-1, -1.55, 0);
    glScalef(2.5, .1, .75);
    glutSolidCube(.5);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -1.55, 0);
    glScalef(0.05, .11, .05);
    glRotatef(90, 1, 0, 0);
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, -5);
    glScalef(1, 1, 2.4);
    glBegin(GL_QUAD_STRIP);
    for (int j = 0; j <= 360; j += DEF_D) {
        glColor3f(.3, .3, .3);
        glVertex3f(cos(j), +1, sin(j));
        glColor3f(.7, .7, .7);
        glVertex3f(cos(j), -1, sin(j));
    }
    glEnd();
    glPopMatrix();

    glPopMatrix();

    glPushMatrix();
    glRotatef(270, 0, 1, 0);
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.4);
    glTranslatef(-1, -1.55, 0);
    glScalef(2.5, .1, .75);
    glutSolidCube(.5);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -1.55, 0);
    glScalef(0.05, .11, .05);
    glRotatef(90, 1, 0, 0);
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, -5);
    glScalef(1, 1, 2.4);
    glBegin(GL_QUAD_STRIP);
    for (int j = 0; j <= 360; j += DEF_D) {
        glColor3f(.3, .3, .3);
        glVertex3f(cos(j), +1, sin(j));
        glColor3f(.7, .7, .7);
        glVertex3f(cos(j), -1, sin(j));
    }
    glEnd();
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();
}

void theBase(){
	for(int i = -1000;i<1000;i++){
		
			glBegin(GL_LINES);
			glColor3f(rand(),random(),random());
			glVertex3f(1001,0,i);
			glVertex3f(-1001,0,i);
			glColor3f(random(),random(),random());
			glVertex3f(i,0,1001);
			glVertex3f(i,0,-1001);
			glEnd();
	}
	
}
void flooor(){

    glPushMatrix();

    glScalef(12,.3,14);
    glTranslatef(0,-11,0);
    drawcube(0.663, 0.663, 0.663);
    glPopMatrix();


}
void drawHeart(float size )
{	 glTranslatef(0, 0.0f, -0.2*(0.2-1/size));
		glBegin(GL_POLYGON);
        for (float x = -1.139; x <= 1.139; x += 0.001) 
        {
            float delta = pow((x*x), 1.0 / 3.0) * pow((x*x), 1.0 / 3.0)  - 4*x*x + 4;
            float y1 = (pow((x*x), 1.0 / 3.0)  + sqrt(delta)) / 2;
            float y2 = (pow((x*x), 1.0 / 3.0)  - sqrt(delta)) / 2;
            glVertex2f(x/size, y1/size);
            glVertex2f(x/size, y2/size);
        }
    glEnd();
}



void draw_map(int x)
{   
    if(x == 0){
        GLuint Texture = loadBMP_custom("field.bmp");
            glEnable(GL_TEXTURE_2D);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    glColor3f(1.0f,1.0f,1.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);    glVertex3f(-380.0f, -0.0f, 385.0f);
        glTexCoord2f(10.0f, 0.0f);    glVertex3f(395.0f, -0.0f, 385.0f);
        glTexCoord2f(10.0f, 10.0f);    glVertex3f( 395.0f, -0.0f, -385.0f);
        glTexCoord2f(0.0f, 10.0f);    glVertex3f( -380.0f, -0.0f, -385.0f);
        glEnd();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
    delete []data;
    glDeleteTextures(1, &textureID);
    glDisable(GL_TEXTURE_2D);
    }else{
        GLuint Texture = loadBMP_custom("./bmp/bg_space_seamless.bmp");
        glEnable(GL_TEXTURE_2D);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    glColor3f(0.2f,0.3f,0.9f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);    glVertex3f(-480.0f, -3.0f, 485.0f);
        glTexCoord2f(10.0f, 0.0f);    glVertex3f(495.0f, -3.0f, 485.0f);
        glTexCoord2f(10.0f, 10.0f);    glVertex3f( 495.0f, -3.0f, -485.0f);
        glTexCoord2f(0.0f, 10.0f);    glVertex3f( -480.0f, -3.0f, -485.0f);
        glEnd();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
    delete []data;
    glDeleteTextures(1, &textureID);
    glDisable(GL_TEXTURE_2D);
    }

}




void mech_court()		//badminton court
{
    int k;
    glPushMatrix();
        glTranslatef(-70,0.1,20);
            glColor3f(0.5,0.5,0.5);
            glBegin(GL_QUADS);
                glVertex3f(0,0,0);
                glVertex3f(20,0,0);
                glVertex3f(20,0,-40);
                glVertex3f(0,0,-40);
            glEnd();
            glColor3f(0,0,0);
            for(k=2;k<18;k+=4)
                {
                    glBegin(GL_QUADS);
                        glVertex3f(k,0,-0.1);
                        glVertex3f(k,4,-0.1);
                        glVertex3f(k+2,4,-0.1);
                        glVertex3f(k+2,0,-0.1);
                    glEnd();
                }
            for(k=-2;k>-38;k-=6)
                {
                    glBegin(GL_QUADS);
                        glVertex3f(0.1,0,k);
                        glVertex3f(0.1,4,k);
                        glVertex3f(0.1,4,k-2);
                        glVertex3f(0.1,0,k-2);
                    glEnd();
                }
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-50,0.1,20);
            for(k=-2;k>-38;k-=6)
                    {
                        glBegin(GL_QUADS);
                            glVertex3f(-0.1,0,k);
                            glVertex3f(-0.1,4,k);
                            glVertex3f(-0.1,4,k-2);
                            glVertex3f(-0.1,0,k-2);
                        glEnd();
                    }
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-50,0.1,-20);
            for(k=-2;k>-18;k-=4)
                    {    
                        glBegin(GL_QUADS);
                            glVertex3f(k,0,0.1);
                            glVertex3f(k,4,0.1);
                            glVertex3f(k-2,4,0.1);
                            glVertex3f(k-2,0,0.1);
                        glEnd();
                    }
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-65,0.1,15);
        glColor3f(0,0,1);
        glBegin(GL_QUADS);
            glVertex3f(0,0.2,0);
            glVertex3f(10,0.2,0);
            glVertex3f(10,0.2,-30);
            glVertex3f(0,0.2,-30);
        glEnd();
        glColor3f(1,1,1);
        glBegin(GL_LINE_LOOP);
            glVertex3f(1,0.3,-1);
            glVertex3f(9,0.3,-1);
            glVertex3f(9,0.3,-13);
            glVertex3f(1,0.3,-13);
        glEnd();
        glBegin(GL_LINE_LOOP);
            glVertex3f(1,0.3,-15);
            glVertex3f(9,0.3,-15);
            glVertex3f(9,0.3,-29);
            glVertex3f(1,0.3,-29);
        glEnd();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-65,0,8);
        glColor3f(1,1,1);
        for(float i=0;i<0.8;i+=0.2)
            for(float j=0;j<9.8;j+=0.2)
            {
                glBegin(GL_LINE_LOOP);
                    glVertex3f(j,i,0);
                    glVertex3f(j+0.2,i,0);
                    glVertex3f(j+0.2,i+0.2,0);
                    glVertex3f(j,i+0.2,0);
                glEnd();
            }
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-65,0,-8);
        glColor3f(1,1,1);
        for(float i=0;i<0.8;i+=0.2)
            for(float j=0;j<9.8;j+=0.2)
            {
                glBegin(GL_LINE_LOOP);
                    glVertex3f(j,i,0);
                    glVertex3f(j+0.2,i,0);
                    glVertex3f(j+0.2,i+0.2,0);
                    glVertex3f(j,i+0.2,0);
                glEnd();
            }
    glPopMatrix();
    for(int i=0;i<15;i+=5)
    {
        glPushMatrix();
            glTranslatef(-70,6+i,20);
                glColor3f(1,0.894,0.709);
                glBegin(GL_QUADS);
                    glVertex3f(0,0,0);
                    glVertex3f(20,0,0);
                    glVertex3f(20,0,-4);
                    glVertex3f(0,0,-4);
                glEnd();
                glColor3f(1,0.972,0.862);
                glBegin(GL_QUADS);
                    glVertex3f(4,0,-4);
                    glVertex3f(16,0,-4);
                    glVertex3f(16,2,-4);
                    glVertex3f(4,2,-4);
                glEnd();
                glColor3f(0,0,0);
                for(k=2;k<18;k+=4)
                {
                    glBegin(GL_QUADS);
                        glVertex3f(k,0,-0.1);
                        glVertex3f(k,4,-0.1);
                        glVertex3f(k+2,4,-0.1);
                        glVertex3f(k+2,0,-0.1);
                    glEnd();
                }
                glPushMatrix();
                    glTranslatef(20,0,0);
                        glColor3f(1,0.894,0.709);
                        glBegin(GL_QUADS);
                            glVertex3f(0,0,0);
                            glVertex3f(0,0,-40);
                            glVertex3f(-4,0,-40);
                            glVertex3f(-4,0,0);
                        glEnd();
                        glColor3f(1,0.972,0.862);
                        glBegin(GL_QUADS);
                            glVertex3f(-4,0,-4);
                            glVertex3f(-4,0,-36);
                            glVertex3f(-4,2,-36);
                            glVertex3f(-4,2,-4);
                        glEnd();
                        glColor3f(0,0,0);
                    for(k=-2;k>-38;k-=6)
                    {
                        glBegin(GL_QUADS);
                            glVertex3f(-0.1,0,k);
                            glVertex3f(-0.1,4,k);
                            glVertex3f(-0.1,4,k-2);
                            glVertex3f(-0.1,0,k-2);
                        glEnd();
                    }
                    glTranslatef(0,0,-40);
                    glColor3f(1,0.894,0.709);
                    glBegin(GL_QUADS);
                        glVertex3f(0,0,0);
                        glVertex3f(-20,0,0);
                        glVertex3f(-20,0,4);
                        glVertex3f(0,0,4);
                    glEnd();
                    glColor3f(1,0.972,0.862);
                    glBegin(GL_QUADS);
                        glVertex3f(-4,0,4);
                        glVertex3f(-16,0,4);
                        glVertex3f(-16,2,4);
                        glVertex3f(-4,2,4);
                    glEnd();
                    glColor3f(0,0,0);
                    for(k=-2;k>-18;k-=4)
                    {    
                        glBegin(GL_QUADS);
                            glVertex3f(k,0,0.1);
                            glVertex3f(k,4,0.1);
                            glVertex3f(k-2,4,0.1);
                            glVertex3f(k-2,0,0.1);
                        glEnd();
                    }
                glPopMatrix();
                glColor3f(1,0.894,0.709);
                glBegin(GL_QUADS);
                    glVertex3f(0,0,0);
                    glVertex3f(0,0,-40);
                    glVertex3f(4,0,-40);
                    glVertex3f(4,0,0);
                glEnd();
                glColor3f(1,0.972,0.862);
                glBegin(GL_QUADS);    
                    glVertex3f(4,0,-4);
                    glVertex3f(4,0,-36);
                    glVertex3f(4,2,-36);
                    glVertex3f(4,2,-4);
                glEnd();
                glColor3f(0,0,0);
                for(k=-2;k>-38;k-=6)
                {
                    glBegin(GL_QUADS);
                        glVertex3f(0.1,0,k);
                        glVertex3f(0.1,4,k);
                        glVertex3f(0.1,4,k-2);
                        glVertex3f(0.1,0,k-2);
                    glEnd();
                }
        glPopMatrix();
        }    
}

void disp_mba()		//mba block interiors
{int k;
    glPushMatrix();
        glTranslatef(-70,0.1,-30);
            glColor3f(0.5,0.5,0.5);
            glBegin(GL_QUADS);
                glVertex3f(0,0,0);
                glVertex3f(20,0,0);
                glVertex3f(20,0,-40);
                glVertex3f(0,0,-40);
            glEnd();
            glColor3f(0,0,0);
            for(k=2;k<18;k+=4)
                {
                    glBegin(GL_QUADS);
                        glVertex3f(k,0,-0.1);
                        glVertex3f(k,4,-0.1);
                        glVertex3f(k+2,4,-0.1);
                        glVertex3f(k+2,0,-0.1);
                    glEnd();
                }
            for(k=-2;k>-38;k-=6)
                {
                    glBegin(GL_QUADS);
                        glVertex3f(0.1,0,k);
                        glVertex3f(0.1,4,k);
                        glVertex3f(0.1,4,k-2);
                        glVertex3f(0.1,0,k-2);
                    glEnd();
                }
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-50,0.1,-30);
            for(k=-2;k>-38;k-=6)
                    {
                        glBegin(GL_QUADS);
                            glVertex3f(-0.1,0,k);
                            glVertex3f(-0.1,4,k);
                            glVertex3f(-0.1,4,k-2);
                            glVertex3f(-0.1,0,k-2);
                        glEnd();
                    }
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-50,0.1,-70);
            for(k=-2;k>-18;k-=4)
                    {    
                        glBegin(GL_QUADS);
                            glVertex3f(k,0,0.1);
                            glVertex3f(k,4,0.1);
                            glVertex3f(k-2,4,0.1);
                            glVertex3f(k-2,0,0.1);
                        glEnd();
                    }
    glPopMatrix();
    for(int i=0;i<15;i+=5)
    {
        glPushMatrix();
            glTranslatef(-70,6+i,-30);
                glColor3f(1,0.894,0.709);
                glBegin(GL_QUADS);
                    glVertex3f(0,0,0);
                    glVertex3f(20,0,0);
                    glVertex3f(20,0,-4);
                    glVertex3f(0,0,-4);
                glEnd();
                glColor3f(1,0.972,0.862);
                glBegin(GL_QUADS);
                    glVertex3f(4,0,-4);
                    glVertex3f(16,0,-4);
                    glVertex3f(16,2,-4);
                    glVertex3f(4,2,-4);
                glEnd();
                glColor3f(0,0,0);
                for(k=2;k<18;k+=4)
                {
                    glBegin(GL_QUADS);
                        glVertex3f(k,0,-0.1);
                        glVertex3f(k,4,-0.1);
                        glVertex3f(k+2,4,-0.1);
                        glVertex3f(k+2,0,-0.1);
                    glEnd();
                }
                glPushMatrix();
                    glTranslatef(20,0,0);
                        glColor3f(1,0.894,0.709);
                        glBegin(GL_QUADS);
                            glVertex3f(0,0,0);
                            glVertex3f(0,0,-40);
                            glVertex3f(-4,0,-40);
                            glVertex3f(-4,0,0);
                        glEnd();
                        glColor3f(1,0.972,0.862);
                        glBegin(GL_QUADS);
                            glVertex3f(-4,0,-4);
                            glVertex3f(-4,0,-36);
                            glVertex3f(-4,2,-36);
                            glVertex3f(-4,2,-4);
                        glEnd();
                        glColor3f(0,0,0);
                    for(k=-2;k>-38;k-=6)
                    {
                        glBegin(GL_QUADS);
                            glVertex3f(-0.1,0,k);
                            glVertex3f(-0.1,4,k);
                            glVertex3f(-0.1,4,k-2);
                            glVertex3f(-0.1,0,k-2);
                        glEnd();
                    }
                    glTranslatef(0,0,-40);
                    glColor3f(1,0.894,0.709);
                    glBegin(GL_QUADS);
                        glVertex3f(0,0,0);
                        glVertex3f(-20,0,0);
                        glVertex3f(-20,0,4);
                        glVertex3f(0,0,4);
                    glEnd();
                    glColor3f(1,0.972,0.862);
                    glBegin(GL_QUADS);
                        glVertex3f(-4,0,4);
                        glVertex3f(-16,0,4);
                        glVertex3f(-16,2,4);
                        glVertex3f(-4,2,4);
                    glEnd();
                    glColor3f(0,0,0);
                    for(k=-2;k>-18;k-=4)
                    {    
                        glBegin(GL_QUADS);
                            glVertex3f(k,0,0.1);
                            glVertex3f(k,4,0.1);
                            glVertex3f(k-2,4,0.1);
                            glVertex3f(k-2,0,0.1);
                        glEnd();
                    }
                glPopMatrix();
                glColor3f(1,0.894,0.709);
                glBegin(GL_QUADS);
                    glVertex3f(0,0,0);
                    glVertex3f(0,0,-40);
                    glVertex3f(4,0,-40);
                    glVertex3f(4,0,0);
                glEnd();
                glColor3f(1,0.972,0.862);
                glBegin(GL_QUADS);    
                    glVertex3f(4,0,-4);
                    glVertex3f(4,0,-36);
                    glVertex3f(4,2,-36);
                    glVertex3f(4,2,-4);
                glEnd();
                glColor3f(0,0,0);
                for(k=-2;k>-38;k-=6)
                {
                    glBegin(GL_QUADS);
                        glVertex3f(0.1,0,k);
                        glVertex3f(0.1,4,k);
                        glVertex3f(0.1,4,k-2);
                        glVertex3f(0.1,0,k-2);
                    glEnd();
                }
        glPopMatrix();
        }    
    
}

float stair[4][3];
float room[8][3];
float cil[6][3];
void centre()
{
        stair[0][0]=0;stair[0][1]=0;stair[0][2]=0;
        stair[1][0]=12;stair[1][1]=0;stair[1][2]=0;
        stair[2][0]=12;stair[2][1]=0;stair[2][2]=-12;
        stair[3][0]=0;stair[3][1]=0;stair[3][2]=-12;
        
        room[0][0]=0;room[0][1]=0;room[0][2]=0;
        room[1][0]=0;room[1][1]=6;room[1][2]=0;
        room[2][0]=0;room[2][1]=6;room[2][2]=-7;
        room[3][0]=0;room[3][1]=0;room[3][2]=-7;
        room[4][0]=7;room[4][1]=0;room[4][2]=-7;
        room[5][0]=7;room[5][1]=6;room[5][2]=-7;
        room[6][0]=7;room[6][1]=6;room[6][2]=0;
        room[7][0]=7;room[7][1]=0;room[7][2]=0;

        cil[0][0]=0;cil[0][1]=6;cil[0][2]=4;
        cil[1][0]=3.5;cil[1][1]=9;cil[1][2]=4;
        cil[2][0]=7;cil[2][1]=6;cil[2][2]=4;
        cil[3][0]=7;cil[3][1]=6;cil[3][2]=-9;
        cil[4][0]=3.5;cil[4][1]=9;cil[4][2]=-9;
        cil[5][0]=0;cil[5][1]=6;cil[5][2]=-9;
}
void disp_stair(int x, int y, int z)
{
        glColor3f(1,0.960,0.933);
        glBegin(GL_QUADS);
            glVertex3f(stair[0][0]-x,stair[0][1]-y,stair[0][2]+z);
            glVertex3f(stair[1][0]+x,stair[1][1]-y,stair[1][2]+z);
            glVertex3f(stair[2][0]+x,stair[2][1]-y,stair[2][2]-z);
            glVertex3f(stair[3][0]-x,stair[3][1]-y,stair[3][2]-z);
        glEnd();
        glColor3f(0.933,0.913,0.8);
        glBegin(GL_QUADS);
            glVertex3f(stair[0][0]-x,stair[0][1]-y,stair[0][2]+z);
            glVertex3f(stair[0][0]-x,stair[0][1]-1-y,stair[0][2]+z);
            glVertex3f(stair[1][0]+x,stair[1][1]-1-y,stair[1][2]+z);
            glVertex3f(stair[1][0]+x,stair[1][1]-y,stair[1][2]+z);
            
            glVertex3f(stair[1][0]+x,stair[1][1]-y,stair[1][2]+z);
            glVertex3f(stair[1][0]+x,stair[1][1]-1-y,stair[1][2]+z);
            glVertex3f(stair[2][0]+x,stair[2][1]-1-y,stair[2][2]-z);
            glVertex3f(stair[2][0]+x,stair[2][1]-y,stair[2][2]-z);
            
            glVertex3f(stair[2][0]+x,stair[2][1]-y,stair[2][2]-z);
            glVertex3f(stair[3][0]-x,stair[3][1]-y,stair[3][2]-z);
            glVertex3f(stair[3][0]-x,stair[3][1]-1-y,stair[3][2]-z);
            glVertex3f(stair[2][0]+x,stair[2][1]-1-y,stair[2][2]-z);
            
            glVertex3f(stair[3][0]-x,stair[3][1]-y,stair[3][2]-z);
            glVertex3f(stair[0][0]-x,stair[0][1]-y,stair[0][2]+z);
            glVertex3f(stair[0][0]-x,stair[0][1]-1-y,stair[0][2]+z);
            glVertex3f(stair[3][0]-x,stair[3][1]-1-y,stair[3][2]-z);
        glEnd();
}

void disp_room()
    {
        glColor3f(0.803,0.803,0.756);
        glBegin(GL_QUADS);
            glVertex3fv(room[0]);
            glVertex3fv(room[1]);
            glVertex3fv(room[2]);
            glVertex3fv(room[3]);
            glVertex3fv(room[3]);
            glVertex3fv(room[2]);
            glVertex3fv(room[5]);
            glVertex3fv(room[4]);
            glVertex3fv(room[4]);
            glVertex3fv(room[5]);
            glVertex3fv(room[6]);
            glVertex3fv(room[7]);
            glVertex3fv(room[1]);
            glVertex3fv(room[2]);
            glVertex3fv(room[5]);
            glVertex3fv(room[6]);
            glVertex3fv(room[0]);
            glVertex3f(room[0][0]+1,room[0][1],room[0][2]);
            glVertex3f(room[1][0]+1,room[1][1],room[1][2]);
            glVertex3fv(room[1]);
            glVertex3fv(room[7]);
            glVertex3f(room[7][0]-1,room[7][1],room[7][2]);
            glVertex3f(room[6][0]-1,room[6][1],room[6][2]);
            glVertex3fv(room[6]);
        glEnd();
    }
void disp_cil()
    {
        glBegin(GL_TRIANGLES);
        glColor3f(1,0.843,0);
            glVertex3fv(cil[2]);
            glVertex3fv(cil[1]);
            glVertex3fv(cil[0]);
            glVertex3fv(cil[3]);
            glVertex3fv(cil[4]);
            glVertex3fv(cil[5]);
        glEnd();
        glBegin(GL_POLYGON);
        glColor3f(0.933,0.866,0.509);
            glVertex3fv(cil[2]);
            glVertex3fv(cil[1]);
            glVertex3fv(cil[4]);
            glVertex3fv(cil[3]);
        glEnd();
        glBegin(GL_POLYGON);
            glVertex3fv(cil[0]);
            glVertex3fv(cil[1]);
            glVertex3fv(cil[4]);
            glVertex3fv(cil[5]);
        glEnd();
    }
void draw_pil()
    {
        GLUquadricObj *quadratic;
        quadratic = gluNewQuadric();
        glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glColor3f(0.933,0.866,0.509);
        gluCylinder(quadratic,0.5,0.5,6.0f,32,32);
        glPopMatrix();
    }
void draw_mesh()
    {
        glColor3f(1,0.843,0);
        for(float i=0;i<0.9;i+=0.2)
            for(float j=0;j<6;j+=0.2)
            {
                glBegin(GL_LINE_LOOP);
                    glVertex3f(i,j,0);
                    glVertex3f(i+0.2,j,0);
                    glVertex3f(i+0.2,j+0.2,0);
                    glVertex3f(i,j+0.2,0);
                glEnd();
            }
    }
void disp_centre()
    {
        disp_stair(0,0,0);
        glPushMatrix();
            disp_stair(2,1,2);
            disp_stair(4,2,4);
        
        glPopMatrix();
        glPushMatrix();
        glTranslatef(4,0,-9);
        glRotatef(-90,0,1,0);
            disp_room();
            disp_cil();
            glPushMatrix();
            glTranslatef(0.4,0,2.5);
                draw_pil();
            glTranslatef(6.2,0,0);
                draw_pil();
            glPopMatrix();
            glPushMatrix();
            glTranslatef(1,0,0);
                draw_mesh();
            glTranslatef(4,0,0);
                draw_mesh();
            glPopMatrix();
        glPopMatrix();

    }

// building 

float structure[8][3];

void building(float a, float b, float c)
    {
        structure[0][0]=0;structure[0][1]=0;structure[0][2]=0;
        structure[1][0]=a;structure[1][1]=0;structure[1][2]=0;
        structure[2][0]=a;structure[2][1]=b;structure[2][2]=0;
        structure[3][0]=0;structure[3][1]=b;structure[3][2]=0;
        structure[4][0]=0;structure[4][1]=0;structure[4][2]=c;
        structure[5][0]=a;structure[5][1]=0;structure[5][2]=c;
        structure[6][0]=a;structure[6][1]=b;structure[6][2]=c;
        structure[7][0]=0;structure[7][1]=b;structure[7][2]=c;
    }
void disp_build(char text[15],char side='/0')
    {
        float door[3];
        glColor3f(1,0.980,0.980);
        glBegin(GL_QUADS);
            glVertex3fv(structure[0]);
            glVertex3fv(structure[1]);
            glVertex3fv(structure[2]);
            glVertex3fv(structure[3]);
        glEnd();
        glBegin(GL_QUADS);
                glVertex3fv(structure[0]);
                glVertex3fv(structure[4]);
                glVertex3fv(structure[7]);
                glVertex3fv(structure[3]);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3fv(structure[4]);
            glVertex3fv(structure[5]);
            glVertex3fv(structure[6]);
            glVertex3fv(structure[7]);
        glEnd();
        glBegin(GL_QUADS);
                glVertex3fv(structure[1]);
                glVertex3fv(structure[2]);
                glVertex3fv(structure[6]);
                glVertex3fv(structure[5]);
        glEnd();
        
        if(structure[1][0]>(-1*structure[4][2]))
        {
            for(float i=10; i<structure[2][1]; i+=10)
            {
                glPushMatrix();
                glTranslatef(0,i,0);
                for(float j=5; j<structure[1][0]; j+=15)
                {
                    glColor3f(0,0,0);
                    glBegin(GL_POLYGON);
                        glVertex3f(j,0,0.1);
                        glVertex3f(j+5,0,0.1);
                        glVertex3f(j+5,5,0.1);
                        glVertex3f(j,5,0.1);
                    glEnd();
                    glBegin(GL_POLYGON);
                        glVertex3f(j,0,structure[4][2]-0.1);
                        glVertex3f(j+5,0,structure[4][2]-0.1);
                        glVertex3f(j+5,5,structure[4][2]-0.1);
                        glVertex3f(j,5,structure[4][2]-0.1);
                    glEnd();
                }
                for(float j=0;j<structure[1][0];j+=15)
                {
                    glColor3f(1,0,0);
                    glBegin(GL_POLYGON);
                        glVertex3f(j,-10,0.1);
                        glVertex3f(j+2,-10,0.1);
                        glVertex3f(j+2,10,0.1);
                        glVertex3f(j,10,0.1);
                    glEnd();
                    glBegin(GL_POLYGON);
                        glVertex3f(j,-10,structure[4][2]-0.1);
                        glVertex3f(j+2,-10,structure[4][2]-0.1);
                        glVertex3f(j+2,10,structure[4][2]-0.1);
                        glVertex3f(j,10,structure[4][2]-0.1);
                    glEnd();
                }
                glPopMatrix();
            }    
            glColor3f(0,0,0);
            door[0]=(structure[1][0]/2);
            glBegin(GL_POLYGON);
                glVertex3f(door[0]-4,0,0.2);
                glVertex3f(door[0]+4,0,0.2);
                glVertex3f(door[0]+4,7,0.2);
                glVertex3f(door[0]-4,7,0.2);
            glEnd();
            glPushMatrix();
            glTranslatef(10,0,3);
            draw_board();
            glPushMatrix();
            glTranslatef(1,2,0.1);
            glScalef(.01, .01, .01);
            glLineWidth(2);
            glColor3f(0,0,0);
            for (int c=0; text[c] != 0; ++c)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, text[c]);
            glPopMatrix();
            glPopMatrix();
        }
        else
        {
            for(float i=10; i<structure[2][1]; i+=10)
            {
                glPushMatrix();
                glTranslatef(0,i,0);
                for(float j=-5; j>structure[4][2]; j-=15)
                {
                    glColor3f(0,0,0);
                    glBegin(GL_POLYGON);
                        glVertex3f(-0.1,0,j);
                        glVertex3f(-0.1,0,j-5);
                        glVertex3f(-0.1,5,j-5);
                        glVertex3f(-0.1,5,j);
                    glEnd();
                    glBegin(GL_POLYGON);
                        glVertex3f(structure[1][0]+0.1,0,j);
                        glVertex3f(structure[1][0]+0.1,0,j-5);
                        glVertex3f(structure[1][0]+0.1,5,j-5);
                        glVertex3f(structure[1][0]+0.1,5,j);
                    glEnd();
                }
                for(float j=0;j>structure[4][2];j-=15)
                {
                    glColor3f(1,0,0);
                    glBegin(GL_POLYGON);
                        glVertex3f(-0.1,-10,j);
                        glVertex3f(-0.1,-10,j-2);
                        glVertex3f(-0.1,10,j-2);
                        glVertex3f(-0.1,10,j);
                    glEnd();
                    glBegin(GL_POLYGON);
                        glVertex3f(structure[1][0]+0.1,-10,j);
                        glVertex3f(structure[1][0]+0.1,-10,j-2);
                        glVertex3f(structure[1][0]+0.1,10,j-2);
                        glVertex3f(structure[1][0]+0.1,10,j);
                    glEnd();
                }
                glPopMatrix();
            }
            door[2]=(structure[4][2]/2);
            door[0]=structure[1][0];
            glColor3f(0,0,0);
            if(side=='r')
            {
                glBegin(GL_POLYGON);
                    glVertex3f(door[0]+0.2,0,door[2]-4);
                    glVertex3f(door[0]+0.2,0,door[2]+4);
                    glVertex3f(door[0]+0.2,7,door[2]+4);
                    glVertex3f(door[0]+0.2,7,door[2]-4);
                glEnd();
                glPushMatrix();
                glTranslatef(door[0]+3,0,-2);
                glRotatef(90,0,1,0);
                draw_board();
                glPushMatrix();
                glTranslatef(1,2,0.1);
                glScalef(.01, .01, .01);
                glLineWidth(2);
                glColor3f(0,0,0);
                for (int c=0; text[c] != 0; ++c)
                    glutStrokeCharacter(GLUT_STROKE_ROMAN, text[c]);
                glPopMatrix();
                glPopMatrix();
            }
            else if(side=='l')
            {
                glBegin(GL_POLYGON);
                    glVertex3f(-0.2,0,door[2]-4);
                    glVertex3f(-0.2,0,door[2]+4);
                    glVertex3f(-0.2,7,door[2]+4);
                    glVertex3f(-0.2,7,door[2]-4);
                glEnd();
                glPushMatrix();
                glTranslatef(-3,0,-10);
                glRotatef(-90,0,1,0);
                draw_board();
                glPushMatrix();
                glTranslatef(1,2,0.1);
                glScalef(.01, .01, .01);
                glLineWidth(2);
                glColor3f(0,0,0);
                for (int c=0; text[c] != 0; ++c)
                    glutStrokeCharacter(GLUT_STROKE_ROMAN, text[c]);
                glPopMatrix();
                glPopMatrix();
            }
        }
        glPushMatrix();
        glTranslatef(0,10,0);
        glColor3f(0,0,1);
        for(int i=0;i<structure[2][1]-5;i+=5)
        {
            glBegin(GL_LINES);
                glVertex3f(structure[0][0],i,structure[0][2]+0.1);
                glVertex3f(structure[1][0],i,structure[0][2]+0.1);
                glVertex3f(structure[0][0]-0.1,i,structure[0][2]);
                glVertex3f(structure[0][0]-0.1,i,structure[4][2]);
                glVertex3f(structure[4][0],i,structure[4][2]-0.1);
                glVertex3f(structure[5][0],i,structure[4][2]-0.1);
                glVertex3f(structure[5][0]+0.1,i,structure[5][2]);
                glVertex3f(structure[1][0]+0.1,i,structure[1][2]);

            glEnd();
        }
        glPopMatrix();
    }

void loop(float x,float y,float z)		//the basket loop of the basketball post
{
    float xx,zz,d;
    glColor3f(1,0,0);
    glPointSize(2);
    glBegin(GL_POINTS);
        for(int i=0;i<360;i++)
    {
        d=i*(180/3.14);
        xx=cos(d)+x;
        zz=sin(d)+z;
        glVertex3f(xx,y,zz);
    }
    glEnd();
}

float bordr[4][3];
float bskt[8][3];
void bball()
    {
        bordr[0][0]=0;bordr[0][1]=0;bordr[0][2]=0;
        bordr[1][0]=20;bordr[1][1]=0;bordr[1][2]=0;
        bordr[2][0]=20;bordr[2][1]=0;bordr[2][2]=-20;
        bordr[3][0]=0;bordr[3][1]=0;bordr[3][2]=-20;
        bskt[0][0]=14;bskt[0][1]=4.5;bskt[0][2]=-0.1;
        bskt[1][0]=16;bskt[1][1]=4.5;bskt[1][2]=-0.1;
        bskt[2][0]=16;bskt[2][1]=6.5;bskt[2][2]=-0.1;
        bskt[3][0]=14;bskt[3][1]=6.5;bskt[3][2]=-0.1;
        bskt[4][0]=14,bskt[4][1]=4.5;bskt[4][2]=-19.9;
        bskt[5][0]=16;bskt[5][1]=4.5;bskt[5][2]=-19.9;
        bskt[6][0]=16;bskt[6][1]=6.5;bskt[6][2]=-19.9;
        bskt[7][0]=14;bskt[7][1]=6.5;bskt[7][2]=-19.9;
    }
void disp_court()
    {
        glPushMatrix();
        glTranslatef(0,0.1,0);
        glColor3f(0.745,0.745,0.745);
        glBegin(GL_QUADS);
            glVertex3fv(bordr[0]);
            glVertex3fv(bordr[1]);
            glVertex3fv(bordr[2]);
            glVertex3fv(bordr[3]);
        glEnd();
        glColor3f(1,0.270,0);
        GLUquadricObj *quadratic;
        quadratic = gluNewQuadric();
        GLUquadricObj *quadratic1;
        quadratic1 = gluNewQuadric();
        glPushMatrix();
        glTranslatef(15,0,0);
            loop(0,5,-1);
        glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glColor3f(0.698,0.133,0.133);
        gluCylinder(quadratic,0.1,0.1,5.0f,32,32);
        glPopMatrix();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(15,0,-20);
            loop(0,5,1);
        glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glColor3f(0.698,0.133,0.133);
        gluCylinder(quadratic1,0.1,0.1,5.0f,32,32);
        glPopMatrix();
        glPopMatrix();
        glColor3f(0.745,0.745,0.745);
        glBegin(GL_QUADS);
            for(int i=0;i<8;i++)
                glVertex3fv(bskt[i]);
        glEnd();
        glPopMatrix();
    }
void ground()
    {
        bordr[0][0]=0;bordr[0][1]=-2;bordr[0][2]=0;
        bordr[1][0]=40;bordr[1][1]=-2;bordr[1][2]=0;
        bordr[2][0]=40;bordr[2][1]=-2;bordr[2][2]=-40;
        bordr[3][0]=0;bordr[3][1]=-2;bordr[3][2]=-40;
    }
void ground_disp()
    {
        float t=6;
        for(int i=0;i<3;i++,t-=2)
        {
            glPushMatrix();
            glTranslatef(0,i,0);
            glColor3f(0.803,0.701,0.545);
            glBegin(GL_POLYGON);
                glVertex3f(bordr[0][0],bordr[0][1],bordr[0][2]);
                glVertex3f(bordr[1][0],bordr[1][1],bordr[1][2]);
                glVertex3f(bordr[1][0],bordr[1][1],bordr[1][2]-t);
                glVertex3f(bordr[0][0],bordr[0][1],bordr[0][2]-t);
            glEnd();
            glColor3f(0.545,0.474,0.368);
            glBegin(GL_POLYGON);
                glVertex3f(bordr[0][0],bordr[0][1],bordr[0][2]-t);
                glVertex3f(bordr[1][0],bordr[1][1],bordr[1][2]-t);
                glVertex3f(bordr[1][0],bordr[1][1]-1,bordr[1][2]-t);
                glVertex3f(bordr[0][0],bordr[0][1]-1,bordr[0][2]-t);
            glEnd();
            glColor3f(0.803,0.701,0.545);
            glBegin(GL_POLYGON);
                glVertex3f(bordr[1][0],bordr[1][1],bordr[1][2]);
                glVertex3f(bordr[2][0],bordr[2][1],bordr[2][2]);
                glVertex3f(bordr[2][0]-t,bordr[2][1],bordr[2][2]);
                glVertex3f(bordr[1][0]-t,bordr[1][1],bordr[1][2]);
            glEnd();
            glColor3f(0.545,0.474,0.368);
            glBegin(GL_POLYGON);
                glVertex3f(bordr[1][0]-t,bordr[1][1],bordr[1][2]);
                glVertex3f(bordr[2][0]-t,bordr[2][1],bordr[2][2]);
                glVertex3f(bordr[2][0]-t,bordr[2][1]-1,bordr[2][2]);
                glVertex3f(bordr[1][0]-t,bordr[1][1]-1,bordr[1][2]);
            glEnd();
            glColor3f(0.803,0.701,0.545);
            glBegin(GL_POLYGON);
                glVertex3f(bordr[2][0],bordr[2][1],bordr[2][2]);
                glVertex3f(bordr[3][0],bordr[3][1],bordr[3][2]);
                glVertex3f(bordr[3][0],bordr[3][1],bordr[3][2]+t);
                glVertex3f(bordr[2][0],bordr[2][1],bordr[2][2]+t);
            glEnd();
            glColor3f(0.545,0.474,0.368);
            glBegin(GL_POLYGON);
                glVertex3f(bordr[2][0],bordr[2][1],bordr[2][2]+t);
                glVertex3f(bordr[3][0],bordr[3][1],bordr[3][2]+t);
                glVertex3f(bordr[3][0],bordr[3][1]-1,bordr[3][2]+t);
                glVertex3f(bordr[2][0],bordr[2][1]-1,bordr[2][2]+t);
            glEnd();
            glColor3f(0.803,0.701,0.545);
            glBegin(GL_POLYGON);
                glVertex3f(bordr[3][0],bordr[3][1],bordr[3][2]);
                glVertex3f(bordr[0][0],bordr[0][1],bordr[0][2]);
                glVertex3f(bordr[0][0]+t,bordr[0][1],bordr[0][2]);
                glVertex3f(bordr[3][0]+t,bordr[3][1],bordr[3][2]);
            glEnd();
            glColor3f(0.545,0.474,0.368);
            glBegin(GL_POLYGON);
            glVertex3f(bordr[0][0]+t,bordr[0][1],bordr[0][2]);
                glVertex3f(bordr[3][0]+t,bordr[3][1],bordr[3][2]);
                glVertex3f(bordr[3][0]+t,bordr[3][1]-1,bordr[3][2]);
                glVertex3f(bordr[0][0]+t,bordr[0][1]-1,bordr[0][2]);
            glEnd();
            glPopMatrix();
        }
        glPushMatrix();
        glTranslatef(16.5,-3,-7);
        glColor3f(0.827,0.827,0.827);
        glLineWidth(10);
        glBegin(GL_LINE_LOOP);
            glVertex3f(0,0,0);
            glVertex3f(0,2,0);
            glVertex3f(4,2,0);
            glVertex3f(4,0,0);
        glEnd();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(16.5,-3,-33);
        glColor3f(0.827,0.827,0.827);
        glLineWidth(10);
        glBegin(GL_LINE_LOOP);
            glVertex3f(0,0,0);
            glVertex3f(0,2,0);
            glVertex3f(4,2,0);
            glVertex3f(4,0,0);
        glEnd();
        glPopMatrix();
    }

// roads 
void disp_roads()			//display the roads in the campus
{
    glColor3f(0.411,0.411,0.411);
    glBegin(GL_QUADS);
        glVertex3f(-40,0.1,90);
        glVertex3f(-40,0.1,-70);
        glVertex3f(-20,0.1,-70);
        glVertex3f(-20,0.1,90);
    glEnd();
    glBegin(GL_QUADS);
        glVertex3f(-20,0.1,55);
        glVertex3f(90,0.1,55);
        glVertex3f(90,0.1,60);
        glVertex3f(-20,0.1,60);
    glEnd();
    glBegin(GL_QUADS);
        glVertex3f(-20,0.1,75);
        glVertex3f(40,0.1,75);
        glVertex3f(40,0.1,80);
        glVertex3f(-20,0.1,80);
    glEnd();
    glBegin(GL_QUADS);
        glVertex3f(35,0.1,75);
        glVertex3f(35,0.1,-70);
        glVertex3f(40,0.1,-70);
        glVertex3f(40,0.1,75);
    glEnd();
}

//trees 
void trees()			//draw a tree
{   glScalef(2,2,2);
    GLUquadricObj *quadratic;
    GLUquadricObj *quadratic1;
        quadratic1 = gluNewQuadric();
    quadratic = gluNewQuadric();
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glColor3f(0.721,0.525,0.043);
    gluCylinder(quadratic,1,1,10.0f,32,32);
    glPopMatrix();
    glTranslatef(0,2,0);
        glPushMatrix();
    float k=0;
    for(int i=0,j=0;i<3;i++,j+=0.5,k+=0.15)
    {
        glTranslatef(0,1.8,0);
        glColor3f(0.133+k,0.545+k,0.133-k);
        glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(quadratic1,4-j,0,4.0f,32,32);
        glPopMatrix();
    }
    glPopMatrix();
}

// archs
void draw_arch(char text[5])		//draw the arch
{
    glColor3f(0,0,1);
    glPushMatrix();
    glTranslatef(0,3.5,0);
    glScalef(4,7,2);
    glutSolidCube(1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(16,3.5,0);
    glScalef(4,7,2);
    glutSolidCube(1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(8,9,0);
    glScalef(20,4,2);
    glutSolidCube(1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(2,8,1.1);
    glScalef(.02, .02, .02);
    glLineWidth(4.5);
    glColor3f(1,1,1);
    for (int c=0; text[c] != 0; ++c)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, text[c]);
    glPopMatrix();
}

void drawLineTrees(int p){
    glLoadIdentity();
        glTranslatef(p*75,0,0);
        trees();
    
        glLoadIdentity();
        glTranslatef(p*95,0,0);
        trees();
        
        glLoadIdentity();
        glTranslatef(p*115,0,0);
        trees();
        
        glLoadIdentity();
        glTranslatef(p*135,0,0);
        trees();
        
        glLoadIdentity();
        glTranslatef(p*155,0,0);
        trees();
        
        glLoadIdentity();
        glTranslatef(p*175,0,0);
        trees();
        
        glLoadIdentity();
        glTranslatef(p*195,0,0);
        trees();
        glLoadIdentity();
        glTranslatef(p*215,0,0);
        trees();
        glLoadIdentity();
        glTranslatef(p*235,0,0);
        trees();
        glLoadIdentity();
        glTranslatef(p*255,0,0);
        trees();
        glLoadIdentity();
        glTranslatef(p*275,0,0);
        trees();
        glLoadIdentity();
        glTranslatef(p*295,0,0);
        trees();
        glLoadIdentity();
        glTranslatef(p*315,0,0);
        trees();
        glLoadIdentity();
        glTranslatef(p*335,0,0);
        trees();
}


void MyGame(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	theCamera();
    glMatrixMode(GL_MODELVIEW);

	    glLoadIdentity();
        draw_map(0);
        glTranslatef(0,0,-380);
        glRotatef(90,1,0,0);
        draw_map(1);
        glLoadIdentity();
        glTranslatef(0,380,0);
        draw_map(1);
        glLoadIdentity();
        glTranslatef(0,0,380);
        glRotatef(90,1,0,0);
        draw_map(1);
        glLoadIdentity();
        glTranslatef(380,0,0);
        glRotatef(90,0,0,1);
        draw_map(1);
        glLoadIdentity();
        glTranslatef(-380,0,0);
        glRotatef(90,0,0,1);
        draw_map(1);

        glLoadIdentity();
        glScalef(5.5,5.5,5.5);
        glTranslatef(-8,0,0);
        draw_arch("welcome");  
        glScalef(2,2,2);      
        drawLineTrees(-1);
        drawLineTrees(1);
        glLoadIdentity();
        building(5,5,5);
        disp_build("oussama",'r');
        glLoadIdentity();
        glTranslatef(-100,0,-100);
        building(51,51,51);
        disp_build("oussama",'l');
        glLoadIdentity();
        //draw_map();
        /*glBegin(GL_QUADS);
        glVertex3f(-100,0,100);
        glVertex3f(100,0,100);
        glVertex3f(100,0,20);
        glVertex3f(-100,0,20);
        glVertex3f(-100,0,20);
        glVertex3f(-15,0,20);
        glVertex3f(-15,0,-100);
        glVertex3f(-100,0,-100);
        glVertex3f(-15,0,-100);
        glVertex3f(100,0,-100);
        glVertex3f(100,0,-20);
        glVertex3f(-15,0,-20);
        glVertex3f(25,0,-20);
        glVertex3f(100,0,-20);
        glVertex3f(100,0,20);
        glVertex3f(25,0,20);
        glEnd();        
        */

        glBegin(GL_LINES);
		glColor3f(0,0,0);
		glVertex3f(1001,0,0);
		glVertex3f(-1001,0,0);

		glVertex3f(0,0,1110);
		glVertex3f(0,0,-1110);

		glVertex3f(0,1001,0);
		glVertex3f(0,-1001,0);
	glEnd();

	//the end 
	glutSwapBuffers();
}

int main(int argc , char** argv){
	glutInit(&argc,argv);
	glutInitWindowSize(1360,7680);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("the aowsome park");

	initialScene();
	glutDisplayFunc(MyGame);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutMainLoop();
	return 0;
}