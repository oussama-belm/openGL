#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "BmpLoader.h"
#include <windows.h>
#include <stdlib.h>
#include <vector>
#include <time.h>

std::vector<unsigned int>TextureID;

void loadTexture ( const char * filename ) {
    BmpLoader bl ( filename );
    TextureID.push_back ( int ( ) );
    glGenTextures ( 1, &TextureID[TextureID.size ( ) - 1] );
    glBindTexture ( GL_TEXTURE_2D, TextureID[TextureID.size ( ) - 1] );
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    gluBuild2DMipmaps ( GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}

using namespace std;

int fps = 60;
float speedValue = 1.5f;

struct Vector3f{
    float x,y,z;
}BoxAngle = { 0 , 0 , 0 },snakeSpeed = { speedValue , 0 , 0 };

struct Button{
    float x1,x2,y1,y2;
    void (*action)();
    int TexID;
    int cond;
};

vector <Button> button;

Vector3f initPos = { -80 , 80 , 106 };
Vector3f snakePos = { initPos.x , initPos.y , initPos.z };
Vector3f sysxy;
bool KeyBuffer[256];
bool Paused = 1;
int ACTIVE_FRAME = 0;
int snakeSize = 5;
int foodSize = 80;
bool Alive = 1;
vector <Vector3f> snakeTail;
vector <Vector3f> wall;
vector <Vector3f> food;

const GLfloat light_ambient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
const GLfloat light_diffuse[]  = { 0.5f, 0.4f, 0.6f, 1.0f };
const GLfloat light_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
const GLfloat light_position[] = { 0.0f, 0.0f, 200.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 0.6f, 0.6f, 0.6f, 1.0f };
const GLfloat high_shininess[] = { 128.0f };

static Vector3f randomize(){
    Vector3f tmp;
    int act_frame;
    again:
    act_frame = rand()%6;/*
    if(act_frame == 0 || act_frame == 1 || act_frame == 2 || act_frame == 3)tmp.y = rand()%190;tmp.y -= 95;
    if(act_frame == 0 || act_frame == 1 || act_frame == 4 || act_frame == 4)tmp.x = rand()%190;tmp.x -= 95;
    if(act_frame == 4 || act_frame == 5 || act_frame == 2 || act_frame == 3)tmp.z = rand()%190;tmp.z -= 95;*/

    if(act_frame == 0)tmp.z = 105, tmp.x = rand()%180-90,tmp.y = rand()%180-90;
    if(act_frame == 1)tmp.z = -105,tmp.x = rand()%180-90,tmp.y = rand()%180-90;
    if(act_frame == 2)tmp.x = -105,tmp.z = rand()%180-90,tmp.y = rand()%180-90;
    if(act_frame == 3)tmp.x = 105, tmp.z = rand()%180-90,tmp.y = rand()%180-90;
    if(act_frame == 4)tmp.y = 105, tmp.x = rand()%180-90,tmp.z = rand()%180-90;
    if(act_frame == 5)tmp.y = -105,tmp.x = rand()%180-90,tmp.z = rand()%180-90;

    if( snakePos.x > tmp.x - 10 &&
        snakePos.x < tmp.x + 10 &&
        snakePos.y > tmp.y - 10 &&
        snakePos.y < tmp.y + 10 &&
        snakePos.z > tmp.z - 10 &&
        snakePos.z < tmp.z + 10 )goto again;

    for(unsigned int i = 0; i<= snakeTail.size(); i+=5*speedValue){
        if(tmp.x > snakeTail[i].x - 10 &&
           tmp.x < snakeTail[i].x + 10 &&
           tmp.y > snakeTail[i].y - 10 &&
           tmp.y < snakeTail[i].y + 10 &&
           tmp.z > snakeTail[i].z - 10 &&
           tmp.z < snakeTail[i].z + 10)goto again;
    }

    for(unsigned int i = 0; i< food.size(); i++){
        if(tmp.x > food[i].x - 10 &&
           tmp.x < food[i].x + 10 &&
           tmp.y > food[i].y - 10 &&
           tmp.y < food[i].y + 10 &&
           tmp.z > food[i].z - 10 &&
           tmp.z < food[i].z + 10)goto again;
    }

    return tmp;
}

static void resize(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0,(GLfloat)width/(GLfloat)height,0.1,1000);
    glClearColor(125.0f/255.0f,255.0f/255.0f,203.0f/255.0f,1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void Menu(){
    glDisable ( GL_LIGHTING );
    glDisable ( GL_CULL_FACE );
    glDisable ( GL_DEPTH_TEST );
    glMatrixMode ( GL_PROJECTION );
    glPushMatrix ( );
    glLoadIdentity ( );
    gluOrtho2D ( 0,sysxy.x,sysxy.y,0 );
    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ( );
    glLoadIdentity ( );

    /// MENU
                        glEnable ( GL_BLEND );
                        glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );


                        glBegin ( GL_QUADS );
                        glColor4f ( 0.4, 0.4, 0.6, 0.9 );   glVertex2f ( 80, sysxy.y-60 );
                        glColor4f ( 0.8, 0.8, 0.8, 0.0 );   glVertex2f ( sysxy.x-80, sysxy.y-60 );
                        glColor4f ( 1.0, 0.8, 1.0, 0.0 );   glVertex2f ( sysxy.x-80, 60 );
                        glColor4f ( 0.5, 0.4, 0.4, 0.9 );   glVertex2f ( 80, 60 );
                        glEnd ( );
    /// BUTTONS
        for(unsigned int i = 0; i<button.size(); i++){
            if(button[i].cond == (int)NULL || button[i].cond == Alive){
                if(button[i].TexID!=-1)glEnable(GL_TEXTURE_2D);
                if(button[i].TexID!=-1)glBindTexture(GL_TEXTURE_2D,TextureID[button[i].TexID]);
                glBegin ( GL_QUADS );
                glColor4f(1,1,1,0.8);     glTexCoord2f(0.0,1.0);    glVertex2f ( button[i].x1 , button[i].y1 );
                glColor4f(1,1,1,0.2);     glTexCoord2f(1.0,1.0);    glVertex2f ( button[i].x2 , button[i].y1 );
                glColor4f(1,1,1,0.2);     glTexCoord2f(1.0,0.0);    glVertex2f ( button[i].x2 , button[i].y2 );
                glColor4f(1,1,1,0.8);     glTexCoord2f(0.0,0.0);    glVertex2f ( button[i].x1 , button[i].y2 );
                glEnd ( );
                glDisable(GL_TEXTURE_2D);
            }
        }
                        glDisable ( GL_BLEND );

    glMatrixMode ( GL_PROJECTION );
    glPopMatrix ( );
    glMatrixMode ( GL_MODELVIEW );
    glPopMatrix ( );

    glEnable ( GL_LIGHTING );
    glEnable ( GL_CULL_FACE );
    glEnable ( GL_DEPTH_TEST );

    BoxAngle.x+=1;BoxAngle.y+=1;BoxAngle.z+=1;
}

static void display(void)
{
    glClear( GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0,0,-700);
    glRotatef(BoxAngle.y,1,0,0);
    glRotatef(BoxAngle.x,0,1,0);
    glRotatef(BoxAngle.z,0,0,1);

    /// CUBE

    glBegin(GL_QUADS);
    glColor3f(0,0,1);
    glNormal3f(0.0,1.0,1.0);    glVertex3f(-100,-100, 100);
    glNormal3f(1.0,1.0,1.0);    glVertex3f( 100,-100, 100);
    glNormal3f(1.0,1.0,0.0);    glVertex3f( 100, 100, 100);
    glNormal3f(0.0,1.0,0.0);    glVertex3f(-100, 100, 100);
    glColor3f(0,1,0);
    glNormal3f(0.0,1.0,-1.0);    glVertex3f( 100,-100,-100);
    glNormal3f(1.0,1.0,-1.0);    glVertex3f(-100,-100,-100);
    glNormal3f(1.0,1.0,0.0);    glVertex3f(-100, 100,-100);
    glNormal3f(0.0,1.0,0.0);    glVertex3f( 100, 100,-100);
    glColor3f(1,1,1);
    glNormal3f(0.0,-1.0,1.0);    glVertex3f( 100,-100, 100);
    glNormal3f(1.0,-1.0,1.0);    glVertex3f(-100,-100, 100);
    glNormal3f(1.0,-1.0,0.0);    glVertex3f(-100,-100,-100);
    glNormal3f(0.0,-1.0,0.0);    glVertex3f( 100,-100,-100);
    glColor3f(1,1,0);
    glNormal3f(0.0,1.0,1.0);    glVertex3f(-100, 100, 100);
    glNormal3f(1.0,1.0,1.0);    glVertex3f( 100, 100, 100);
    glNormal3f(1.0,1.0,0.0);    glVertex3f( 100, 100,-100);
    glNormal3f(0.0,1.0,0.0);    glVertex3f(-100, 100,-100);
    glColor3f(1,0,0);
    glNormal3f(0.0,1.0,1.0);    glVertex3f( 100,-100, 100);
    glNormal3f(1.0,1.0,1.0);    glVertex3f( 100,-100,-100);
    glNormal3f(1.0,1.0,0.0);    glVertex3f( 100, 100,-100);
    glNormal3f(0.0,1.0,0.0);    glVertex3f( 100, 100, 100);
    glColor3f(1,0.396078f,0);
    glNormal3f(0.0,1.0,1.0);    glVertex3f(-100,-100,-100);
    glNormal3f(-1.0,1.0,1.0);    glVertex3f(-100,-100, 100);
    glNormal3f(-1.0,1.0,0.0);    glVertex3f(-100, 100, 100);
    glNormal3f(0.0,1.0,0.0);    glVertex3f(-100, 100,-100);

    /// SNAKE

    glColor3f(51.0f/255.0f,189.0f/255.0f,4.0f/255.0f);
    glNormal3f(0.0,1.0,1.0);    glVertex3f(snakePos.x-5,snakePos.y-5,snakePos.z+5);
    glNormal3f(1.0,1.0,1.0);    glVertex3f(snakePos.x+5,snakePos.y-5,snakePos.z+5);
    glNormal3f(1.0,1.0,0.0);    glVertex3f(snakePos.x+5,snakePos.y+5,snakePos.z+5);
    glNormal3f(0.0,1.0,0.0);    glVertex3f(snakePos.x-5,snakePos.y+5,snakePos.z+5);
    glNormal3f(0.0,1.0,-1.0);    glVertex3f(snakePos.x+5,snakePos.y-5,snakePos.z-5);
    glNormal3f(1.0,1.0,-1.0);    glVertex3f(snakePos.x-5,snakePos.y-5,snakePos.z-5);
    glNormal3f(1.0,1.0,0.0);    glVertex3f(snakePos.x-5,snakePos.y+5,snakePos.z-5);
    glNormal3f(0.0,1.0,0.0);    glVertex3f(snakePos.x+5,snakePos.y+5,snakePos.z-5);
    glNormal3f(0.0,-1.0,1.0);    glVertex3f(snakePos.x+5,snakePos.y-5,snakePos.z+5);
    glNormal3f(1.0,-1.0,1.0);    glVertex3f(snakePos.x-5,snakePos.y-5,snakePos.z+5);
    glNormal3f(1.0,-1.0,0.0);    glVertex3f(snakePos.x-5,snakePos.y-5,snakePos.z-5);
    glNormal3f(0.0,-1.0,0.0);    glVertex3f(snakePos.x+5,snakePos.y-5,snakePos.z-5);
    glNormal3f(0.0,1.0,1.0);    glVertex3f(snakePos.x-5,snakePos.y+5,snakePos.z+5);
    glNormal3f(1.0,1.0,1.0);    glVertex3f(snakePos.x+5,snakePos.y+5,snakePos.z+5);
    glNormal3f(1.0,1.0,0.0);    glVertex3f(snakePos.x+5,snakePos.y+5,snakePos.z-5);
    glNormal3f(0.0,1.0,0.0);    glVertex3f(snakePos.x-5,snakePos.y+5,snakePos.z-5);
    glNormal3f(0.0,1.0,1.0);    glVertex3f(snakePos.x+5,snakePos.y-5,snakePos.z+5);
    glNormal3f(1.0,1.0,1.0);    glVertex3f(snakePos.x+5,snakePos.y-5,snakePos.z-5);
    glNormal3f(1.0,1.0,0.0);    glVertex3f(snakePos.x+5,snakePos.y+5,snakePos.z-5);
    glNormal3f(0.0,1.0,0.0);    glVertex3f(snakePos.x+5,snakePos.y+5,snakePos.z+5);
    glNormal3f(0.0,1.0,1.0);    glVertex3f(snakePos.x-5,snakePos.y-5,snakePos.z-5);
    glNormal3f(-1.0,1.0,1.0);    glVertex3f(snakePos.x-5,snakePos.y-5,snakePos.z+5);
    glNormal3f(-1.0,1.0,0.0);    glVertex3f(snakePos.x-5,snakePos.y+5,snakePos.z+5);
    glNormal3f(0.0,1.0,0.0);    glVertex3f(snakePos.x-5,snakePos.y+5,snakePos.z-5);

    /// TAIL

    glColor3f(166.0f/255.0f,201.0f/255.0f,61.0f/255.0f);
    for(unsigned int i = 10; i<= snakeTail.size(); i+=5*speedValue){
        glNormal3f(0.0,1.0,1.0);     glVertex3f(snakeTail[i].x-4.99,snakeTail[i].y-4.99,snakeTail[i].z+4.99);
        glNormal3f(1.0,1.0,1.0);     glVertex3f(snakeTail[i].x+4.99,snakeTail[i].y-4.99,snakeTail[i].z+4.99);
        glNormal3f(1.0,1.0,0.0);     glVertex3f(snakeTail[i].x+4.99,snakeTail[i].y+4.99,snakeTail[i].z+4.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(snakeTail[i].x-4.99,snakeTail[i].y+4.99,snakeTail[i].z+4.99);
        glNormal3f(0.0,1.0,-1.0);    glVertex3f(snakeTail[i].x+4.99,snakeTail[i].y-4.99,snakeTail[i].z-4.99);
        glNormal3f(1.0,1.0,-1.0);    glVertex3f(snakeTail[i].x-4.99,snakeTail[i].y-4.99,snakeTail[i].z-4.99);
        glNormal3f(1.0,1.0,0.0);     glVertex3f(snakeTail[i].x-4.99,snakeTail[i].y+4.99,snakeTail[i].z-4.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(snakeTail[i].x+4.99,snakeTail[i].y+4.99,snakeTail[i].z-4.99);
        glNormal3f(0.0,-1.0,1.0);    glVertex3f(snakeTail[i].x+4.99,snakeTail[i].y-4.99,snakeTail[i].z+4.99);
        glNormal3f(1.0,-1.0,1.0);    glVertex3f(snakeTail[i].x-4.99,snakeTail[i].y-4.99,snakeTail[i].z+4.99);
        glNormal3f(1.0,-1.0,0.0);    glVertex3f(snakeTail[i].x-4.99,snakeTail[i].y-4.99,snakeTail[i].z-4.99);
        glNormal3f(0.0,-1.0,0.0);    glVertex3f(snakeTail[i].x+4.99,snakeTail[i].y-4.99,snakeTail[i].z-4.99);
        glNormal3f(0.0,1.0,1.0);     glVertex3f(snakeTail[i].x-4.99,snakeTail[i].y+4.99,snakeTail[i].z+4.99);
        glNormal3f(1.0,1.0,1.0);     glVertex3f(snakeTail[i].x+4.99,snakeTail[i].y+4.99,snakeTail[i].z+4.99);
        glNormal3f(1.0,1.0,0.0);     glVertex3f(snakeTail[i].x+4.99,snakeTail[i].y+4.99,snakeTail[i].z-4.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(snakeTail[i].x-4.99,snakeTail[i].y+4.99,snakeTail[i].z-4.99);
        glNormal3f(0.0,1.0,1.0);     glVertex3f(snakeTail[i].x+4.99,snakeTail[i].y-4.99,snakeTail[i].z+4.99);
        glNormal3f(1.0,1.0,1.0);     glVertex3f(snakeTail[i].x+4.99,snakeTail[i].y-4.99,snakeTail[i].z-4.99);
        glNormal3f(1.0,1.0,0.0);     glVertex3f(snakeTail[i].x+4.99,snakeTail[i].y+4.99,snakeTail[i].z-4.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(snakeTail[i].x+4.99,snakeTail[i].y+4.99,snakeTail[i].z+4.99);
        glNormal3f(0.0,1.0,1.0);     glVertex3f(snakeTail[i].x-4.99,snakeTail[i].y-4.99,snakeTail[i].z-4.99);
        glNormal3f(-1.0,1.0,1.0);    glVertex3f(snakeTail[i].x-4.99,snakeTail[i].y-4.99,snakeTail[i].z+4.99);
        glNormal3f(-1.0,1.0,0.0);    glVertex3f(snakeTail[i].x-4.99,snakeTail[i].y+4.99,snakeTail[i].z+4.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(snakeTail[i].x-4.99,snakeTail[i].y+4.99,snakeTail[i].z-4.99);
    }

    /// Obstacles

    glColor3f(1,0,1);
    for(unsigned int i = 0; i< wall.size(); i++){
        glNormal3f(0.0,1.0,1.0);     glVertex3f(wall[i].x-9.99,wall[i].y-9.99,wall[i].z+9.99);
        glNormal3f(1.0,1.0,1.0);     glVertex3f(wall[i].x+9.99,wall[i].y-9.99,wall[i].z+9.99);
        glNormal3f(1.0,1.0,0.0);     glVertex3f(wall[i].x+9.99,wall[i].y+9.99,wall[i].z+9.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(wall[i].x-9.99,wall[i].y+9.99,wall[i].z+9.99);
        glNormal3f(0.0,1.0,-1.0);    glVertex3f(wall[i].x+9.99,wall[i].y-9.99,wall[i].z-9.99);
        glNormal3f(1.0,1.0,-1.0);    glVertex3f(wall[i].x-9.99,wall[i].y-9.99,wall[i].z-9.99);
        glNormal3f(1.0,1.0,0.0);     glVertex3f(wall[i].x-9.99,wall[i].y+9.99,wall[i].z-9.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(wall[i].x+9.99,wall[i].y+9.99,wall[i].z-9.99);
        glNormal3f(0.0,-1.0,1.0);    glVertex3f(wall[i].x+9.99,wall[i].y-9.99,wall[i].z+9.99);
        glNormal3f(1.0,-1.0,1.0);    glVertex3f(wall[i].x-9.99,wall[i].y-9.99,wall[i].z+9.99);
        glNormal3f(1.0,-1.0,0.0);    glVertex3f(wall[i].x-9.99,wall[i].y-9.99,wall[i].z-9.99);
        glNormal3f(0.0,-1.0,0.0);    glVertex3f(wall[i].x+9.99,wall[i].y-9.99,wall[i].z-9.99);
        glNormal3f(0.0,1.0,1.0);     glVertex3f(wall[i].x-9.99,wall[i].y+9.99,wall[i].z+9.99);
        glNormal3f(1.0,1.0,1.0);     glVertex3f(wall[i].x+9.99,wall[i].y+9.99,wall[i].z+9.99);
        glNormal3f(1.0,1.0,0.0);     glVertex3f(wall[i].x+9.99,wall[i].y+9.99,wall[i].z-9.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(wall[i].x-9.99,wall[i].y+9.99,wall[i].z-9.99);
        glNormal3f(0.0,1.0,1.0);     glVertex3f(wall[i].x+9.99,wall[i].y-9.99,wall[i].z+9.99);
        glNormal3f(1.0,1.0,1.0);     glVertex3f(wall[i].x+9.99,wall[i].y-9.99,wall[i].z-9.99);
        glNormal3f(1.0,1.0,0.0);     glVertex3f(wall[i].x+9.99,wall[i].y+9.99,wall[i].z-9.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(wall[i].x+9.99,wall[i].y+9.99,wall[i].z+9.99);
        glNormal3f(0.0,1.0,1.0);     glVertex3f(wall[i].x-9.99,wall[i].y-9.99,wall[i].z-9.99);
        glNormal3f(-1.0,1.0,1.0);    glVertex3f(wall[i].x-9.99,wall[i].y-9.99,wall[i].z+9.99);
        glNormal3f(-1.0,1.0,0.0);    glVertex3f(wall[i].x-9.99,wall[i].y+9.99,wall[i].z+9.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(wall[i].x-9.99,wall[i].y+9.99,wall[i].z-9.99);
    }

    /// Food

    glColor3f(253.0f/255.0f,133.0f/255.0f,45.0f/255.0f);
    for(unsigned int i = 0; i< food.size(); i++){
        glNormal3f(0.0,1.0,1.0);     glVertex3f(food[i].x-4.99,food[i].y-4.99,food[i].z+4.99);
        glNormal3f(1.0,1.0,1.0);     glVertex3f(food[i].x+4.99,food[i].y-4.99,food[i].z+4.99);
        glNormal3f(1.0,1.0,0.0);     glVertex3f(food[i].x+4.99,food[i].y+4.99,food[i].z+4.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(food[i].x-4.99,food[i].y+4.99,food[i].z+4.99);
        glNormal3f(0.0,1.0,-1.0);    glVertex3f(food[i].x+4.99,food[i].y-4.99,food[i].z-4.99);
        glNormal3f(1.0,1.0,-1.0);    glVertex3f(food[i].x-4.99,food[i].y-4.99,food[i].z-4.99);
        glNormal3f(1.0,1.0,0.0);     glVertex3f(food[i].x-4.99,food[i].y+4.99,food[i].z-4.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(food[i].x+4.99,food[i].y+4.99,food[i].z-4.99);
        glNormal3f(0.0,-1.0,1.0);    glVertex3f(food[i].x+4.99,food[i].y-4.99,food[i].z+4.99);
        glNormal3f(1.0,-1.0,1.0);    glVertex3f(food[i].x-4.99,food[i].y-4.99,food[i].z+4.99);
        glNormal3f(1.0,-1.0,0.0);    glVertex3f(food[i].x-4.99,food[i].y-4.99,food[i].z-4.99);
        glNormal3f(0.0,-1.0,0.0);    glVertex3f(food[i].x+4.99,food[i].y-4.99,food[i].z-4.99);
        glNormal3f(0.0,1.0,1.0);     glVertex3f(food[i].x-4.99,food[i].y+4.99,food[i].z+4.99);
        glNormal3f(1.0,1.0,1.0);     glVertex3f(food[i].x+4.99,food[i].y+4.99,food[i].z+4.99);
        glNormal3f(1.0,1.0,0.0);     glVertex3f(food[i].x+4.99,food[i].y+4.99,food[i].z-4.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(food[i].x-4.99,food[i].y+4.99,food[i].z-4.99);
        glNormal3f(0.0,1.0,1.0);     glVertex3f(food[i].x+4.99,food[i].y-4.99,food[i].z+4.99);
        glNormal3f(1.0,1.0,1.0);     glVertex3f(food[i].x+4.99,food[i].y-4.99,food[i].z-4.99);
        glNormal3f(1.0,1.0,0.0);     glVertex3f(food[i].x+4.99,food[i].y+4.99,food[i].z-4.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(food[i].x+4.99,food[i].y+4.99,food[i].z+4.99);
        glNormal3f(0.0,1.0,1.0);     glVertex3f(food[i].x-4.99,food[i].y-4.99,food[i].z-4.99);
        glNormal3f(-1.0,1.0,1.0);    glVertex3f(food[i].x-4.99,food[i].y-4.99,food[i].z+4.99);
        glNormal3f(-1.0,1.0,0.0);    glVertex3f(food[i].x-4.99,food[i].y+4.99,food[i].z+4.99);
        glNormal3f(0.0,1.0,0.0);     glVertex3f(food[i].x-4.99,food[i].y+4.99,food[i].z-4.99);
    }
    glEnd();

    if(Paused)Menu();

    glutSwapBuffers();
}

static void snake(){
    snakeTail[0].x = snakePos.x;
    snakeTail[0].y = snakePos.y;
    snakeTail[0].z = snakePos.z;
    snakePos.x += snakeSpeed.x;
    snakePos.y += snakeSpeed.y;
    snakePos.z += snakeSpeed.z;
}

static void tail(){
    for(unsigned int i = snakeTail.size(); i > 0; i--){
        snakeTail[i] = snakeTail[i-1];
    }
}

static void reset(){
    snakePos = (Vector3f){ initPos.x , initPos.y , initPos.z };
    snakeSpeed = (Vector3f){ speedValue , 0 , 0 };
    ACTIVE_FRAME = 0;
    snakeSize = 5;
    snakeTail.clear();
    for(int i = 0; i < snakeSize*5*speedValue+1; i++)snakeTail.push_back(Vector3f());
    for(unsigned int i = 0; i<food.size(); i++)food[i] = randomize();
    Alive = 1;
}

static void key(unsigned char key, int x, int y)
{
    if(Alive && key == 27)Paused?Paused = 0:Paused = 1;
    if(key == 32 && Alive)Paused = 0;
    if(key == 'r' && Paused)reset(),Paused=0;
    if(key == 'q' && Paused)exit(0);
    KeyBuffer[key]=true;
    if(ACTIVE_FRAME == 0 && Paused == 0){
        if(key == 'a' && snakeSpeed.x == 0)snakeSpeed.y=0,snakeSpeed.z=0,snakeSpeed.x=-speedValue;
        if(key == 'd' && snakeSpeed.x == 0)snakeSpeed.y=0,snakeSpeed.z=0,snakeSpeed.x=speedValue;
        if(key == 's' && snakeSpeed.y == 0)snakeSpeed.y=-speedValue,snakeSpeed.z=0,snakeSpeed.x=0;
        if(key == 'w' && snakeSpeed.y == 0)snakeSpeed.y=speedValue,snakeSpeed.z=0,snakeSpeed.x=0;
    }
    if(ACTIVE_FRAME == 1 && Paused == 0){
        if(key == 'a' && snakeSpeed.x == 0)snakeSpeed.y=0,snakeSpeed.z=0,snakeSpeed.x=speedValue;
        if(key == 'd' && snakeSpeed.x == 0)snakeSpeed.y=0,snakeSpeed.z=0,snakeSpeed.x=-speedValue;
        if(key == 's' && snakeSpeed.y == 0)snakeSpeed.y=-speedValue,snakeSpeed.z=0,snakeSpeed.x=0;
        if(key == 'w' && snakeSpeed.y == 0)snakeSpeed.y=speedValue,snakeSpeed.z=0,snakeSpeed.x=0;
    }
    if(ACTIVE_FRAME == 2 && Paused == 0){
        if(key == 'a' && snakeSpeed.z == 0)snakeSpeed.y=0,snakeSpeed.z=-speedValue,snakeSpeed.x=0;
        if(key == 'd' && snakeSpeed.z == 0)snakeSpeed.y=0,snakeSpeed.z=speedValue,snakeSpeed.x=0;
        if(key == 's' && snakeSpeed.y == 0)snakeSpeed.y=-speedValue,snakeSpeed.z=0,snakeSpeed.x=0;
        if(key == 'w' && snakeSpeed.y == 0)snakeSpeed.y=speedValue,snakeSpeed.z=0,snakeSpeed.x=0;
    }
    if(ACTIVE_FRAME == 3 && Paused == 0){
        if(key == 'a' && snakeSpeed.z == 0)snakeSpeed.y=0,snakeSpeed.z=speedValue,snakeSpeed.x=0;
        if(key == 'd' && snakeSpeed.z == 0)snakeSpeed.y=0,snakeSpeed.z=-speedValue,snakeSpeed.x=0;
        if(key == 's' && snakeSpeed.y == 0)snakeSpeed.y=-speedValue,snakeSpeed.z=0,snakeSpeed.x=0;
        if(key == 'w' && snakeSpeed.y == 0)snakeSpeed.y=speedValue,snakeSpeed.z=0,snakeSpeed.x=0;
    }
    if(ACTIVE_FRAME == 4 && Paused == 0){
        if(key == 'a' && snakeSpeed.x == 0)snakeSpeed.y=0,snakeSpeed.z=0,snakeSpeed.x=-speedValue;
        if(key == 'd' && snakeSpeed.x == 0)snakeSpeed.y=0,snakeSpeed.z=0,snakeSpeed.x=speedValue;
        if(key == 's' && snakeSpeed.z == 0)snakeSpeed.y=0,snakeSpeed.z=speedValue,snakeSpeed.x=0;
        if(key == 'w' && snakeSpeed.z == 0)snakeSpeed.y=0,snakeSpeed.z=-speedValue,snakeSpeed.x=0;
    }
    if(ACTIVE_FRAME == 5 && Paused == 0){
        if(key == 'a' && snakeSpeed.x == 0)snakeSpeed.y=0,snakeSpeed.z=0,snakeSpeed.x=-speedValue;
        if(key == 'd' && snakeSpeed.x == 0)snakeSpeed.y=0,snakeSpeed.z=0,snakeSpeed.x=speedValue;
        if(key == 's' && snakeSpeed.z == 0)snakeSpeed.y=0,snakeSpeed.z=-speedValue,snakeSpeed.x=0;
        if(key == 'w' && snakeSpeed.z == 0)snakeSpeed.y=0,snakeSpeed.z=speedValue,snakeSpeed.x=0;
    }
}

static void keyup(unsigned char key, int x, int y)
{
    KeyBuffer[key]=false;
}

static void keyCheck(){

}

static void Rotate(){
    if(BoxAngle.x > 360.0f)BoxAngle.x = BoxAngle.x - 360.0f;
    if(BoxAngle.x < 0.0f)BoxAngle.x = 360.0f + BoxAngle.x ;
    if(BoxAngle.y > 360.0f)BoxAngle.y = BoxAngle.y - 360.0f;
    if(BoxAngle.y < 0.0f)BoxAngle.y = 360.0f + BoxAngle.y ;
    if(BoxAngle.z > 360.0f)BoxAngle.z = BoxAngle.z - 360.0f;
    if(BoxAngle.z < 0.0f)BoxAngle.z = 360.0f + BoxAngle.z ;

    if(ACTIVE_FRAME == 0){
    if(BoxAngle.x < 180.0f && BoxAngle.x >= 0.0f)BoxAngle.x -= (BoxAngle.x)/20;
    if(BoxAngle.x < 360.0f && BoxAngle.x >= 180.0f)BoxAngle.x += (360.0f - BoxAngle.x)/20;
    if(BoxAngle.y < 180.0f && BoxAngle.y >= 0.0f)BoxAngle.y -= (BoxAngle.y)/20;
    if(BoxAngle.y < 360.0f && BoxAngle.y >= 180.0f)BoxAngle.y += (360.0f - BoxAngle.y)/20;
    if(BoxAngle.z < 180.0f && BoxAngle.z >= 0.0f)BoxAngle.z -= (BoxAngle.z)/20;
    if(BoxAngle.z < 360.0f && BoxAngle.z >= 180.0f)BoxAngle.z += (360.0f - BoxAngle.z)/20;
    BoxAngle.x-=(snakePos.x/60.0f);BoxAngle.y+=(snakePos.y/60.0f);
    if(snakePos.x > 106.0f)snakeSpeed.x = 0.0f,snakeSpeed.z = -speedValue,ACTIVE_FRAME = 3,snakePos.x = 106.0f;
    if(snakePos.x <-106.0f)snakeSpeed.x = 0.0f,snakeSpeed.z = -speedValue,ACTIVE_FRAME = 2,snakePos.x =-106.0f;
    if(snakePos.y > 106.0f)snakeSpeed.y = 0.0f,snakeSpeed.z = -speedValue,ACTIVE_FRAME = 4,snakePos.y = 106.0f;
    if(snakePos.y <-106.0f)snakeSpeed.y = 0.0f,snakeSpeed.z = -speedValue,ACTIVE_FRAME = 5,snakePos.y =-106.0f;
    }
    if(ACTIVE_FRAME == 1){
    if(BoxAngle.x < 180.0f && BoxAngle.x >= 0.0f)BoxAngle.x += ( 180.0f - BoxAngle.x)/20;
    if(BoxAngle.x < 360.0f && BoxAngle.x >= 180.0f)BoxAngle.x -= (BoxAngle.x - 180.0f)/20;
    if(BoxAngle.y < 180.0f && BoxAngle.y >= 0.0f)BoxAngle.y -= (BoxAngle.y)/20;
    if(BoxAngle.y < 360.0f && BoxAngle.y >= 180.0f)BoxAngle.y += (360.0f - BoxAngle.y)/20;
    if(BoxAngle.z < 180.0f && BoxAngle.z >= 0.0f)BoxAngle.z -= (BoxAngle.z)/20;
    if(BoxAngle.z < 360.0f && BoxAngle.z >= 180.0f)BoxAngle.z += (360.0f - BoxAngle.z)/20;
    BoxAngle.x+=(snakePos.x/60.0f);BoxAngle.y+=(snakePos.y/60.0f);
    if(snakePos.x > 106.0f)snakeSpeed.x = 0.0f,snakeSpeed.z = speedValue,ACTIVE_FRAME = 3,snakePos.x = 106.0f;
    if(snakePos.x <-106.0f)snakeSpeed.x = 0.0f,snakeSpeed.z = speedValue,ACTIVE_FRAME = 2,snakePos.x =-106.0f;
    if(snakePos.y > 106.0f)snakeSpeed.y = 0.0f,snakeSpeed.z = speedValue,ACTIVE_FRAME = 4,snakePos.y = 106.0f;
    if(snakePos.y <-106.0f)snakeSpeed.y = 0.0f,snakeSpeed.z = speedValue,ACTIVE_FRAME = 5,snakePos.y =-106.0f;
    }
    if(ACTIVE_FRAME == 2){
    if(BoxAngle.x < 90.0f && BoxAngle.x >= 0.0f)BoxAngle.x += (90.0f - BoxAngle.x)/20;
    if(BoxAngle.x < 270.0f && BoxAngle.x >= 90.0f)BoxAngle.x -= (BoxAngle.x - 90.0f)/20;
    if(BoxAngle.x < 360.0f && BoxAngle.x >= 270.0f)BoxAngle.x += (450.0f - BoxAngle.x)/20;
    if(BoxAngle.y < 180.0f && BoxAngle.y >= 0.0f)BoxAngle.y -= (BoxAngle.y)/20;
    if(BoxAngle.y < 360.0f && BoxAngle.y >= 180.0f)BoxAngle.y += (360.0f - BoxAngle.y)/20;
    if(BoxAngle.z < 180.0f && BoxAngle.z >= 0.0f)BoxAngle.z -= (BoxAngle.z)/20;
    if(BoxAngle.z < 360.0f && BoxAngle.z >= 180.0f)BoxAngle.z += (360.0f - BoxAngle.z)/20;
    BoxAngle.z+=(snakePos.y/60.0f);BoxAngle.x-=(snakePos.z/60.0f);
    if(snakePos.z > 106.0f)snakeSpeed.z = 0.0f,snakeSpeed.x = speedValue,ACTIVE_FRAME = 0,snakePos.z = 106.0f;
    if(snakePos.z <-106.0f)snakeSpeed.z = 0.0f,snakeSpeed.x = speedValue,ACTIVE_FRAME = 1,snakePos.z =-106.0f;
    if(snakePos.y > 106.0f)snakeSpeed.y = 0.0f,snakeSpeed.x = speedValue,ACTIVE_FRAME = 4,snakePos.y = 106.0f;
    if(snakePos.y <-106.0f)snakeSpeed.y = 0.0f,snakeSpeed.x = speedValue,ACTIVE_FRAME = 5,snakePos.y =-106.0f;
    }
    if(ACTIVE_FRAME == 3){
    if(BoxAngle.x < 90.0f && BoxAngle.x >= 0.0f)BoxAngle.x -= (BoxAngle.x + 90.0f)/20;
    if(BoxAngle.x < 270.0f && BoxAngle.x >= 90.0f)BoxAngle.x += (270.0f - BoxAngle.x)/20;
    if(BoxAngle.x < 360.0f && BoxAngle.x >= 270.0f)BoxAngle.x -= (BoxAngle.x - 270.0f)/20;
    if(BoxAngle.y < 180.0f && BoxAngle.y >= 0.0f)BoxAngle.y -= (BoxAngle.y)/20;
    if(BoxAngle.y < 360.0f && BoxAngle.y >= 180.0f)BoxAngle.y += (360.0f - BoxAngle.y)/20;
    if(BoxAngle.z < 180.0f && BoxAngle.z >= 0.0f)BoxAngle.z -= (BoxAngle.z)/20;
    if(BoxAngle.z < 360.0f && BoxAngle.z >= 180.0f)BoxAngle.z += (360.0f - BoxAngle.z)/20;
    BoxAngle.z-=(snakePos.y/60.0f);BoxAngle.x+=(snakePos.z/60.0f);
    if(snakePos.z > 106.0f)snakeSpeed.z = 0.0f,snakeSpeed.x =-speedValue,ACTIVE_FRAME = 0,snakePos.z = 106.0f;
    if(snakePos.z <-106.0f)snakeSpeed.z = 0.0f,snakeSpeed.x =-speedValue,ACTIVE_FRAME = 1,snakePos.z =-106.0f;
    if(snakePos.y > 106.0f)snakeSpeed.y = 0.0f,snakeSpeed.x =-speedValue,ACTIVE_FRAME = 4,snakePos.y = 106.0f;
    if(snakePos.y <-106.0f)snakeSpeed.y = 0.0f,snakeSpeed.x =-speedValue,ACTIVE_FRAME = 5,snakePos.y =-106.0f;
    }
    if(ACTIVE_FRAME == 4){
    if(BoxAngle.x < 180.0f && BoxAngle.x >= 0.0f)BoxAngle.x -= (BoxAngle.x)/20;
    if(BoxAngle.x < 360.0f && BoxAngle.x >= 180.0f)BoxAngle.x += (360.0f - BoxAngle.x)/20;
    if(BoxAngle.y < 90.0f && BoxAngle.y >= 0.0f)BoxAngle.y += (90.0f - BoxAngle.y)/20;
    if(BoxAngle.y < 270.0f && BoxAngle.y >= 90.0f)BoxAngle.y -= (BoxAngle.y - 90.0f)/20;
    if(BoxAngle.y < 360.0f && BoxAngle.y >= 270.0f)BoxAngle.y += (450.0f - BoxAngle.y)/20;
    if(BoxAngle.z < 180.0f && BoxAngle.z >= 0.0f)BoxAngle.z -= (BoxAngle.z)/20;
    if(BoxAngle.z < 360.0f && BoxAngle.z >= 180.0f)BoxAngle.z += (360.0f - BoxAngle.z)/20;
    BoxAngle.z+=(snakePos.x/60.0f);BoxAngle.y-=(snakePos.z/60.0f);
    if(snakePos.z > 106.0f)snakeSpeed.z = 0.0f,snakeSpeed.y =-speedValue,ACTIVE_FRAME = 0,snakePos.z = 106.0f;
    if(snakePos.z <-106.0f)snakeSpeed.z = 0.0f,snakeSpeed.y =-speedValue,ACTIVE_FRAME = 1,snakePos.z =-106.0f;
    if(snakePos.x > 106.0f)snakeSpeed.x = 0.0f,snakeSpeed.y =-speedValue,ACTIVE_FRAME = 3,snakePos.x = 106.0f;
    if(snakePos.x <-106.0f)snakeSpeed.x = 0.0f,snakeSpeed.y =-speedValue,ACTIVE_FRAME = 2,snakePos.x =-106.0f;
    }
    if(ACTIVE_FRAME == 5){
    if(BoxAngle.x < 180.0f && BoxAngle.x >= 0.0f)BoxAngle.x -= (BoxAngle.x)/20;
    if(BoxAngle.x < 360.0f && BoxAngle.x >= 180.0f)BoxAngle.x += (360.0f - BoxAngle.x)/20;
    if(BoxAngle.y < 90.0f && BoxAngle.y >= 0.0f)BoxAngle.y -= (BoxAngle.y + 90.0f)/20;
    if(BoxAngle.y < 270.0f && BoxAngle.y >= 90.0f)BoxAngle.y += (270.0f - BoxAngle.y)/20;
    if(BoxAngle.y < 360.0f && BoxAngle.y >= 270.0f)BoxAngle.y -= (BoxAngle.y - 270.0f)/20;
    if(BoxAngle.z < 180.0f && BoxAngle.z >= 0.0f)BoxAngle.z -= (BoxAngle.z)/20;
    if(BoxAngle.z < 360.0f && BoxAngle.z >= 180.0f)BoxAngle.z += (360.0f - BoxAngle.z)/20;
    BoxAngle.z-=(snakePos.x/60.0f);BoxAngle.y+=(snakePos.z/60.0f);
    if(snakePos.z > 106.0f)snakeSpeed.z = 0.0f,snakeSpeed.y = speedValue,ACTIVE_FRAME = 0,snakePos.z = 106.0f;
    if(snakePos.z <-106.0f)snakeSpeed.z = 0.0f,snakeSpeed.y = speedValue,ACTIVE_FRAME = 1,snakePos.z =-106.0f;
    if(snakePos.x > 106.0f)snakeSpeed.x = 0.0f,snakeSpeed.y = speedValue,ACTIVE_FRAME = 3,snakePos.x = 106.0f;
    if(snakePos.x <-106.0f)snakeSpeed.x = 0.0f,snakeSpeed.y = speedValue,ACTIVE_FRAME = 2,snakePos.x =-106.0f;
    }
}

static void resetGame(){
    reset();Paused = 0;
}

static void exitGame(){
    exit(0);
}

static void resume(){
    if(Alive)Paused = 0;
}

static void collisionCheck(){
    for(unsigned int i = 0; i< wall.size(); i++){
        if(snakePos.x > wall[i].x - 17 &&
           snakePos.x < wall[i].x + 17 &&
           snakePos.y > wall[i].y - 17 &&
           snakePos.y < wall[i].y + 17 &&
           snakePos.z > wall[i].z - 17 &&
           snakePos.z < wall[i].z + 17)Paused = 1,Alive = 0;
    }
    for(unsigned int i = 23; i<= snakeTail.size(); i+=5*speedValue){
        if(snakePos.x > snakeTail[i].x - 10 &&
           snakePos.x < snakeTail[i].x + 10 &&
           snakePos.y > snakeTail[i].y - 10 &&
           snakePos.y < snakeTail[i].y + 10 &&
           snakePos.z > snakeTail[i].z - 10 &&
           snakePos.z < snakeTail[i].z + 10)Paused = 1,Alive = 0;
    }
    for(unsigned int i = 0; i< food.size(); i++){
        if(snakePos.x > food[i].x - 10 &&
           snakePos.x < food[i].x + 10 &&
           snakePos.y > food[i].y - 10 &&
           snakePos.y < food[i].y + 10 &&
           snakePos.z > food[i].z - 10 &&
           snakePos.z < food[i].z + 10){
               for(int j = 0; j<5*speedValue; j++)snakeTail.push_back(Vector3f());
               food[i] = randomize();
           }
    }
}

static void none(){}

static void Timer(int)
{
    keyCheck();
    if(!Paused)Rotate();
    if(!Paused)snake();
    if(!Paused)tail();
    if(!Paused)collisionCheck();
    glutTimerFunc(1000/fps,Timer,0);
    glutPostRedisplay();
}

static void mouse(int BUTTON, int STATE, int x, int y){
    for(unsigned int i = 0; i<button.size(); i++){
        if( Paused && x > button[i].x1 && x < button[i].x2 && y > button[i].y1 && y < button[i].y2 )button[i].action();
    }
}

int main(int argc, char *argv[])
{
    for(int i = 0; i < snakeSize*5*speedValue; i++)snakeTail.push_back(Vector3f());
    wall.push_back((Vector3f){100,100,100});
    wall.push_back((Vector3f){-100,100,100});
    wall.push_back((Vector3f){-100,-100,100});
    wall.push_back((Vector3f){100,-100,100});
    wall.push_back((Vector3f){100,100,-100});
    wall.push_back((Vector3f){-100,100,-100});
    wall.push_back((Vector3f){100,-100,-100});
    wall.push_back((Vector3f){-100,-100,-100});
    button.push_back((Button){100,220,80,100,resume,0,Alive});
    button.push_back((Button){100,200,120,140,resetGame,1,(int)NULL});
    button.push_back((Button){100,180,160,180,exitGame,2,(int)NULL});
    srand(time(0));
    for(int i = 0; i<foodSize; i++){
        food.push_back(Vector3f());
        food[i]=randomize();
    }
    none();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("SNAKE-3D");
    glutFullScreen();
    sysxy = (Vector3f){ (float)GetSystemMetrics(SM_CXSCREEN) , (float)GetSystemMetrics(SM_CYSCREEN) };
    button.push_back((Button){sysxy.x/2-200,sysxy.x/2+200,sysxy.y/2-50,sysxy.y/2+50,none,3,Alive});

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(keyup);
    glutMouseFunc(mouse);
    glutTimerFunc(0,Timer,0);

    glClearColor(1,1,1,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

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

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    loadTexture("./res/tex/resume.bmp");
    loadTexture("./res/tex/reset.bmp");
    loadTexture("./res/tex/exit.bmp");
    loadTexture("./res/tex/space.bmp");

    glutMainLoop();

    return EXIT_SUCCESS;
}
