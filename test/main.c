#include <windows.h>
#include <gl/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.1415926535
#define PI2 PI/2
#define PI3 3*PI/2
#define DEG 0.0174533

float playerX, playerY, playerdX, playerdY, playera; //Позиция игрока(камера)

int mapX = 8, mapY = 8, mapS = 64;
int map[] =
{
  1,1,1,0,1,1,1,1,
  1,0,1,0,0,0,0,1,
  1,0,1,0,0,1,0,1,
  1,0,0,0,0,0,1,1,
  1,0,0,0,0,0,0,1,
  1,1,1,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,1,1,1,1,1,1,1,
};

void randomizeMap()
{
    int i;
    for(i = 0; i<mapS; i++)
    {
        srand(time(0));
        map[i] = (map[i]==1)? 1 : (rand() % (1 - 0 + 1) + 0);
        printf("%d", (rand() % (2 - 0 + 1) + 0));
    }
}

void drawMap2D()
{
    int x, y, xo, yo;

    for(y=0; y<mapY; y++)
    {
        for(x=0; x<mapX; x++)
        {
            if(map[y*mapX+x]==1){glColor3f(0.6,0.5,1);} else {glColor3f(0,0,0);}
            xo=x*mapS; yo=y*mapS;
            glBegin(GL_QUADS);
            glVertex2i(xo+1, yo+1);
            glVertex2i(xo+1, yo+mapS-1);
            glVertex2i(xo+mapS-1, yo+mapS-1);
            glVertex2i(xo+mapS-1, yo+1);
            glEnd();
        }
    }
}

void drawPlayer()
{
    glColor3f(1,0.5,0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(playerX, playerY);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(playerX, playerY);
    glVertex2i(playerX+playerdX*5, playerY+playerdY*5);
    glEnd();
}

float dist(float ax, float ay, float bx, float by, float ang)
{
    return (sqrt(pow((bx-ax), 2) + pow((by-ay), 2)));
}

void drawRays3D()
{
    int r, mx, my, mp, dof;
    float rx, ry, ra, xo, yo;
    float disH=1000000, hx = playerX, hy = playerY;
    float disV=1000000, vx = playerX, vy = playerY;
    float disT;
    ra=playera-30*DEG; if(ra<0){ra += 2*PI;} if(ra>2*PI){ ra-=2*PI;}

    for(r=0; r<60; r++)
    {
        //Проверка горизонтальных линий
        dof=0;
        float aTan=-1/tan(ra);
        if(ra>PI) { ry=(((int)playerY>>6)<<6)-0.0001; rx=(playerY-ry)*aTan+playerX; yo=-64; xo=-yo*aTan;}//низ
        if(ra<PI) { ry=(((int)playerY>>6)<<6)+64; rx=(playerY-ry)*aTan+playerX; yo=64; xo=-yo*aTan;}//верх
        if(ra == 0 || ra == PI) {rx=playerX; ry=playerY; dof=8;}//левая часть или право
        while(dof<8)
        {
            mx = (int)(rx)>>6; my = (int)(ry)>>6; mp=my*mapX+mx;
            if(mp>0 && (mp<mapX*mapY) && map[mp] == 1){ dof=8; hx=rx, hy=ry, disH=dist(playerX, playerY, rx, ry, ra);}//остановиться у стены
            else{ rx+=xo; ry+=yo; dof++; }//следующая стена
        }

        //Проверяет вертикальных линий
        dof=0;
        float nTan=-tan(ra);
        if(ra>PI2 && ra<PI3) { rx=(((int)playerX>>6)<<6)-0.0001; ry=(playerX-rx)*nTan+playerY; xo=-64; yo=-xo*nTan;}//левая часть
        if(ra<PI2 || ra>PI3) { rx=(((int)playerX>>6)<<6)+64; ry=(playerX-rx)*nTan+playerY; xo=64; yo=-xo*nTan;}//правая
        if(ra == 0 || ra == PI) {rx=playerX; ry=playerY; dof=8;}//вверх или вниз
        while(dof<8)
        {
            mx = (int)(rx)>>6; my = (int)(ry)>>6; mp=my*mapX+mx;
            if(mp>0 &&(mp<mapX*mapY) && map[mp] == 1){ dof=8; vx=rx, vy=ry, disV=dist(playerX, playerY, rx, ry, ra);}//стена
            else{ rx+=xo; ry+=yo; dof++; }//вверх или вниз
        }

        if(disH > disV){rx=vx; ry=vy; disT=disV; glColor3f(0,0.5,0);}
        if(disH < disV){rx=hx; ry=hy; disT=disH; glColor3f(0,0.3,0);}
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(playerX, playerY);
        glVertex2i(rx, ry);
        glEnd();
        if(rx-playerX == 0 || ry-playerY == 0) printf("%d %d",rx, ry);

        //3д стены
        float ca=playera-ra; if(ca<0){ca+=PI*2;} if(ca>2*PI){ca-=2*PI;} disT=disT*cos(ca); //коррекция рыбьего глаза
        float lineH = (mapS*320)/disT; if(lineH>320) {lineH=320;}//высота стены
        float lineO = 160-lineH/2;//линейное смещение
        glLineWidth(8); glBegin(GL_LINES); glVertex2i(r*8+530,lineO); glVertex2i(r*8+530, lineH+lineO); glEnd();

        ra+=DEG;
        if(ra<0){ra += 2*PI;} if(ra>2*PI){ ra-=2*PI;}
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawRays3D();
    drawPlayer();
    glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'd':
            playera+=0.1;
            playera -= (playera>2*PI) ? 2*PI : 0;
            playerdX = cos(playera)*5;
            playerdY = sin(playera)*5;
            break;
        case 'q':
            playera-=0.1;
            playera += (playera<0) ? 2*PI : 0;
            playerdX = cos(playera)*5;
            playerdY = sin(playera)*5;
            break;
        case 'z':
            playerY+=playerdY;
            playerX+=playerdX;
            break;
        case 's':
            playerY-=playerdY;
            playerX-=playerdX;
            break;
    }
    glutPostRedisplay();
}

void init()
{
    glClearColor(0,0,0,0);
    gluOrtho2D(0,1024,512,0);
    playerX = 200;
    playerY = 200;
    playerdX = cos(playera)*5;
    playerdY = sin(playera)*5;
}

void main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
   glutInitWindowSize(1024, 512);
   glutCreateWindow("testArena_glut");
   init();
   //randomizeMap();
   glutDisplayFunc(display);
   glutKeyboardFunc(buttons);
   glutMainLoop();
}
