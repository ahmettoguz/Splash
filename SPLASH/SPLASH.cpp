/*********
   CTIS164 - Template Source Program
----------
STUDENT :
SECTION :
HOMEWORK:
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:
*********/
/*
(D�rtgen �izimi) gl rect2f(x, y, x, y);
*/
/*
(noktali �izgi)
//  glEnable(GL_LINE_STIPPLE);
//  glLineStipple(1, 0x00F0);
//  glDisable(GL_LINE_STIPPLE);
*/
/*
(translate trigo)
void vertex(point_t P, point_t position, double angle) {
	double xp = (P.x * cos(angle) - P.y * sin(angle)) + position.x;
	double yp = (P.x * sin(angle) + P.y * cos(angle)) + position.y;
	vertex({ 8, 25 }, p.pos, angle);
*/
/*
vekt�r hizi= mulV(ENEMY_SPEED, unitV(subV(P.pos, E.pos)));
vekt�r yeri belirlerken= addV(p.pos, pol2rec({ 30, p.angle }));
vekt�r ilerleme = subV(E.pos, E.vel);
*/
/*
 (API command to midfy ModelView(MV) matrix)

	glLoadIdentity(); mv = identity        main i 1 di�erleri 0
	glTranslatef(tx, ty, 0);  mv = mv. (main yine 1   son column = tx, ty, 1) son arg�man0: 2 dim oldu�u i�in
	glScalef(sx, sy, 1); mv = mv.(maini sx, sy, 1) di�erleri 0
	glRotatef(Q, 0, 0, 1); mv = mv.([cosq, -sinQ, 0], [sinQ  cosQ 0], [0, 0, 1])
bunlar� yap�nca glvertex2f de�i�iyo tekrar initialize etmek istiyorsan glLoadIdentity(); yi �a��r.

Ard� ard�na rotate ve translate koydun mesela sondakini, ilk �nce yapar yani mesela �nce translate sonra rotati yapar ��nk� en son olu�an matrixte �nce rotate yap�l�r ama translate yap�l�rken sonra uygulan�r.

*/
/*
Stringi de�i�tirmemek i�in fonksiyona giderken
(const char name[])
*/
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <windows.h>

#define WINDOW_WIDTH  1500 //1500
#define WINDOW_HEIGHT  750 //750
#define TIMER_PERIOD    16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer
#define D2R 0.0174532

#define MAXBALL 150

#define SMALL 0
#define MID 1
#define BIG 2
#define HUGE 3

#define SMALLRAD 8
#define MIDRAD 30
#define BIGRAD 50
#define HUGERAD 70

#define PLAYSCREEN  0
#define MENUSCREEN  1
#define LEVELSCREEN 2
#define INFOSCREEN  3
#define LOSESCREEN  4
#define WINSCREEN   5

#define STOPTIME 2000

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
bool right1 = false, left1 = false, up1 = false;
bool isDarkMenu;
int winWidth, winHeight, timems, times, timeremain, lastballsize, lvl, stop, score;
float barincrease, dist;
char screen, playercount, menuchoice;

typedef struct {
	int r, g, b;
}color_t;
typedef struct {
	float x, y;
}point_t;
typedef struct {
	point_t p;
	double angle;
	double speed;
	color_t c1;
	color_t c2;
}player_t;
typedef struct {
	color_t c;
	point_t p;
	bool active;
	float speed;
}arrow_t;
typedef struct {
	point_t p, velvector;
	float velvectorspeed;
	char type;
	bool active;
	color_t c;
}ball_t;

point_t velV_small = { 2,-4.2 };
point_t velV_mid = { 2,-6.5 };
point_t velV_big = { 2,-7.5 };
point_t velV_huge = { 2,-8.5 };

color_t rc; //random color
arrow_t a1, a2; //arrows
player_t p1, p2; //players
point_t lastpoint1, lastpoint2; //for arrow locations
ball_t ball[MAXBALL];  //balls

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

int find_Available_Index(ball_t ball[], int* lbsize)
{
	int i = 0;

	while (i < *lbsize)
	{
		if (ball[i].active == false)
			return(i);
		i++;
	}

	//increase last ball size
	(*lbsize)++;
	return(*lbsize - 1);
}

//add_Ball(HUGE, { 0,0 }, 2, false, { 0,255,0 }, ball, &lastballsize);
void add_Ball(int tp, point_t p, float dx, bool updown, color_t c, ball_t ball[], int* lbindex)
{
	//find available space to add ball
	int availableindex = find_Available_Index(ball, lbindex);

	//necessary position information
	{
		ball[availableindex].active = true;
		ball[availableindex].p = { p.x, p.y };
		ball[availableindex].c = { c.r,c.g,c.b };
		ball[availableindex].type = tp;
	}

	//velocity vector
	{
		if (ball[availableindex].type == SMALL)
		{
			if (updown == true)
				ball[availableindex].velvector.y = -velV_small.y;
			else
				ball[availableindex].velvector.y = velV_small.y;

			ball[availableindex].velvector.x = dx;
			ball[availableindex].velvectorspeed = ball[availableindex].velvector.y;
		}
		else if (ball[availableindex].type == MID)
		{
			if (updown == true)
				ball[availableindex].velvector.y = -velV_mid.y;
			else
				ball[availableindex].velvector.y = velV_mid.y;

			ball[availableindex].velvector.x = dx;

			ball[availableindex].velvectorspeed = ball[availableindex].velvector.y;
		}
		else if (ball[availableindex].type == BIG)
		{
			if (updown == true)
				ball[availableindex].velvector.y = -velV_big.y;
			else
				ball[availableindex].velvector.y = velV_big.y;

			ball[availableindex].velvector.x = dx;

			ball[availableindex].velvectorspeed = ball[availableindex].velvector.y;
		}
		else if (ball[availableindex].type == HUGE)
		{
			if (updown == true)
				ball[availableindex].velvector.y = -velV_huge.y;
			else
				ball[availableindex].velvector.y = velV_huge.y;

			ball[availableindex].velvector.x = dx;

			ball[availableindex].velvectorspeed = ball[availableindex].velvector.y;
		}
	}
}

color_t random_Color(void)
{
	return{ (rand() % ((255 + 1) - 0) + 0), (rand() % ((255 + 1) - 0) + 0), (rand() % ((255 + 1) - 0) + 0) };
}

void inittolvl(int lvll, int* lvl, ball_t ball[], int* lastballsize, player_t* p1, player_t* p2, arrow_t* a1, arrow_t* a2, float* barincrease, int* timeremain, int* timems, int* times, int* stop, char* screen, int* score, int pcount)
{
	//fix
	{
		//initialize screen
		*screen = PLAYSCREEN;

		//initialize lvl
		*lvl = lvll;

		//initialize ball to inactive, ballindex
		{
			for (*lastballsize = 0; *lastballsize < MAXBALL; (*lastballsize)++)
				ball[*lastballsize].active = false;

			*lastballsize = 0;
		}

		//initialize arrow
		{
			a1->active = false;
			a1->c.r = 255;
			a1->c.g = 0;
			a1->c.b = 0;
			a1->speed = 10;

			a2->active = false;
			a2->c.r = 255;
			a2->c.g = 0;
			a2->c.b = 0;
			a2->speed = 10;
		}

		//initialize score
		{
			*score = 0;
		}

		//initialize players
		{
			p1->c1 = { 40, 100, 10 };
			//p1->c2 = { 128, 0, 255 };
			//p1->c2 = { 64,255,128 };
			p1->c2 = { 0,255,0 };
			p1->speed = 4;

			p2->c1 = { 40, 100, 100 };
			//p2->c2 = { 255, 128, 64 };
			p2->c2 = { 255,255,0 };
			p2->speed = 4;

			//positions
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
		}
	}


	//////sil
	//lvll = 44;
	//playercount = 1;
	//pcount = playercount;

	if (lvll == 1)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 10;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 200 ,-277 };
				*timeremain = 10;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,255 }, ball, lastballsize);
		}
	}

	else if (lvll == 2)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 10;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 200 ,-277 };
				*timeremain = 10;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 3)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 10;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 10;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { 100,0 }, 0, false, { 0,255,0 }, ball, lastballsize);
			add_Ball(MID, { -100,0 }, 0, false, { 0,255,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 4)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 10;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 10;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(BIG, { 0,-200 }, 0, true, { 100,0,0 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 5)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 10;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 150 ,-277 };
				*timeremain = 15;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { 0,-200 }, 0, false, { 100,100,100 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 6)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 10;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 10;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { -5,0 }, -0.5, true, { 237,5,133 }, ball, lastballsize);
			add_Ball(MID, { 5,0 }, 0.5, true, { 133,5,236 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 7)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 8;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 10;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { -200,0 }, 0.3, false, { 0,24,0 }, ball, lastballsize);
			add_Ball(MID, { 200,0 }, -0.3, false, { 0,24,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 8)
	{
		//2 player
		{
			p1->p = { 600 ,-277 };
			p2->p = { 600 ,-277 };
			*timeremain = 6;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 600 ,-277 };
				*timeremain = 6;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -525,200 }, -0.6, true, { 255,255,255 }, ball, lastballsize);
		}
	}

	else if (lvll == 9)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 3;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 3;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 50,0 }, -1, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -50,0 }, +1, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 10)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 2;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 6;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -300,0 }, 0, true, { 255,255,0 }, ball, lastballsize);
			add_Ball(SMALL, { 300,0 }, 0, true, { 255,255,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 11)
	{
		//2 player
		{
			p1->p = { -600 ,-277 };
			p2->p = { 600 ,-277 };
			*timeremain = 7;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { -600 ,-277 };
				*timeremain = 8;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { 100,0 }, -1, false, { 0,255 ,0 }, ball, lastballsize);
			add_Ball(MID, { -100,0 }, 1, false, { 255,255,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 12)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 8;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 15;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { 600,0 }, -1, true, { 100,100,0 }, ball, lastballsize);
			add_Ball(MID, { 600,0 }, -2, true, { 200,200,0 }, ball, lastballsize);
			add_Ball(MID, { -600,0 }, 2, true, { 200,200,0 }, ball, lastballsize);
			add_Ball(MID, { -600,0 }, 1, true, { 100,100,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 13)
	{
		//2 player
		{
			p1->p = { 400,-277 };
			p2->p = { -400 ,-277 };
			*timeremain = 14;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 17;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(BIG, { -600,0 }, -2, false, { 0,155,0 }, ball, lastballsize);
			add_Ball(BIG, { 600,-0 }, 2, false, { 0,0,155 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 14)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 10;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 100 ,-277 };
				*timeremain = 13;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, -2, false, { 0,255,0 }, ball, lastballsize);
			//add_Ball(HUGE, { 0,0 }, +2, false, { 0,255,0 }, ball, lastballsize);
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,255,0 }, ball, lastballsize);
			add_Ball(BIG, { 0,0 }, 0, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(BIG, { 0,0 }, 0, false, { 255,0,0 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, -2, false, { 255,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, -2, false, { 255,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 15)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 12;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 100 ,-277 };
				*timeremain = 15;
			}
		}

		//initialize balls
		{

			add_Ball(BIG, { 0,-200 }, -2, true, { 255,0,0 }, ball, lastballsize);
			add_Ball(BIG, { 0,-200 }, +2, true, { 255,0,0 }, ball, lastballsize);
			add_Ball(MID, { 0,0 }, 0, false, { 255,255,0 }, ball, lastballsize);
			add_Ball(MID, { 0,0 }, 0, false, { 255,255,0 }, ball, lastballsize);

		}
	}

	else if (lvll == 16)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 15;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 18;
			}
		}

		//initialize balls
		{
			add_Ball(BIG, { 600,-200 }, 0, true, { 0,0,255 }, ball, lastballsize);
			add_Ball(BIG, { -600,-200 }, 0, true, { 0,0,255 }, ball, lastballsize);
			add_Ball(MID, { -20,0 }, -1, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(MID, { 20,0 }, 1, false, { 255,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 17)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 13;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 300 ,-277 };
				*timeremain = 25;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { -500,0 }, 0, false, { 255,255,0 }, ball, lastballsize);
			add_Ball(HUGE, { 500,0 }, 0, false, { 255,255,255 }, ball, lastballsize);
		}
	}

	else if (lvll == 18)
	{
		//2 player
		{
			p1->p = { 0,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 10;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 13;
			}
		}

		//initialize balls
		{
			add_Ball(SMALL, { -650,0 }, 0, true, { 255,255,0 }, ball, lastballsize);
			add_Ball(SMALL, { -650,0 }, 0.5, true, { 255,255,0 }, ball, lastballsize);
			add_Ball(SMALL, { -650,0 }, 1, true, { 255,255,0 }, ball, lastballsize);
			add_Ball(SMALL, { -650,0 }, 1.5, true, { 255,255,0 }, ball, lastballsize);
			add_Ball(SMALL, { -650,0 }, 2, true, { 255,255,0 }, ball, lastballsize);

			add_Ball(SMALL, { 650,0 }, -0, true, { 0,255,0 }, ball, lastballsize);
			add_Ball(SMALL, { 650,0 }, -0.5, true, { 0,255,0 }, ball, lastballsize);
			add_Ball(SMALL, { 650,0 }, -1, true, { 0,255,0 }, ball, lastballsize);
			add_Ball(SMALL, { 650,0 }, -1.5, true, { 0,255,0 }, ball, lastballsize);
			add_Ball(SMALL, { 650,0 }, -2, true, { 0,255,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 19)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 20;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 300 ,-277 };
				*timeremain = 30;
			}
		}

		//initialize balls
		{
			add_Ball(MID, { 0,0 }, 0, false, { 0,255,128 }, ball, lastballsize);
			add_Ball(MID, { 0,0 }, +0.5, false, { 0,255,128 }, ball, lastballsize);
			add_Ball(MID, { 0,0 }, +1, false, { 0,255,128 }, ball, lastballsize);
			add_Ball(MID, { 0,0 }, 0, false, { 0,255,128 }, ball, lastballsize);
			add_Ball(MID, { 0,0 }, -0.5, false, { 0,255,128 }, ball, lastballsize);
			add_Ball(MID, { 0,0 }, -1, false, { 0,255,128 }, ball, lastballsize);
		}
	}

	else if (lvll == 20)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 30;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 40;
			}
		}

		//initialize balls
		{
			add_Ball(BIG, { 600,100 }, -1, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(BIG, { 600,100 }, -2, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(BIG, { 600,100 }, -3, true, { 0,0,0 }, ball, lastballsize);

			add_Ball(BIG, { -600,100 }, 1, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(BIG, { -600,100 }, 2, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(BIG, { -600,100 }, 3, true, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 21)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 2;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 9;
			}
		}

		//initialize balls
		{
			add_Ball(SMALL, { -650,-125 }, 0, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -650,-100 }, 0, true, { 50,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -650,-75 }, 0, true, { 100,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -650,-50 }, 0, true, { 200,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 650,-125 }, 0, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 650,-100 }, 0, true, { 50,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 650,-75 }, 0, true, { 100,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 650,-50 }, 0, true, { 200,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 22)
	{
		//2 player
		{
			p1->p = { 400 ,-277 };
			p2->p = { -400 ,-277 };
			*timeremain = 10;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { -300 ,-277 };
				*timeremain = 15;
			}
		}

		//initialize balls
		{
			add_Ball(SMALL, { 0,0 }, 0, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, -0.5, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, -1, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, -1.5, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, -2, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, -2.5, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, -3, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, 0, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, 0.5, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, 1, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, 1.5, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, 2, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, 2.5, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, 3, false, { 255,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 23)
	{
		//2 player
		{
			p1->p = { 0,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 15;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 30;
			}
		}

		//initialize balls
		{
			add_Ball(SMALL, { -650,0 }, 0, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -600,0 }, 0, true, { 100,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -550,0 }, 0, true, { 200,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -500,0 }, 0, true, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -450,0 }, 0, true, { 0,100,0 }, ball, lastballsize);
			add_Ball(SMALL, { -400,0 }, 0, true, { 0,200,0 }, ball, lastballsize);
			add_Ball(SMALL, { -350,0 }, 0, true, { 0,255,0 }, ball, lastballsize);
			add_Ball(SMALL, { -300,0 }, 0, true, { 0,0,255 }, ball, lastballsize);
			add_Ball(SMALL, { 300,0 }, 0, true, { 0,0,255 }, ball, lastballsize);
			add_Ball(SMALL, { 350,0 }, 0, true, { 0,255,0 }, ball, lastballsize);
			add_Ball(SMALL, { 400,0 }, 0, true, { 0,200,0 }, ball, lastballsize);
			add_Ball(SMALL, { 450,0 }, 0, true, { 0,100,0 }, ball, lastballsize);
			add_Ball(SMALL, { 500,0 }, 0, true, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 550,0 }, 0, true, { 200,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 600,0 }, 0, true, { 100,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 650,0 }, 0, true, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 24)
	{
		//2 player
		{
			p1->p = { 500 ,-277 };
			p2->p = { 600 ,-277 };
			*timeremain = 10;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 300 ,-277 };
				*timeremain = 10;
			}
		}

		//initialize balls
		{
			add_Ball(SMALL, { 60 - 500,    -300 }, 2, false, { 50,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 40 - 500,    -280 }, 2, false, { 100,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 20 - 500,    -260 }, 2, false, { 150,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0 - 500,    -240 }, 2, false, { 200,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -20 - 500,   -220 }, 2, false, { 250,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -40 - 500,   -210 }, 2, false, { 250,50,0 }, ball, lastballsize);
			add_Ball(SMALL, { -60 - 500,   -200 }, 2, false, { 250,100,0 }, ball, lastballsize);
			add_Ball(SMALL, { -80 - 500,   -190 }, 2, false, { 250,150,0 }, ball, lastballsize);
			add_Ball(SMALL, { -100 - 500,  -180 }, 2, false, { 250,200,0 }, ball, lastballsize);
			add_Ball(SMALL, { -120 - 500,  -170 }, 2, false, { 250,250,0 }, ball, lastballsize);
			add_Ball(SMALL, { -140 - 500,  -160 }, 2, false, { 250,250,50 }, ball, lastballsize);
			add_Ball(SMALL, { -160 - 500,  -150 }, 2, false, { 250,250,100 }, ball, lastballsize);
			add_Ball(SMALL, { -180 - 500,  -140 }, 2, false, { 250,250,150 }, ball, lastballsize);
			add_Ball(SMALL, { -200 - 500,  -130 }, 2, false, { 250,250,200 }, ball, lastballsize);
			add_Ball(SMALL, { -220 - 500,  -120 }, 2, false, { 250,250,250 }, ball, lastballsize);

			add_Ball(SMALL, { 60,    -300 }, 2, false, { 250,250,250 }, ball, lastballsize);
			add_Ball(SMALL, { 40,    -280 }, 2, false, { 200,250,250 }, ball, lastballsize);
			add_Ball(SMALL, { 20,    -260 }, 2, false, { 150,250,250 }, ball, lastballsize);
			add_Ball(SMALL, { 0 ,    -240 }, 2, false, { 100,250,250 }, ball, lastballsize);
			add_Ball(SMALL, { -20,   -220 }, 2, false, { 50,250,250 }, ball, lastballsize);
			add_Ball(SMALL, { -40,   -210 }, 2, false, { 0,250,250 }, ball, lastballsize);
			add_Ball(SMALL, { -60,   -200 }, 2, false, { 0,250,200 }, ball, lastballsize);
			add_Ball(SMALL, { -80,   -190 }, 2, false, { 0,250,150 }, ball, lastballsize);
			add_Ball(SMALL, { -100,  -180 }, 2, false, { 0,250,100 }, ball, lastballsize);
			add_Ball(SMALL, { -120,  -170 }, 2, false, { 0,250,50 }, ball, lastballsize);
			add_Ball(SMALL, { -140,  -160 }, 2, false, { 0,250,0 }, ball, lastballsize);
			add_Ball(SMALL, { -160,  -150 }, 2, false, { 0,200,0 }, ball, lastballsize);
			add_Ball(SMALL, { -180,  -140 }, 2, false, { 0,150,0 }, ball, lastballsize);
			add_Ball(SMALL, { -200,  -130 }, 2, false, { 0,100,0 }, ball, lastballsize);
			add_Ball(SMALL, { -220,  -120 }, 2, false, { 0,50,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 25)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 30;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 100 ,-277 };
				*timeremain = 45;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { -600,-200 }, 1, true, { 0,255,0 }, ball, lastballsize);
			add_Ball(HUGE, { 600,-200 }, -1, true, { 0,255,0 }, ball, lastballsize);
			add_Ball(MID, { 0,0 }, 0, false, { 255,69,13 }, ball, lastballsize);
			add_Ball(SMALL, { -200,0 }, 0, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 200,0 }, 0, false, { 255,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 26)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 50;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 60;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { -450,0 }, 0, true, { 255,0,0 }, ball, lastballsize);
			add_Ball(BIG, { -300,0 }, 0, true, { 255,0,0 }, ball, lastballsize);
			add_Ball(MID, { -200,0 }, 0, true, { 255,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { -100,0 }, 0, true, { 255,0,0 }, ball, lastballsize);
			add_Ball(HUGE, { 450,0 }, 0, true, { 0,0,255 }, ball, lastballsize);
			add_Ball(BIG, { 300,0 }, 0, true, { 0,0,255 }, ball, lastballsize);
			add_Ball(MID, { 200,0 }, 0, true, { 0,0,255 }, ball, lastballsize);
			add_Ball(SMALL, { 100,0 }, 0, true, { 0,0,255 }, ball, lastballsize);
		}
	}

	else if (lvll == 27)
	{
		//2 player
		{
			p1->p = { 0,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 60;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 90;
			}
		}

		//initialize balls
		{
			//initialize balls
			{
				add_Ball(HUGE, { -450,0 }, 0, false, { 255,255,0 }, ball, lastballsize);
				add_Ball(BIG, { -300,0 }, 0, false, { 255,255,0 }, ball, lastballsize);
				add_Ball(MID, { -200,0 }, 0, false, { 255,255,0 }, ball, lastballsize);
				add_Ball(SMALL, { -100,0 }, 0, false, { 255,255,0 }, ball, lastballsize);
				add_Ball(HUGE, { 450,0 }, 0, false, { 0,255,255 }, ball, lastballsize);
				add_Ball(BIG, { 300,0 }, 0, false, { 0,255,255 }, ball, lastballsize);
				add_Ball(MID, { 200,0 }, 0, false, { 0,255,255 }, ball, lastballsize);
				add_Ball(SMALL, { 100,0 }, 0, false, { 0,255,255 }, ball, lastballsize);
			}
		}
	}

	else if (lvll == 28)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 20;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 40;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);

			add_Ball(MID, { -600,125 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -550,100 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -500,75 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -450,50 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -400,25 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -350,0 }, 2, false, random_Color(), ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 29)
	{
		//2 player
		{
			p1->p = { 500 ,-277 };
			p2->p = { -500 ,-277 };
			*timeremain = 40;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 500 ,-277 };
				*timeremain = 60;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { 0,100 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(HUGE, { 0,100 }, -2, false, random_Color(), ball, lastballsize);
			add_Ball(BIG, { 0,75 }, 1.5, false, random_Color(), ball, lastballsize);
			add_Ball(BIG, { 0,75 }, -1.5, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { 0,50 }, 1, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { 0,50 }, -1, false, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, 0, false, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, 0, false, random_Color(), ball, lastballsize);
		}
	}

	else if (lvll == 30)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 40;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 60;
			}
		}

		//initialize balls
		{
			add_Ball(BIG, { -200,20 }, 0, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -100,0 }, 0, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -300,0 }, 0, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -200,100 }, 0, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -200,-100 }, 0, false, random_Color(), ball, lastballsize);

			add_Ball(BIG, { 200,20 }, 0, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { 100,0 }, 0, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { 300,0 }, 0, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { 200,100 }, 0, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { 200,-100 }, 0, false, random_Color(), ball, lastballsize);
		}
	}

	else if (lvll == 31)
	{
		//2 player
		{
			p1->p = { 500 ,-277 };
			p2->p = { 500 ,-277 };
			*timeremain = 30;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 500 ,-277 };
				*timeremain = 50;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { -600,0 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(BIG, { -600,0 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -600 ,0 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { -600,0 }, 2, false, random_Color(), ball, lastballsize);

			add_Ball(HUGE, { -100,0 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(BIG, { -100,0 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -100 ,0 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { -100,0 }, 2, false, random_Color(), ball, lastballsize);
		}
	}

	else if (lvll == 32)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 00 ,-277 };
			*timeremain = 50;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 60;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { 600,0 }, -1, false, random_Color(), ball, lastballsize);
			add_Ball(HUGE, { 600,0 }, -1, false, random_Color(), ball, lastballsize);
			add_Ball(HUGE, { -600,0 }, 1, false, random_Color(), ball, lastballsize);
			add_Ball(HUGE, { -600,0 }, 1, false, random_Color(), ball, lastballsize);
		}
	}

	else if (lvll == 33)
	{
		//2 player
		{
			p1->p = { 700 ,-277 };
			p2->p = { -700 ,-277 };
			*timeremain = 30;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 700 ,-277 };
				*timeremain = 40;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(BIG, { 0,0 }, 1, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(BIG, { 0,0 }, -1, false, { 255,0,0 }, ball, lastballsize);
			add_Ball(MID, { -700,50 }, -2, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { -700,0 }, -2, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { -700,-50 }, -2, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { -700,-100 }, -2, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { 700,50 }, 2, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { 700,0 }, 2, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { 700,-50 }, 2, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(MID, { 700,-100 }, 2, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 34)
	{
		//2 player
		{
			p1->p = { -700 ,-277 };
			p2->p = { 700 ,-277 };
			*timeremain = 45;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { -700 ,-277 };
				*timeremain = 50;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { 0,0 }, -1, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(HUGE, { 0,0 }, -0.5, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(HUGE, { 0,0 }, 0, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(HUGE, { 0,0 }, 0.5, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(HUGE, { 0,0 }, 1, true, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 35)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 50;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 60;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { -650,0 }, 0.1, false, random_Color(), ball, lastballsize);
			add_Ball(HUGE, { 650,0 }, -0.1, false, random_Color(), ball, lastballsize);

			add_Ball(BIG, { -700,0 }, 0.5, false, random_Color(), ball, lastballsize);
			add_Ball(BIG, { -600,0 }, 0.5, false, random_Color(), ball, lastballsize);
			add_Ball(BIG, { 700,0 }, -0.5, false, random_Color(), ball, lastballsize);
			add_Ball(BIG, { 600,0 }, -0.5, false, random_Color(), ball, lastballsize);

			add_Ball(MID, { -700,0 }, 1, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -650,0 }, 1, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { -600,0 }, 1, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { 700,0 }, -1, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { 650,0 }, -1, false, random_Color(), ball, lastballsize);
			add_Ball(MID, { 600,0 }, -1, false, random_Color(), ball, lastballsize);

			add_Ball(SMALL, { -700,0 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { -650,0 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { -600,0 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { -500,0 }, 2, false, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { -400,0 }, 2, false, random_Color(), ball, lastballsize);

			add_Ball(SMALL, { 700,0 }, -2, false, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { 650,0 }, -2, false, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { 600,0 }, -2, false, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { 500,0 }, -2, false, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { 400,0 }, -2, false, random_Color(), ball, lastballsize);
		}
	}

	else if (lvll == 36)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 100;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 120;
			}
		}

		//initialize balls
		{
			for (float i = -600; i <= -200; i += 150)
				add_Ball(HUGE, { i,0 }, 0, false, random_Color(), ball, lastballsize);

			for (float i = 200; i <= 600; i += 150)
				add_Ball(HUGE, { i,0 }, 0, false, random_Color(), ball, lastballsize);

			for (float i = -650; i <= -250; i += 150)
				add_Ball(HUGE, { i,200 }, 0, false, random_Color(), ball, lastballsize);

			for (float i = 250; i <= 600; i += 150)
				add_Ball(HUGE, { i,200 }, 0, false, random_Color(), ball, lastballsize);

			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 37)
	{
		//2 player
		{
			p1->p = { 700 ,-277 };
			p2->p = { 700 ,-277 };
			*timeremain = 50;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 700 ,-277 };
				*timeremain = 50;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			for (int i = 1; i <= 10; i++)
				add_Ball(BIG, { -700,-50 }, i, true, random_Color(), ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 38)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 50;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 50;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { 0,0 }, 10, false, { 0,255,0 }, ball, lastballsize);
			add_Ball(HUGE, { 0,0 }, -10, false, { 0,255,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 30, false, { 0,0,0 }, ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, 0, true, { 255,0,255 }, ball, lastballsize);
			add_Ball(SMALL, { -30,0 }, -1, true, { 255,0,255 }, ball, lastballsize);
			add_Ball(SMALL, { 30,0 }, 1, true, { 255,0,255 }, ball, lastballsize);
		}
	}

	else if (lvll == 39)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 50;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 60;
			}
		}

		//initialize balls
		{
			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			for (int i = 0; i <= 11; i++)
				add_Ball(BIG, { float(-600 + i * 100),0 }, 0, true, { i * 10,i * 10,i * 10 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 40)
	{
		//2 player
		{
			p1->p = { 700 ,-277 };
			p2->p = { -700 ,-277 };
			*timeremain = 70;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { -700 ,-277 };
				*timeremain = 80;
			}
		}

		//initialize balls
		{
			for (int i = 1; i <= 6; i++)
				add_Ball(BIG, { float(-700 + i * 100),float(250 - i * 30) }, 0, false, random_Color(), ball, lastballsize);

			for (int i = 1; i <= 6; i++)
				add_Ball(BIG, { float(700 - i * 100),float(250 - i * 30) }, 0, false, random_Color(), ball, lastballsize);

			add_Ball(SMALL, { 0,0 }, -1, true, random_Color(), ball, lastballsize);
			add_Ball(SMALL, { 0,0 }, 1, true, random_Color(), ball, lastballsize);

			add_Ball(MID, { 0,-50 }, -2, true, random_Color(), ball, lastballsize);
			add_Ball(MID, { 0,-50 }, 2, true, random_Color(), ball, lastballsize);

			add_Ball(HUGE, { 0,-100 }, -3, true, random_Color(), ball, lastballsize);
			add_Ball(HUGE, { 0,-100 }, 3, true, random_Color(), ball, lastballsize);

		}
	}

	else if (lvll == 41)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 50;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 60;
			}
		}

		//initialize balls
		{
			for (int i = 50; i <= 720; i += 50)
				add_Ball(MID, { float(i),float(cos(i * D2R) * 100) }, 0, true, { 0,i / 5, 10 }, ball, lastballsize);
			for (int i = -50; i >= -720; i -= 50)
				add_Ball(MID, { float(i),float(cos(i * D2R) * 100) }, 0, true, { 0,i / 5,10 }, ball, lastballsize);




			//add_Ball(HUGE, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 42)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 90;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 85;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { -650,0 }, 1, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(HUGE, { 650,0 }, -1, true, { 0,0,0 }, ball, lastballsize);

			for (int i = -700; i <= -100; i += 50)
				add_Ball(MID, { float(i),float(cos(i * D2R) * 200) }, 0.5, false, { 100,0,50 }, ball, lastballsize);
			for (int i = 100; i <= 700; i += 50)
				add_Ball(MID, { float(i),float(cos(i * D2R) * 200) }, -0.5, false, { 100,0,50 }, ball, lastballsize);


			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 43)
	{
		//2 player
		{
			p1->p = { -300 ,-277 };
			p2->p = { 300 ,-277 };
			*timeremain = 50;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { -300 ,-277 };
				*timeremain = 60;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { 300,-100 }, 0, true, { 0,255,0 }, ball, lastballsize);
			add_Ball(HUGE, { 300,-100 }, 0, true, { 0,255,0 }, ball, lastballsize);
			add_Ball(HUGE, { 300,-100 }, 0, true, { 0,255,0 }, ball, lastballsize);

			add_Ball(HUGE, { -300,-100 }, 0, true, { 0,0,255 }, ball, lastballsize);
			add_Ball(HUGE, { -300,-100 }, 0, true, { 0,0,255 }, ball, lastballsize);
			add_Ball(HUGE, { -300,-100 }, 0, true, { 0,0,255 }, ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 44)
	{
		//2 player
		{
			p1->p = { -650 ,-277 };
			p2->p = { -650 ,-277 };
			*timeremain = 50;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { -650 ,-277 };
				*timeremain = 60;
			}
		}

		//initialize balls
		{
			for (int i = 0; i <= 7; i++)
				add_Ball(HUGE, { -650,-100 }, i, true, random_Color(), ball, lastballsize);
			//add_Ball(BIG, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(MID, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
			//add_Ball(SMALL, { 0,0 }, 0, false, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 45)
	{
		//2 player
		{
			p1->p = { 300 ,-277 };
			p2->p = { -300 ,-277 };
			*timeremain = 20;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 60;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { -600, 150 }, 0, false, { 0,100,150 }, ball, lastballsize);
			add_Ball(BIG, { -600,  100 }, 0, false, { 0,100,200 }, ball, lastballsize);
			add_Ball(MID, { -600,  50 }, 0, false, { 0,150,250 }, ball, lastballsize);
			add_Ball(SMALL, { -600,0 }, 0, false, { 0,200,250 }, ball, lastballsize);

			add_Ball(HUGE, { 600 ,150 }, 0, false, { 0,100,150 }, ball, lastballsize);
			add_Ball(BIG, { 600  ,100 }, 0, false, { 0,100,200 }, ball, lastballsize);
			add_Ball(MID, { 600  ,50 }, 0, false, { 0,150,250 }, ball, lastballsize);
			add_Ball(SMALL, { 600,0 }, 0, false, { 0,200,250 }, ball, lastballsize);
		}
	}

	else if (lvll == 46)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 60;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 60;
			}
		}

		//initialize balls
		{
			add_Ball(HUGE, { -600,-100 }, 0, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(HUGE, { -600,-100 }, 0, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(HUGE, { -600,-100 }, 0, true, { 0,0,0 }, ball, lastballsize);

			add_Ball(HUGE, { 600,-100 }, 0, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(HUGE, { 600,-100 }, 0, true, { 0,0,0 }, ball, lastballsize);
			add_Ball(HUGE, { 600,-100 }, 0, true, { 0,0,0 }, ball, lastballsize);
		}
	}

	else if (lvll == 47)
	{
		//2 player
		{
			p1->p = { 650 ,-277 };
			p2->p = { -650 ,-277 };
			*timeremain = 40;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 650 ,-277 };
				*timeremain = 50;
			}
		}

		//initialize balls
		{
			for (int i = 0; i < 10; i++)
				add_Ball(SMALL, { float(0 + -i * 50),float(0) + 50 }, 0, false, { random_Color() }, ball, lastballsize);
			for (int i = 0; i < 10; i++)
				add_Ball(SMALL, { float(0 + i * 50),float(0) + 50 }, 0, false, { random_Color() }, ball, lastballsize);

			for (int i = 0; i < 10; i++)
				add_Ball(SMALL, { float(0 - i * 50),float(0) + 50 - i * 10 }, 0, false, { random_Color() }, ball, lastballsize);
			for (int i = 0; i < 10; i++)
				add_Ball(SMALL, { float(0 + i * 50),float(0) + 50 + i * 10 }, 0, false, { random_Color() }, ball, lastballsize);

		}
	}

	else if (lvll == 48)
	{
		//2 player
		{
			p1->p = { -700 ,-277 };
			p2->p = { -700 ,-277 };
			*timeremain = 30;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { -700 ,-277 };
				*timeremain = 30;
			}
		}

		//initialize balls
		{
			for (int i = 0; i <= 360; i += 5)
				add_Ball(SMALL, { float(250 * cos(i * D2R)) - 450,float(200 * sin(i * D2R)) }, 1, false, { 0,i / 2,i / 2 }, ball, lastballsize);
		}
	}

	else if (lvll == 49)
	{
		//2 player
		{
			p1->p = { 0 ,-277 };
			p2->p = { 0 ,-277 };
			*timeremain = 70;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { 0 ,-277 };
				*timeremain = 90;
			}
		}

		//initialize balls
		{
			for (int i = 90; i <= 270; i += 10)
				add_Ball(SMALL, { float(100 * cos(i * D2R)),float(100 * sin(i * D2R)) + 50 }, -0.5, false, { 255,0,0 }, ball, lastballsize);
			for (int i = 270; i <= 450; i += 10)
				add_Ball(SMALL, { float(100 * cos(i * D2R)),float(100 * sin(i * D2R)) + 50 }, 0.5, false, { 255,0,0 }, ball, lastballsize);
			for (int i = 90; i <= 270; i += 45)
				add_Ball(MID, { float(100 * cos(i * D2R)) - 50,float(100 * sin(i * D2R)) + 90 }, -0.5, false, { 0,255,0 }, ball, lastballsize);
			for (int i = 270; i <= 450; i += 45)
				add_Ball(MID, { float(100 * cos(i * D2R)) + 50,float(100 * sin(i * D2R)) + 90 }, 0.5, false, { 0,255,0 }, ball, lastballsize);
			for (int i = 90; i <= 270; i += 90)
				add_Ball(BIG, { float(150 * cos(i * D2R)) - 50,float(150 * sin(i * D2R)) + 150 }, -0.5, false, { 0,0,255 }, ball, lastballsize);
			for (int i = 270; i <= 450; i += 90)
				add_Ball(BIG, { float(150 * cos(i * D2R)) + 50,float(150 * sin(i * D2R)) + 150 }, 0.5, false, { 0,0,255 }, ball, lastballsize);
		}
	}

	else if (lvll == 50)
	{
		//2 player
		{
			p1->p = { -700 ,-277 };
			p2->p = { -700 ,-277 };
			*timeremain = 70;
		}

		//1 player
		{
			if (pcount == 1)
			{
				p1->p = { -700 ,-277 };
				*timeremain = 70;
			}
		}

		//initialize balls
		{
			//A
			{
				for (int i = -704; i <= -465; i += 17)
					add_Ball(SMALL, { float(i),float(i) + 670 - 20 }, 1, true, { random_Color() }, ball, lastballsize);

				for (int i = -226; i >= -460; i -= 17)
					add_Ball(SMALL, { float(i), -float(i) - 260 - 20 }, 1, true, { random_Color() }, ball, lastballsize);
			}

			//O
			{
				for (float i = 0; i <= 360; i += 10)
					add_Ball(SMALL, { float(150 * cos(i * D2R)),float(150 * sin(i * D2R)) + 70 }, 1, true, { random_Color() }, ball, lastballsize);
			}

			//E
			{
				for (float i = 205; i >= -85; i -= 20)
					add_Ball(SMALL, { 250,i }, 1, true, { random_Color() }, ball, lastballsize);

				for (float i = 270; i <= 420; i += 20)
					add_Ball(SMALL, { i,205 }, 1, true, { random_Color() }, ball, lastballsize);

				for (float i = 270; i <= 420; i += 20)
					add_Ball(SMALL, { i, 65 }, 1, true, { random_Color() }, ball, lastballsize);

				for (float i = 270; i <= 420; i += 20)
					add_Ball(SMALL, { i, -75 }, 1, true, { random_Color() }, ball, lastballsize);

			}

		}
	}

	//fix
	{
		//initialize time
		{
			*timems = 0;
			*times = 0;
			*stop = 0;
			*barincrease = 1000. / *timeremain;
		}
	}
}

int calculate_Last_Size(ball_t b[])
{
	int i = MAXBALL - 1;
	while (i >= 0)
	{
		if (b[i].active == true)
			return(i + 1);
		i--;
	}
	return(0);
}

void check_ball_hits_player(ball_t b[], player_t p1, player_t p2, int* lbsize, char pcount)
{
	//calculate last ball size
	*lbsize = calculate_Last_Size(ball);

	float distance1;
	float distance2;

	for (int i = 0; i < *lbsize; i++)
	{
		if (b[i].active == true)
		{
			//calculate distance for p1
			{
				if (b[i].type == SMALL)
				{
					distance1 = sqrt((b[i].p.x - p1.p.x) * (b[i].p.x - p1.p.x) + (b[i].p.y - p1.p.y) * (b[i].p.y - p1.p.y)) - SMALLRAD - 7;
					if (pcount == 2)
						distance2 = sqrt((b[i].p.x - p2.p.x) * (b[i].p.x - p2.p.x) + (b[i].p.y - p2.p.y) * (b[i].p.y - p2.p.y)) - SMALLRAD - 7;
				}
				else if (b[i].type == MID)
				{
					distance1 = sqrt((b[i].p.x - p1.p.x) * (b[i].p.x - p1.p.x) + (b[i].p.y - p1.p.y) * (b[i].p.y - p1.p.y)) - MIDRAD - 7;
					if (pcount == 2)
						distance2 = sqrt((b[i].p.x - p2.p.x) * (b[i].p.x - p2.p.x) + (b[i].p.y - p2.p.y) * (b[i].p.y - p2.p.y)) - MIDRAD - 7;
				}
				else if (b[i].type == BIG)
				{
					distance1 = sqrt((b[i].p.x - p1.p.x) * (b[i].p.x - p1.p.x) + (b[i].p.y - p1.p.y) * (b[i].p.y - p1.p.y)) - BIGRAD - 7;
					if (pcount == 2)
						distance2 = sqrt((b[i].p.x - p2.p.x) * (b[i].p.x - p2.p.x) + (b[i].p.y - p2.p.y) * (b[i].p.y - p2.p.y)) - BIGRAD - 7;
				}
				else if (b[i].type == HUGE)
				{
					distance1 = sqrt((b[i].p.x - p1.p.x) * (b[i].p.x - p1.p.x) + (b[i].p.y - p1.p.y) * (b[i].p.y - p1.p.y)) - HUGERAD - 7;
					if (pcount == 2)
						distance2 = sqrt((b[i].p.x - p2.p.x) * (b[i].p.x - p2.p.x) + (b[i].p.y - p2.p.y) * (b[i].p.y - p2.p.y)) - HUGERAD - 7;
				}
			}

			if (distance1 <= 0 || pcount == 2 && distance2 <= 0)
				stop = STOPTIME;
		}
	}
}

void hit_split_balls(ball_t* b, int* lbsize, arrow_t* a, int* score)
{
	//remove arrow
	a->active = false;

	//remove ball
	b->active = false;

	//give new colors
	rc = random_Color();

	//add 2 new ball if it is not small one
	if (b->type == MID)
	{
		add_Ball(SMALL, b->p, 2, true, { rc.r,rc.g,rc.b }, ball, lbsize);
		add_Ball(SMALL, b->p, -2, true, { rc.r,rc.g,rc.b }, ball, lbsize);
	}
	else if (b->type == BIG)
	{
		add_Ball(MID, b->p, 2, true, { rc.r,rc.g,rc.b }, ball, lbsize);
		add_Ball(MID, b->p, -2, true, { rc.r,rc.g,rc.b }, ball, lbsize);
	}
	else if (b->type == HUGE)
	{
		add_Ball(BIG, b->p, 2, true, { rc.r,rc.g,rc.b }, ball, lbsize);
		add_Ball(BIG, b->p, -2, true, { rc.r,rc.g,rc.b }, ball, lbsize);
	}

	//increase score
	(*score)++;
}

void arrow_Hit(arrow_t* a1, arrow_t* a2, ball_t b[], int* lbsize, char pcount)
{
	if (a1->active == true)
	{
		//calculate last ball index 
		*lbsize = calculate_Last_Size(b);

		for (int i = 0; i < *lbsize && a1->active == true; i++)
			if (b[i].active == true)
			{
				for (int j = -320; j <= a1->p.y && b[i].active == true && a1->active == true; j++)
				{
					//calculate distance
					{
						if (b[i].type == SMALL)
						{
							dist = sqrt((b[i].p.x - a1->p.x) * (b[i].p.x - a1->p.x) + (b[i].p.y - j) * (b[i].p.y - j)) - SMALLRAD;
						}
						else if (b[i].type == MID)
						{
							dist = sqrt((b[i].p.x - a1->p.x) * (b[i].p.x - a1->p.x) + (b[i].p.y - j) * (b[i].p.y - j)) - MIDRAD;
						}
						else if (b[i].type == BIG)
						{
							dist = sqrt((b[i].p.x - a1->p.x) * (b[i].p.x - a1->p.x) + (b[i].p.y - j) * (b[i].p.y - j)) - BIGRAD;
						}
						else if (b[i].type == HUGE)
						{
							dist = sqrt((b[i].p.x - a1->p.x) * (b[i].p.x - a1->p.x) + (b[i].p.y - j) * (b[i].p.y - j)) - HUGERAD;
						}
					}

					//hit
					if (dist <= 0)
					{
						hit_split_balls(&b[i], lbsize, a1, &score);
						*lbsize = calculate_Last_Size(b);
						//son indexi g�stermek i�in 
						//printf("%d\n", *lbsize);
					}
				}
			}
	}

	if (pcount == 2 && a2->active == true)
	{
		//calculate last ball index 
		*lbsize = calculate_Last_Size(b);

		for (int i = 0; i < *lbsize && a2->active == true; i++)
			if (b[i].active == true)
			{
				for (int j = -320; j <= a2->p.y && b[i].active == true && a2->active == true; j++)
				{
					//calculate distance
					{
						if (b[i].type == SMALL)
						{
							dist = sqrt((b[i].p.x - a2->p.x) * (b[i].p.x - a2->p.x) + (b[i].p.y - j) * (b[i].p.y - j)) - SMALLRAD;
						}
						else if (b[i].type == MID)
						{
							dist = sqrt((b[i].p.x - a2->p.x) * (b[i].p.x - a2->p.x) + (b[i].p.y - j) * (b[i].p.y - j)) - MIDRAD;
						}
						else if (b[i].type == BIG)
						{
							dist = sqrt((b[i].p.x - a2->p.x) * (b[i].p.x - a2->p.x) + (b[i].p.y - j) * (b[i].p.y - j)) - BIGRAD;
						}
						else if (b[i].type == HUGE)
						{
							dist = sqrt((b[i].p.x - a2->p.x) * (b[i].p.x - a2->p.x) + (b[i].p.y - j) * (b[i].p.y - j)) - HUGERAD;
						}
					}

					//hit
					if (dist <= 0)
					{
						hit_split_balls(&b[i], lbsize, a2, &score);
						*lbsize = calculate_Last_Size(b);
						//son indexi g�stermek i�in printf("%d\n", *lbsize);
					}
				}
			}
	}
}

void reflect_ball(ball_t* b, int* lbsize, int* score)
{
	//left reflection
	{
		if (b->type == SMALL && b->p.x - SMALLRAD <= -734)
		{
			//prevent to go below the ground
			{
				if (b->type == SMALL && b->p.x - SMALLRAD <= -734)
					b->p.x = -734 + SMALLRAD;
			}

			//velocity vekt�r�n� ayarla
			b->velvector.x = -b->velvector.x;
		}
		else if (b->type == MID && b->p.x - MIDRAD <= -734)
		{
			//prevent to go below the ground
			{
				if (b->type == MID && b->p.x - MIDRAD <= -734)
					b->p.x = -734 + MIDRAD;
			}

			//velocity vekt�r�n� ayarla
			b->velvector.x = -b->velvector.x;
		}
		else if (b->type == BIG && b->p.x - BIGRAD <= -734)
		{
			//prevent to go below the ground
			{
				if (b->type == BIG && b->p.x - BIGRAD <= -734)
					b->p.x = -734 + BIGRAD;
			}

			//velocity vekt�r�n� ayarla
			b->velvector.x = -b->velvector.x;
		}
		else if (b->type == HUGE && b->p.x - HUGERAD <= -734)
		{
			//prevent to go below the ground
			{
				if (b->type == HUGE && b->p.x - HUGERAD <= -734)
					b->p.x = -734 + HUGERAD;
			}

			//velocity vekt�r�n� ayarla
			b->velvector.x = -b->velvector.x;
		}
	}

	//right reflection
	{
		if (b->type == SMALL && b->p.x + SMALLRAD >= 734)
		{
			//prevent to go below the ground
			{
				if (b->type == SMALL && b->p.x + SMALLRAD >= 734)
					b->p.x = 734 - SMALLRAD;
			}

			//velocity vekt�r�n� ayarla
			b->velvector.x = -b->velvector.x;
		}
		else if (b->type == MID && b->p.x + MIDRAD >= 734)
		{
			//prevent to go below the ground
			{
				if (b->type == MID && b->p.x + MIDRAD >= 734)
					b->p.x = 734 - MIDRAD;
			}

			//velocity vekt�r�n� ayarla
			b->velvector.x = -b->velvector.x;
		}
		else if (b->type == BIG && b->p.x + BIGRAD >= 734)
		{
			//prevent to go below the ground
			{
				if (b->type == BIG && b->p.x + BIGRAD >= 734)
					b->p.x = 734 - BIGRAD;
			}

			//velocity vekt�r�n� ayarla
			b->velvector.x = -b->velvector.x;
		}
		else if (b->type == HUGE && b->p.x + HUGERAD >= 734)
		{
			//prevent to go below the ground
			{
				if (b->type == BIG && b->p.x + HUGERAD >= 734)
					b->p.x = 734 - HUGERAD;
			}

			//velocity vekt�r�n� ayarla
			b->velvector.x = -b->velvector.x;
		}
	}

	//Bottom reflect with velocity vector
	{
		if (b->type == SMALL && b->p.y - SMALLRAD <= -316)
		{
			//prevent to go below the ground
			{
				if (b->type == SMALL && b->p.y - SMALLRAD <= -316)
					b->p.y = -316 + SMALLRAD;
			}

			//velocity vekt�r�n� ayarla
			if (b->velvectorspeed > 0)
				b->velvector.y = b->velvectorspeed;
			else
				b->velvector.y = -b->velvectorspeed;
		}
		else if (b->type == MID && b->p.y - MIDRAD <= -316)
		{
			//prevent to go below the ground
			{
				if (b->type == MID && b->p.y - MIDRAD <= -316)
					b->p.y = -316 + MIDRAD;
			}

			//velocity vekt�r�n� ayarla
			if (b->velvectorspeed > 0)
				b->velvector.y = b->velvectorspeed;
			else
				b->velvector.y = -b->velvectorspeed;
		}
		else if (b->type == BIG && b->p.y - BIGRAD <= -316)
		{
			//prevent to go below the ground
			{
				if (b->type == BIG && b->p.y - BIGRAD <= -316)
					b->p.y = -316 + BIGRAD;
			}

			//velocity vekt�r�n� ayarla
			if (b->velvectorspeed > 0)
				b->velvector.y = b->velvectorspeed;
			else
				b->velvector.y = -b->velvectorspeed;
		}
		else if (b->type == HUGE && b->p.y - HUGERAD <= -316)
		{
			//prevent to go below the ground
			{
				if (b->type == HUGE && b->p.y - HUGERAD <= -316)
					b->p.y = -316 + HUGERAD;
			}

			//velocity vekt�r�n� ayarla
			if (b->velvectorspeed > 0)
				b->velvector.y = b->velvectorspeed;
			else
				b->velvector.y = -b->velvectorspeed;
		}
	}

	//top reflection
	{
		if (b->type == SMALL && b->p.y + SMALLRAD >= 345)
		{
			//prevent to go beyond the ground
			{
				if (b->type == SMALL && b->p.y + SMALLRAD >= 345)
					b->p.y = 345 - SMALLRAD;
			}

			//disable ball
			b->active = false;

			//increase score
			(*score)++;

			//velocity vekt�r�n� ayarla
			b->velvector.y = -b->velvectorspeed;
		}
		else if (b->type == MID && b->p.y + MIDRAD >= 345)
		{
			//prevent to go beyond the ground
			{
				if (b->type == MID && b->p.y + MIDRAD >= 345)
					b->p.y = 345 - MIDRAD;
			}

			//increase score
			(*score)++;

			//split 2 ball
			{
				//remove ball
				b->active = false;

				//give new colors
				rc = random_Color();

				//add 2 new ball if it is not small one
				{
					add_Ball(SMALL, b->p, 2, false, { rc.r,rc.g,rc.b }, ball, lbsize);
					add_Ball(SMALL, b->p, -2, false, { rc.r,rc.g,rc.b }, ball, lbsize);
					*lbsize = calculate_Last_Size(ball);
				}
			}

			//velocity vekt�r�n� ayarla
			//b->velvector.y = -b->velvectorspeed;
		}
		else if (b->type == BIG && b->p.y + BIGRAD >= 345)
		{
			//prevent to go beyond the spikes
			{
				if (b->type == BIG && b->p.y + BIGRAD >= 345)
					b->p.y = 345 - BIGRAD;
			}

			//increase score
			(*score)++;

			//velocity vekt�r�n� ayarla
			//b->velvector.y = -b->velvectorspeed;

			//split 2 ball
			{
				//remove ball
				b->active = false;

				//give new colors
				rc = random_Color();

				//add 2 new ball if it is not small one
				{
					add_Ball(MID, b->p, 2, false, { rc.r,rc.g,rc.b }, ball, lbsize);
					add_Ball(MID, b->p, -2, false, { rc.r,rc.g,rc.b }, ball, lbsize);
				}
				*lbsize = calculate_Last_Size(ball);
			}
		}
		else if (b->type == HUGE && b->p.y + HUGERAD >= 345)
		{
			//prevent to go beyond the spikes
			{
				if (b->type == HUGE && b->p.y + HUGERAD >= 345)
					b->p.y = 345 - HUGERAD;
			}

			//increase score
			(*score)++;

			//velocity vekt�r�n� ayarla
			//b->velvector.y = -b->velvectorspeed;

			//split 2 ball
			{
				//remove ball
				b->active = false;

				//give new colors
				rc = random_Color();

				//add 2 new ball if it is not small one
				{
					add_Ball(BIG, b->p, 2, false, { rc.r,rc.g,rc.b }, ball, lbsize);
					add_Ball(BIG, b->p, -2, false, { rc.r,rc.g,rc.b }, ball, lbsize);
					*lbsize = calculate_Last_Size(ball);
				}
			}
		}
	}
}

void move_ball(ball_t b[], int* lbindex, int* score)
{
	for (int i = 0; i < MAXBALL; i++)
	{
		if (b[i].active == true)
		{
			b[i].p.x += b[i].velvector.x;
			b[i].p.y += b[i].velvector.y;

			b[i].velvector.y -= 0.1;

			//check the reflection of the ball
			reflect_ball(&b[i], lbindex, score);
		}
	}
}

void move_player(player_t* p1, player_t* p2, char pcount)
{
	//player 1
	{
		if (p1->p.x >= -700 && left == true)
		{
			p1->p.x -= p1->speed;
			p1->angle += 10;
		}

		if (p1->p.x <= +700 && right == true)
		{
			p1->p.x += p1->speed;
			p1->angle -= 10;
		}
	}

	//player 2
	if (pcount == 2)
	{
		if (p2->p.x >= -700 && left1 == true)
		{
			p2->p.x -= p2->speed;
			p2->angle += 10;
		}

		if (p2->p.x <= +700 && right1 == true)
		{
			p2->p.x += p2->speed;
			p2->angle -= 10;
		}
	}
}

void move_arrow(arrow_t* a)
{
	if (a->active == true)
	{
		//check hitpoint
		if (a->p.y >= 352)
			a->active = false;

		a->p.y += a->speed;
	}
}

void vertex(point_t P, point_t position, double angle) {
	double xp = (P.x * cos(angle * D2R) - P.y * sin(angle * D2R)) + position.x;
	double yp = (P.x * sin(angle * D2R) + P.y * cos(angle * D2R)) + position.y;

	glVertex2f(xp, yp);
}

void dispbackrec()
{
	glBegin(GL_QUADS);
	glColor3ub(213, 213, 250);
	glVertex2f(-750, -375);
	glVertex2f(750, -375);

	glColor3ub(130, 130, 230);
	glVertex2f(750, 375);
	glVertex2f(-750, 375);

	glEnd();
}

void disp_frames()
{
	glColor3f(0.3, 0.2, 0.7);

	//boxes of frames
	{
		//top
		glRectf(-800, 500, 800, 360);

		//left
		glRectf(-800, -375, -735, 360);

		//right
		glRectf(800, -375, 735, 500);

		//bottom
		glRectf(-800, -500, 800, -320);
	}

	glLineWidth(6);
	glColor3f(0, 0, 0);

	//frames
	{

		glBegin(GL_LINES);

		//left					//6 3  //�izginin s�n�r� -735 de
		glVertex2f(-738, 360);
		glVertex2f(-738, -320);

		//right					//6 3  //�izginin s�n�r� -735 de
		glVertex2f(738, 360);
		glVertex2f(738, -320);

		//top
		glVertex2f(-741, 360);
		glVertex2f(741, 360);

		//bottom
		glVertex2f(-741, -320);
		glVertex2f(741, -320);

		glEnd();
	}

	//display spikes
	{
		//spikes
		{
			glBegin(GL_TRIANGLES);
			for (int i = -730; i <= 720; i += 28)
			{
				glColor3f(0.5, 0.3, 1);
				glVertex2f(i, 357);
				glVertex2f(i + 30, 357);

				glColor3f(1, 0, 0);
				glVertex2f(i + 15, 345);
			}
			glEnd();
		}

		//frames
		{
			glColor3f(0, 0, 0);
			glLineWidth(2);
			for (int i = -730; i <= 720; i += 28)
			{
				glBegin(GL_LINE_LOOP);
				glVertex2f(i, 357);
				glVertex2f(i + 30, 357);
				glVertex2f(i + 15, 345);
				glEnd();
			}
		}
	}
}

void disp_info(int score, int  timeremain, int lvl, int times)
{
	glColor3f(1, 0, 1);
	glLineWidth(4);

	//time bar
	{
		glBegin(GL_LINE_LOOP);

		glVertex2f(-500, -330);
		glVertex2f(500, -330);
		glVertex2f(500, -345);
		glVertex2f(-500, -345);

		glEnd();
	}

	//time line
	{
		if (double(times) * barincrease + double(timems) / 1000 * barincrease - 500 <= 500)
		{

			glColor3f(0, 1, 0);
			glLineWidth(9);
			glBegin(GL_POLYGON);

			glVertex2f(times * barincrease - 500 + double(timems) / 1000 * barincrease, -345);
			glVertex2f(times * barincrease - 500 + double(timems) / 1000 * barincrease, -330);
			glVertex2f(500, -330);
			glVertex2f(500, -345);

			glEnd();
		}
	}

	//time information
	{
		glLineWidth(1.5);
		if (double(times) * barincrease + double(timems) / 1000 * barincrease - 500 <= 500)
		{
			if ((1000 - timems) / 100 != 10)
				vprint2(515, -346, 0.2, "%02d:%.01d", timeremain - 1 - times, (1000 - timems) / 100);
			else
				vprint2(515, -346, 0.2, "%02d:9", timeremain - 1 - times);
		}
		else
			vprint2(515, -346, 0.2, "00:0");
	}

	//level information
	{
		//box
		{
			//box
			{
				glBegin(GL_QUADS);

				glColor3f(1, 1, 0);
				glVertex2f(-100, -353);
				glVertex2f(100, -353);

				glColor3f(1, 0, 0);
				glVertex2f(100, -395);
				glVertex2f(-100, -395);

				glEnd();
			}

			//frame
			{
				glLineWidth(3);
				glColor3f(0, 0, 0);
				glBegin(GL_LINE_LOOP);

				glVertex2f(-100, -353);
				glVertex2f(-100, -395);
				glVertex2f(100, -395);
				glVertex2f(100, -353);

				glEnd();
			}
		}

		//message
		{
			glLineWidth(3);
			glColor3f(0, 1, 1);
			vprint2(-83, -387, 0.3, "Level: %d", lvl);
			glColor3f(0, 0, 0);
			vprint2(-85, -389, 0.3, "Level: %d", lvl);
		}
	}

	//score information
	{
		//box
		{
			//box
			{
				glBegin(GL_QUADS);

				glColor3f(0, 1, 1);
				glVertex2f(-385, -353);
				glVertex2f(-150, -353);

				glColor3f(0, 0, 1);
				glVertex2f(-150, -395);
				glVertex2f(-385, -395);

				glEnd();
			}

			//frame
			{
				glLineWidth(3);
				glColor3f(0, 0, 0);
				glBegin(GL_LINE_LOOP);

				glVertex2f(-385, -353);
				glVertex2f(-385, -395);
				glVertex2f(-150, -395);
				glVertex2f(-150, -353);

				glEnd();
			}
		}

		//message
		{
			glLineWidth(3);
			glColor3f(1, 1, 0);
			vprint2(-358, -387, 0.3, "Score: %d", score);
			glColor3f(0, 0, 0);
			vprint2(-360, -389, 0.3, "Score: %d", score);
		}
	}

	//menu information
	{
		//box
		{
			//box
			{
				glBegin(GL_QUADS);

				glColor3ub(241, 187, 233);
				if (isDarkMenu)
					glColor3ub(219, 60, 135);
				glVertex2f(385, -353);
				glVertex2f(150, -353);

				glColor3ub(219, 60, 135);
				if (isDarkMenu)
					glColor3ub(59, 14, 77);
				glVertex2f(150, -395);
				glVertex2f(385, -395);

				glEnd();
			}

			//frame
			{
				glLineWidth(3);
				glColor3f(0, 0, 0);
				glBegin(GL_LINE_LOOP);

				glVertex2f(385, -353);
				glVertex2f(385, -395);
				glVertex2f(150, -395);
				glVertex2f(150, -353);

				glEnd();
			}
		}

		//message
		{
			glLineWidth(3);
			glColor3f(0, 1, 0);
			if (isDarkMenu)
				glColor3f(0, 0, 0);
			vprint2(222, -387, 0.3, "Menu");
			glColor3f(0, 0, 0);
			if (isDarkMenu)
				glColor3f(1, 1, 1);
			vprint2(220, -389, 0.3, "Menu");
		}
	}
}

void disp_player(player_t p)
{
	//pipe
	{
		glColor3ub(p.c2.r, p.c2.g, p.c2.b);


		glRectf(p.p.x - 7, p.p.y + 35, p.p.x + 7, p.p.y + 15);

		//frame
		{
			glLineWidth(2);
			glColor3f(0, 0, 0);
			glBegin(GL_LINE_STRIP);
			glVertex2f(p.p.x - 7, p.p.y + 15);
			glVertex2f(p.p.x - 7, p.p.y + 35);
			glVertex2f(p.p.x + 7, p.p.y + 35);
			glVertex2f(p.p.x + 7, p.p.y + 15);
			glEnd();
		}
	}

	//rectangle
	{
		//recltange
		{
			glColor3ub(p.c1.r, p.c1.g, p.c1.b);
			glRectf(p.p.x - 30, p.p.y + 15, p.p.x + 30, p.p.y - 20);
		}

		//frame
		{
			glColor3f(0, 0, 0);
			glBegin(GL_LINE_STRIP);

			glVertex2f(p.p.x + 7, p.p.y + 15);
			glVertex2f(p.p.x - 7, p.p.y + 15);
			glVertex2f(p.p.x - 30, p.p.y + 15);
			glVertex2f(p.p.x - 30, p.p.y - 20);
			glVertex2f(p.p.x + 30, p.p.y - 20);
			glVertex2f(p.p.x + 30, p.p.y + 15);
			glVertex2f(p.p.x + 7, p.p.y + 15);

			glEnd();
		}

		//ornament
		{
			glBegin(GL_POLYGON);


			glColor3ub(168, 168, 255);
			glVertex2f(p.p.x + 30, p.p.y - 2.5);

			glColor3ub(p.c2.r, p.c2.g, p.c2.b);
			glVertex2f(p.p.x + 7, p.p.y + 14);
			glVertex2f(p.p.x - 7, p.p.y + 14);

			glColor3ub(168, 168, 255);
			glVertex2f(p.p.x - 30, p.p.y - 2.5);

			glColor3ub(128, 128, 255);
			glVertex2f(p.p.x, p.p.y - 20);
			glEnd();

			//frames
			glBegin(GL_LINE_LOOP);
			glColor3f(0, 0, 0);

			glVertex2f(p.p.x + 30, p.p.y - 2.5);
			glVertex2f(p.p.x + 7, p.p.y + 15);
			glVertex2f(p.p.x - 7, p.p.y + 15);
			glVertex2f(p.p.x - 30, p.p.y - 2.5);
			glVertex2f(p.p.x, p.p.y - 20);

			glEnd();
		}
	}

	//hit circle
	{
		glColor3f(1, 0, 0);
		circle(p.p.x, p.p.y, 7);
		glColor3f(0, 0, 0);
		circle_wire(p.p.x, p.p.y, 7);
	}

	//circles
	{
		//cicrles
		{
			glColor3ub(128, 128, 255);

			//left 
			circle(p.p.x - 20, p.p.y - 30, 10);

			//mid
			circle(p.p.x, p.p.y - 30, 10);

			//right 
			circle(p.p.x + 20, p.p.y - 30, 10);
		}

		//frames
		{
			glLineWidth(2);
			glColor3f(0, 0, 0);

			//left 
			circle_wire(p.p.x - 20, p.p.y - 30, 10);

			//mid
			circle_wire(p.p.x, p.p.y - 30, 10);

			//right 
			circle_wire(p.p.x + 20, p.p.y - 30, 10);
		}

		//ornaments
		{
			glColor3f(0, 0, 0);

			glBegin(GL_LINES);

			//left
			{
				vertex({ 0, 10 }, { p.p.x - 20,p.p.y - 30 }, p.angle);
				vertex({ 0, -10 }, { p.p.x - 20,p.p.y - 30 }, p.angle);
				vertex({ 10, 0 }, { p.p.x - 20,p.p.y - 30 }, p.angle);
				vertex({ -10, 0 }, { p.p.x - 20,p.p.y - 30 }, p.angle);
			}

			//mid
			{
				vertex({ 0, 10 }, { p.p.x  ,p.p.y - 30 }, p.angle);
				vertex({ 0, -10 }, { p.p.x ,p.p.y - 30 }, p.angle);
				vertex({ 10, 0 }, { p.p.x ,p.p.y - 30 }, p.angle);
				vertex({ -10, 0 }, { p.p.x ,p.p.y - 30 }, p.angle);
			}

			//right
			{
				vertex({ 0, 10 }, { p.p.x + 20,p.p.y - 30 }, p.angle);
				vertex({ 0, -10 }, { p.p.x + 20,p.p.y - 30 }, p.angle);
				vertex({ 10, 0 }, { p.p.x + 20,p.p.y - 30 }, p.angle);
				vertex({ -10, 0 }, { p.p.x + 20,p.p.y - 30 }, p.angle);
			}

			glEnd();
		}
	}
}

void disp_arrow(arrow_t a, player_t p, point_t lp)
{
	if (a.active == true)
	{
		glLineWidth(3);

		//straight line
		{
			glBegin(GL_LINES);

			glColor3ub(a.c.r, a.c.g, a.c.b);
			glVertex2f(a.p.x, a.p.y);

			glColor3f(0, 0, 0);
			glVertex2f(lp.x, lp.y - 40);

			glEnd();
		}

		//left arrow
		{
			glBegin(GL_LINES);

			glColor3ub(a.c.r, a.c.g, a.c.b);
			glVertex2f(a.p.x, a.p.y);
			glVertex2f(a.p.x - 7, a.p.y - 10);

			glEnd();
		}

		//right arrow
		{
			glBegin(GL_LINES);

			glVertex2f(a.p.x, a.p.y);
			glVertex2f(a.p.x + 7, a.p.y - 10);

			glEnd();
		}
	}
}

void disp_balls(ball_t b[])
{
	glLineWidth(2);

	for (int i = 0; i < MAXBALL; i++)
	{
		if (b[i].active == true)
		{
			glColor3ub(b[i].c.r, b[i].c.g, b[i].c.b);

			//Display circle
			{
				if (b[i].type == SMALL)
					circle(b[i].p.x, b[i].p.y, SMALLRAD);
				else if (b[i].type == MID)
					circle(b[i].p.x, b[i].p.y, MIDRAD);
				else if (b[i].type == BIG)
					circle(b[i].p.x, b[i].p.y, BIGRAD);
				else if (b[i].type == HUGE)
					circle(b[i].p.x, b[i].p.y, HUGERAD);

				//display wire 
				{
					glColor3f(0, 0, 0);
					if (b[i].type == SMALL)
						circle_wire(b[i].p.x, b[i].p.y, SMALLRAD);
					else if (b[i].type == MID)
						circle_wire(b[i].p.x, b[i].p.y, MIDRAD);
					else if (b[i].type == BIG)
						circle_wire(b[i].p.x, b[i].p.y, BIGRAD);
					else if (b[i].type == HUGE)
						circle_wire(b[i].p.x, b[i].p.y, HUGERAD);
				}

				//display shine
				{
					glColor3f(1, 1, 1);
					if (b[i].type == SMALL)
						circle(b[i].p.x + -2, b[i].p.y + 2, 3);
					else if (b[i].type == MID)
					{
						glBegin(GL_POLYGON);
						glVertex2f(b[i].p.x + (MIDRAD - 10) * cos(D2R * 105), b[i].p.y + (MIDRAD - 10) * sin(D2R * 105));
						glVertex2f(b[i].p.x + (MIDRAD - 10) * cos(D2R * 120), b[i].p.y + (MIDRAD - 10) * sin(D2R * 120));
						glVertex2f(b[i].p.x + (MIDRAD - 10) * cos(D2R * 135), b[i].p.y + (MIDRAD - 10) * sin(D2R * 135));
						glVertex2f(b[i].p.x + (MIDRAD - 10) * cos(D2R * 150), b[i].p.y + (MIDRAD - 10) * sin(D2R * 150));
						glVertex2f(b[i].p.x + (MIDRAD - 10) * cos(D2R * 165), b[i].p.y + (MIDRAD - 10) * sin(D2R * 165));
						glVertex2f(b[i].p.x + (MIDRAD - 16) * cos(D2R * 165), b[i].p.y + (MIDRAD - 16) * sin(D2R * 165));
						glVertex2f(b[i].p.x + (MIDRAD - 16) * cos(D2R * 150), b[i].p.y + (MIDRAD - 16) * sin(D2R * 150));
						glVertex2f(b[i].p.x + (MIDRAD - 16) * cos(D2R * 135), b[i].p.y + (MIDRAD - 16) * sin(D2R * 135));
						glVertex2f(b[i].p.x + (MIDRAD - 16) * cos(D2R * 120), b[i].p.y + (MIDRAD - 16) * sin(D2R * 120));
						glVertex2f(b[i].p.x + (MIDRAD - 16) * cos(D2R * 105), b[i].p.y + (MIDRAD - 16) * sin(D2R * 105));
						glEnd();

						circle(b[i].p.x + -5, b[i].p.y + 5, 3);
					}
					else if (b[i].type == BIG)
					{
						glBegin(GL_POLYGON);
						glVertex2f(b[i].p.x + (BIGRAD - 7) * cos(D2R * 105), b[i].p.y + (BIGRAD - 7) * sin(D2R * 105));
						glVertex2f(b[i].p.x + (BIGRAD - 7) * cos(D2R * 120), b[i].p.y + (BIGRAD - 7) * sin(D2R * 120));
						glVertex2f(b[i].p.x + (BIGRAD - 7) * cos(D2R * 135), b[i].p.y + (BIGRAD - 7) * sin(D2R * 135));
						glVertex2f(b[i].p.x + (BIGRAD - 7) * cos(D2R * 150), b[i].p.y + (BIGRAD - 7) * sin(D2R * 150));
						glVertex2f(b[i].p.x + (BIGRAD - 7) * cos(D2R * 165), b[i].p.y + (BIGRAD - 7) * sin(D2R * 165));
						glVertex2f(b[i].p.x + (BIGRAD - 22) * cos(D2R * 165), b[i].p.y + (BIGRAD - 22) * sin(D2R * 165));
						glVertex2f(b[i].p.x + (BIGRAD - 22) * cos(D2R * 150), b[i].p.y + (BIGRAD - 22) * sin(D2R * 150));
						glVertex2f(b[i].p.x + (BIGRAD - 22) * cos(D2R * 135), b[i].p.y + (BIGRAD - 22) * sin(D2R * 135));
						glVertex2f(b[i].p.x + (BIGRAD - 22) * cos(D2R * 120), b[i].p.y + (BIGRAD - 22) * sin(D2R * 120));
						glVertex2f(b[i].p.x + (BIGRAD - 22) * cos(D2R * 105), b[i].p.y + (BIGRAD - 22) * sin(D2R * 105));
						glEnd();

						circle(b[i].p.x + -12, b[i].p.y + 12, 6);
					}
					else if (b[i].type == HUGE)
					{
						glBegin(GL_POLYGON);
						glVertex2f(b[i].p.x + (HUGERAD - 10) * cos(D2R * 105), b[i].p.y + (HUGERAD - 10) * sin(D2R * 105));
						glVertex2f(b[i].p.x + (HUGERAD - 10) * cos(D2R * 120), b[i].p.y + (HUGERAD - 10) * sin(D2R * 120));
						glVertex2f(b[i].p.x + (HUGERAD - 10) * cos(D2R * 135), b[i].p.y + (HUGERAD - 10) * sin(D2R * 135));
						glVertex2f(b[i].p.x + (HUGERAD - 10) * cos(D2R * 150), b[i].p.y + (HUGERAD - 10) * sin(D2R * 150));
						glVertex2f(b[i].p.x + (HUGERAD - 10) * cos(D2R * 165), b[i].p.y + (HUGERAD - 10) * sin(D2R * 165));
						glVertex2f(b[i].p.x + (HUGERAD - 30) * cos(D2R * 165), b[i].p.y + (HUGERAD - 30) * sin(D2R * 165));
						glVertex2f(b[i].p.x + (HUGERAD - 30) * cos(D2R * 150), b[i].p.y + (HUGERAD - 30) * sin(D2R * 150));
						glVertex2f(b[i].p.x + (HUGERAD - 30) * cos(D2R * 135), b[i].p.y + (HUGERAD - 30) * sin(D2R * 135));
						glVertex2f(b[i].p.x + (HUGERAD - 30) * cos(D2R * 120), b[i].p.y + (HUGERAD - 30) * sin(D2R * 120));
						glVertex2f(b[i].p.x + (HUGERAD - 30) * cos(D2R * 105), b[i].p.y + (HUGERAD - 30) * sin(D2R * 105));
						glEnd();

						circle(b[i].p.x + -20, b[i].p.y + 20, 6);
					}
				}
			}
		}
	}
}

void disp_Lose_Screen(int scr, int lvl)
{
	//background
	{
		glBegin(GL_QUADS);
		glColor3ub(142, 65, 209);
		glVertex2f(-800, 400);
		glVertex2f(800, 400);


		glColor3ub(47, 7, 59);
		glVertex2f(800, -400);
		glVertex2f(-800, -400);

		glEnd();
	}

	//frames
	{
		glLineWidth(10);

		//black
		{
			glColor3f(0, 0, 0);
			glBegin(GL_LINE_LOOP);
			glVertex2f(-630, 275);
			glVertex2f(630, 275);
			glVertex2f(630, -275);
			glVertex2f(-630, -275);
			glEnd();
		}

		//lose message frame
		{
			glLineWidth(6);
			glColor3ub(112, 146, 190);
			glBegin(GL_LINE_STRIP);
			glVertex2f(-210, 270);
			glVertex2f(-210, 170);
			glVertex2f(210, 170);
			glVertex2f(210, 270);
			glEnd();
		}

		//play again message frame
		{
			glColor3ub(112, 146, 190);
			glBegin(GL_LINE_STRIP);
			glVertex2f(-420, -270);
			glVertex2f(-420, -130);
			glVertex2f(420, -130);
			glVertex2f(420, -270);
			glEnd();
		}

	}

	//messages
	{
		glLineWidth(3.5);
		glColor3ub(242, 125, 214);
		vprint2(-140, 200, 0.5, "You lose");

		vprint2(-250, 50, 0.5, "Current Level: %d", lvl);

		vprint2(-210, -50, 0.5, "Last Score: %d", scr);

		glColor3ub(0, 0, 0);
		vprint2(-160, -220, 0.5, "Enter");

		glColor3ub(254, 112, 211);
		vprint2(-370, -220, 0.5, "Press Enter Play Again");
	}

	//circles
	{
		//left
		{
			glColor3ub(102, 203, 197);
			circle(-450, 25, 50);

			glColor3ub(51, 187, 177);
			circle(-450, 25, 40);

			glColor3ub(2, 170, 150);
			circle(-450, 25, 30);

			glColor3ub(1, 128, 117);
			circle(-450, 25, 20);

			glColor3ub(0, 85, 78);
			circle(-450, 25, 10);

			glLineWidth(2);
			glColor3f(0, 0, 0);
			circle_wire(-450, 25, 52);
		}

		//right
		{
			glColor3ub(102, 203, 197);
			circle(450, 25, 50);

			glColor3ub(51, 187, 177);
			circle(450, 25, 40);

			glColor3ub(2, 170, 150);
			circle(450, 25, 30);

			glColor3ub(1, 128, 117);
			circle(450, 25, 20);

			glColor3ub(0, 85, 78);
			circle(450, 25, 10);

			glLineWidth(2);
			glColor3f(0, 0, 0);
			circle_wire(450, 25, 52);
		}
	}
}

void disp_Win_Screen(int scr, int lvl, int sec, int times)
{
	//background
	{
		//big blue circle
		{
			glBegin(GL_POLYGON);

			//glColor3ub(242, 182, 44);
			//glColor3ub(255, 255, 0);
			glColor3ub(0, 255, 0);
			glVertex2f(0, 0);

			//glColor3ub(255, 40, 60);
			glColor3ub(100, 200, 200);
			for (int i = 90; i <= 450; i += 5)
				glVertex2f(500 * cos(i * D2R), 350 * sin(i * D2R));
			glEnd();
		}
	}

	//frames
	{
		glBegin(GL_LINE_LOOP);
		glColor3f(0, 1, 1);

		for (int i = 0; i <= 360; i += 5)
			glVertex2f(500 * cos(i * D2R), 350 * sin(i * D2R));
		glEnd();
	}

	//circles
	{
		//left circle
		{
			glBegin(GL_LINES);
			for (int i = 115; i <= 245; i += 10)
			{
				glColor3ub(0, 0, 0);
				glVertex2f(-750, 0);

				glColor3f(0, 1, 0);
				glVertex2f(cos(i * D2R) * 510, 400 * sin(i * D2R));
			}
			glEnd();
		}

		//right circle
		{
			glBegin(GL_LINES);
			for (int i = 295; i <= 425; i += 10)
			{
				glColor3ub(0, 0, 0);
				glVertex2f(750, 0);

				glColor3f(0, 1, 0);
				glVertex2f(cos(i * D2R) * 510, 400 * sin(i * D2R));
			}
			glEnd();
		}
	}

	//messages
	{
		glLineWidth(5);
		glColor3f(0, 0, 0);
		vprint2(-140, 203, 0.5, "You Win");
		glColor3f(1, 0, 0);
		vprint2(-144, 200, 0.5, "You Win");

		glColor3ub(0, 0, 255);

		vprint2(-280, 100, 0.4, "Remaining time %d Sec.", times - sec - 1);

		vprint2(-210, 20, 0.5, "* Score: %d *", scr);

		vprint2(-300, -70, 0.5, "Level %d Completed", lvl);

		glColor3ub(73, 239, 220);
		vprint2(42, -176, 0.5, "Enter");

		glColor3ub(0, 0, 0);
		vprint2(40, -178, 0.5, "Enter");

		glColor3ub(50, 100, 255);
		glColor3f(1, 0, 0);
		vprint2(-180, -180, 0.5, "Press Enter ");
		vprint2(-220, -260, 0.5, "For Next Level", lvl);
	}
}

void disp_Menu_Screen(char p, char choice)
{
	//display background
	{
		glBegin(GL_POLYGON);
		glColor3ub(255, 38, 118);
		glVertex2f(0, 0);
		glColor3ub(68, 0, 171);
		for (int i = 0; i <= 365; i += 5)
		{
			glVertex2f(900 * cos(i * D2R), 900 * sin(i * D2R));
		}
		glEnd();
	}

	//display messages
	{
		//splash
		{
			//message
			{

				for (int i = 10; i >= 1; i--)
				{
					glColor3f(0, i / 10., 0);
					glLineWidth(i);
					vprint2(-250, 120, 1, "SPLASH");
				}
			}

			//left circle
			{
				glColor3f(0, 1, 1);
				glLineWidth(4);

				glBegin(GL_LINES);
				for (int i = 0; i < 360; i += 45)
				{
					glColor3f(0, 1, 1);
					glVertex2f(-300, 170);
					glColor3f(0, 0, 0);
					glVertex2f(170 * cos(i * D2R) - 500, 100 * sin(i * D2R) + 170);
				}
				glEnd();
			}

			//right circle
			{
				glColor3f(0, 1, 1);
				glLineWidth(4);

				glBegin(GL_LINES);
				for (int i = 0; i < 360; i += 45)
				{
					if (i != 180)
					{
						glColor3f(0, 1, 1);
						glVertex2f(+300, 170);
						glColor3f(0, 0, 0);
						glVertex2f(170 * cos(i * D2R) + 500, 100 * sin(i * D2R) + 170);
					}
				}
				glEnd();
			}

			//up arrow
			{
				//body
				{
					glBegin(GL_POLYGON);
					glColor3f(1, 0, 0);
					glVertex2f(-150, 325);
					glVertex2f(-150, 305);
					glColor3f(1, 1, 0);
					glVertex2f(150, 305);
					glVertex2f(150, 325);
					glEnd();
				}

				//front up
				{
					glColor3f(1, 0, 0);
					glBegin(GL_POLYGON);
					glVertex2f(-140, 315);
					glVertex2f(-170, 315);
					glVertex2f(-100, 355);
					glVertex2f(-70, 355);
					glEnd();
				}

				//front down
				{
					glBegin(GL_POLYGON);
					glVertex2f(-140, 315);
					glVertex2f(-170, 315);
					glVertex2f(-100, 275);
					glVertex2f(-70, 275);
					glEnd();
				}

				//frame
				{
					glColor3f(0, 0, 0);
					glLineWidth(4);

					glBegin(GL_LINE_LOOP);
					glVertex2f(150, 305);
					glVertex2f(150, 325);
					glVertex2f(-121, 325);
					glVertex2f(-70, 355);
					glVertex2f(-100, 355);
					glVertex2f(-170, 315);
					glVertex2f(-100, 275);
					glVertex2f(-70, 275);
					glVertex2f(-121, 305);

					glEnd();
				}
			}

			//down arrow
			{
				//body
				{
					glBegin(GL_POLYGON);
					glColor3f(1, 1, 0);
					glVertex2f(-150, 40);
					glVertex2f(-150, 20);
					glColor3f(1, 0, 0);
					glVertex2f(150, 20);
					glVertex2f(150, 40);
					glEnd();
				}

				//front up
				{
					glBegin(GL_POLYGON);
					glVertex2f(140, 30);
					glVertex2f(170, 30);
					glVertex2f(100, 70);
					glVertex2f(70, 70);
					glEnd();
				}

				//front down
				{
					glBegin(GL_POLYGON);
					glVertex2f(140, 30);
					glVertex2f(170, 30);
					glVertex2f(100, -10);
					glVertex2f(70, -10);
					glEnd();
				}

				//frame
				{
					glColor3f(0, 0, 0);
					glLineWidth(4);

					glBegin(GL_LINE_LOOP);
					glVertex2f(-150, 40);
					glVertex2f(121, 40);
					glVertex2f(70, 70);
					glVertex2f(100, 70);
					glVertex2f(170, 30);
					glVertex2f(100, -10);
					glVertex2f(70, -10);
					glVertex2f(121, 20);
					glVertex2f(-150, 20);
					glEnd();
				}
			}
		};

		//players
		{
			//box
			{
				glBegin(GL_QUAD_STRIP);

				glColor3ub(25, 105, 57);
				glVertex2f(-320, -40);
				glVertex2f(-320, -120);

				glColor3ub(104, 50, 36);
				glVertex2f(0, -40);
				glVertex2f(0, -120);

				glColor3ub(200, 22, 33);
				glVertex2f(320, -40);
				glVertex2f(320, -120);

				glEnd();
			}

			//frame
			{
				//violet
				{
					glColor3ub(82, 3, 167);
					glBegin(GL_LINE_LOOP);
					glVertex2f(-320, -40);
					glVertex2f(-320, -120);
					glVertex2f(0, -120);
					glVertex2f(0, -40);
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex2f(0, -40);
					glVertex2f(0, -120);
					glVertex2f(320, -120);
					glVertex2f(320, -40);
					glEnd();
				}

				if (p == 1)
				{
					glColor3f(0, 0.3, 0);
					glBegin(GL_LINE_LOOP);
					glVertex2f(-330, -30);
					glVertex2f(-330, -130);
					glVertex2f(10, -130);
					glVertex2f(10, -30);
					glEnd();

					glColor3f(0, 0.6, 0);
					glBegin(GL_LINE_LOOP);
					glVertex2f(-326, -34);
					glVertex2f(-326, -126);
					glVertex2f(6, -126);
					glVertex2f(6, -34);
					glEnd();

					glColor3f(0, 1, 0);
					glBegin(GL_LINE_LOOP);
					glVertex2f(-322, -38);
					glVertex2f(-322, -122);
					glVertex2f(2, -122);
					glVertex2f(2, -38);
					glEnd();
				}

				else if (p == 2)
				{
					glColor3f(0, 0.3, 0);
					glBegin(GL_LINE_LOOP);
					glVertex2f(330, -30);
					glVertex2f(330, -130);
					glVertex2f(-10, -130);
					glVertex2f(-10, -30);
					glEnd();

					glColor3f(0, 0.6, 0);
					glBegin(GL_LINE_LOOP);
					glVertex2f(326, -34);
					glVertex2f(326, -126);
					glVertex2f(-6, -126);
					glVertex2f(-6, -34);
					glEnd();

					glColor3f(0, 1, 0);
					glBegin(GL_LINE_LOOP);
					glVertex2f(322, -38);
					glVertex2f(322, -122);
					glVertex2f(-2, -122);
					glVertex2f(-2, -38);
					glEnd();
				}
			}

			//messages
			{
				glLineWidth(4);
				glColor3f(0, 0, 0);
				vprint2(-275, -100, 0.4, "1 Player   2 Player");

				if (p == 1)
				{
					glColor3f(1, 1, 0);
					vprint2(-272, -97, 0.4, "1 Player");
					glColor3ub(122, 192, 255);
					glColor3f(0, 1, 1);
					vprint2(-270, -95, 0.4, "1 Player");
				}
				else if (p == 2)
				{
					glColor3f(1, 1, 0);
					vprint2(65, -97, 0.4, "2 Player");
					glColor3ub(122, 192, 255);
					glColor3f(0, 1, 1);
					vprint2(67, -95, 0.4, "2 Player");
				}
			}
		}

		//Play 
		{
			//box
			{
				glColor3ub(32, 150, 249);
				if (choice == 1)
					glColor3ub(252, 232, 31);

				glBegin(GL_QUADS);
				glVertex2f(-80, -145);
				glVertex2f(80, -145);
				glVertex2f(80, -220);
				glVertex2f(-80, -220);
				glEnd();
			}

			//frame
			{
				glLineWidth(4);
				glColor3f(0, 0, 0);
				glBegin(GL_LINE_LOOP);
				glVertex2f(-80, -145);
				glVertex2f(80, -145);
				glVertex2f(80, -220);
				glVertex2f(-80, -220);
				glEnd();
			}

			//message
			{
				glLineWidth(4);
				if (choice == 1)
					glColor3f(1, 0, 0.3);
				else
					glColor3f(0, 0, 0);
				vprint2(-47, -200, 0.4, "Play");
			}
		}

		//Select Level
		{
			//box
			{
				glColor3ub(32, 150, 249);
				if (choice == 2)
					glColor3ub(252, 232, 31);

				glBegin(GL_QUADS);
				glVertex2f(-170, -220);
				glVertex2f(170, -220);
				glVertex2f(170, -290);
				glVertex2f(-170, -290);
				glEnd();
			}

			//frame
			{
				glLineWidth(4);
				glColor3f(0, 0, 0);
				glBegin(GL_LINE_LOOP);
				glVertex2f(-170, -220);
				glVertex2f(170, -220);
				glVertex2f(170, -290);
				glVertex2f(-170, -290);
				glEnd();
			}

			//message
			{
				glLineWidth(4);
				if (choice == 2)
					glColor3f(1, 0, 0.3);
				else
					glColor3f(0, 0, 0);
				vprint2(-150, -270, 0.4, "Select Level");
			}
		}

		//How to Play
		{
			//box
			{
				glColor3ub(32, 150, 249);
				if (choice == 3)
					glColor3ub(252, 232, 31);

				glBegin(GL_QUADS);
				glVertex2f(-210, -290);
				glVertex2f(210, -290);
				glVertex2f(210, -360);
				glVertex2f(-210, -360);
				glEnd();
			}

			//frame
			{
				glLineWidth(4);
				glColor3f(0, 0, 0);
				glBegin(GL_LINE_LOOP);
				glVertex2f(-210, -290);
				glVertex2f(210, -290);
				glVertex2f(210, -360);
				glVertex2f(-210, -360);
				glEnd();
			}

			//message
			{
				glLineWidth(4);
				if (choice == 3)
					glColor3f(1, 0, 0.3);
				else
					glColor3f(0, 0, 0);
				vprint2(-155, -340, 0.4, "How to play");
			}
		}

		//display bottom arrow
		{
			if (choice == 1)
			{
				//left arrow
				{
					glColor3f(0, 1, 0);
					glBegin(GL_TRIANGLES);
					glVertex2f(-100, -180);
					glVertex2f(-140, -160);
					glVertex2f(-140, -200);
					glEnd();
				}

				//right arrow
				{
					glColor3f(0, 1, 0);
					glBegin(GL_TRIANGLES);
					glVertex2f(100, -180);
					glVertex2f(140, -160);
					glVertex2f(140, -200);
					glEnd();
				}

				//frames
				{
					glColor3ub(0, 153, 0);

					//left arrow
					{
						glBegin(GL_LINE_LOOP);
						glVertex2f(-100, -180);
						glVertex2f(-140, -160);
						glVertex2f(-140, -200);
						glEnd();
					}

					//right arrow
					{
						glBegin(GL_LINE_LOOP);
						glVertex2f(100, -180);
						glVertex2f(140, -160);
						glVertex2f(140, -200);
						glEnd();
					}

					glColor3ub(0, 76, 0);

					//left arrow
					{
						glBegin(GL_LINE_LOOP);
						glVertex2f(-96, -180);
						glVertex2f(-144, -156);
						glVertex2f(-144, -204);
						glEnd();
					}

					//right arrow
					{
						glBegin(GL_LINE_LOOP);
						glVertex2f(96, -180);
						glVertex2f(144, -156);
						glVertex2f(144, -204);
						glEnd();
					}
				}
			}

			else if (choice == 2)
			{
				//left arrow
				{
					glColor3f(0, 1, 0);
					glBegin(GL_TRIANGLES);
					glVertex2f(-200, -250);
					glVertex2f(-240, -230);
					glVertex2f(-240, -270);
					glEnd();
				}

				//right arrow
				{
					glColor3f(0, 1, 0);
					glBegin(GL_TRIANGLES);
					glVertex2f(200, -250);
					glVertex2f(240, -230);
					glVertex2f(240, -270);
					glEnd();
				}

				//frames
				{
					glColor3ub(0, 153, 0);

					//left arrow
					{
						glBegin(GL_LINE_LOOP);
						glVertex2f(-200, -250);
						glVertex2f(-240, -230);
						glVertex2f(-240, -270);
						glEnd();
					}

					//right arrow
					{
						glBegin(GL_LINE_LOOP);
						glVertex2f(200, -250);
						glVertex2f(240, -230);
						glVertex2f(240, -270);
						glEnd();
					}

					glColor3ub(0, 76, 0);

					//left arrow
					{
						glBegin(GL_LINE_LOOP);
						glVertex2f(-196, -250);
						glVertex2f(-244, -226);
						glVertex2f(-244, -274);
						glEnd();
					}

					//right arrow
					{
						glBegin(GL_LINE_LOOP);
						glVertex2f(196, -250);
						glVertex2f(244, -226);
						glVertex2f(244, -274);
						glEnd();
					}
				}
			}

			else if (choice == 3)
			{
				//left arrow
				{
					glColor3f(0, 1, 0);
					glBegin(GL_TRIANGLES);
					glVertex2f(-240, -325);
					glVertex2f(-280, -305);
					glVertex2f(-280, -345);
					glEnd();
				}

				//right arrow
				{
					glColor3f(0, 1, 0);
					glBegin(GL_TRIANGLES);
					glVertex2f(240, -325);
					glVertex2f(280, -305);
					glVertex2f(280, -345);
					glEnd();
				}

				//frames
				{
					glColor3ub(0, 153, 0);

					//left arrow
					{
						glBegin(GL_LINE_LOOP);
						glVertex2f(-240, -325);
						glVertex2f(-280, -305);
						glVertex2f(-280, -345);
						glEnd();
					}

					//right arrow
					{
						glBegin(GL_LINE_LOOP);
						glVertex2f(240, -325);
						glVertex2f(280, -305);
						glVertex2f(280, -345);
						glEnd();
					}

					glColor3ub(0, 76, 0);

					//left arrow
					{
						glBegin(GL_LINE_LOOP);
						glVertex2f(-236, -325);
						glVertex2f(-284, -301);
						glVertex2f(-284, -349);
						glEnd();
					}

					//right arrow
					{
						glBegin(GL_LINE_LOOP);
						glVertex2f(236, -325);
						glVertex2f(284, -301);
						glVertex2f(284, -349);
						glEnd();
					}
				}
			}
		}

		//display name label
		{
			glColor3f(0, 1, 1);
			vprint2(260, -350, 0.31, "By Ahmet Oguz Ergin");
		}
	}
}

void disp_Info_Screen()
{
	//background
	{
		glBegin(GL_QUADS);

		glColor3ub(6, 250, 239);
		glVertex2f(-800, 500);
		glVertex2f(800, 500);

		glColor3ub(62, 118, 232);
		glVertex2f(800, -500);
		glVertex2f(-800, -500);

		glEnd();
	}

	//box
	{
		glBegin(GL_QUADS);
		glColor3f(1, 1, 0);
		glVertex2f(-620, 180);
		glVertex2f(620, 180);
		glColor3f(0, 1, 1);
		glVertex2f(620, -150);
		glVertex2f(-620, -150);
		glEnd();
	}

	//frame
	{
		glLineWidth(5);
		glColor3f(0, 0, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(-620, 180);
		glVertex2f(620, 180);
		glVertex2f(620, -150);
		glVertex2f(-620, -150);
		glEnd();
	}

	//message
	{
		glColor3f(0, 0, 0);
		glLineWidth(2);
		vprint(-300, 100, GLUT_BITMAP_TIMES_ROMAN_24, "After you specified player count and level, you can start game.");
		vprint(-350, 50, GLUT_BITMAP_TIMES_ROMAN_24, "The goal is to burst all the balloons by shooting arrow in the specified time.");
		vprint(-580, 0, GLUT_BITMAP_TIMES_ROMAN_24, "There is a small circle in the middle of the players. In order not to lose the game, you must avoid the bouncing balloons.");
		vprint(-180, -50, GLUT_BITMAP_TIMES_ROMAN_24, "That's all. Easy Peasy. Good luck :) ");
		vprint(-290, -100, GLUT_BITMAP_TIMES_ROMAN_24, "Control keys:   Player 1: arrow keys or ijkl   Player 2: wasd");
	}

}

void disp_Level_Screen(int* lvl)
{
	//background
	{
		glBegin(GL_TRIANGLE_STRIP);
		glColor3ub(253, 178, 136);
		glVertex2f(-800, 400);

		glColor3ub(253, 134, 102);
		glVertex2f(800, 400);
		glVertex2f(-800, -400);

		glColor3ub(253, 90, 69);
		glVertex2f(800, -400);

		glEnd();
	}

	//Select level 
	{
		//box
		{
			glColor3ub(253, 90, 69);
			glBegin(GL_QUADS);
			glVertex2f(-200, 350);
			glVertex2f(200, 350);

			glColor3ub(220, 95, 184);
			glVertex2f(200, 230);
			glVertex2f(-200, 230);
			glEnd();
		}

		//frame
		{
			glBegin(GL_LINE_LOOP);
			glColor3ub(220, 95, 184);
			glVertex2f(-200, 350);
			glVertex2f(200, 350);
			glColor3ub(253, 90, 69);
			glVertex2f(200, 230);
			glVertex2f(-200, 230);
			glEnd();

			//black
			glColor3f(0, 0, 0);
			glBegin(GL_LINE_LOOP);
			glVertex2f(-203, 353);
			glVertex2f(203, 353);
			glVertex2f(203, 227);
			glVertex2f(-203, 227);
			glEnd();
		}

		//message
		{
			glColor3f(0, 0, 0);
			glLineWidth(3);
			vprint2(-150, 275, 0.4, "Select Level");
		}
	}

	//boxes
	{
		//row
		{
			glBegin(GL_LINES);
			for (int i = 0; i < 6; i++)
			{
				glVertex2f(-500, 200 - i * 105);
				glVertex2f(500, 200 - i * 105);
			}
			glEnd();
		}

		//column
		{
			glBegin(GL_LINES);
			for (int i = 0; i < 11; i++)
			{
				glVertex2f(-500 + i * 100, 200);
				glVertex2f(-500 + i * 100, -325);
			}
			glEnd();
		}
	}

	//level box
	{
		glColor3ub(220, 95, 184);

		glBegin(GL_QUADS);

		if (*lvl > 0 && *lvl < 11)
		{
			glVertex2f(-600 + *lvl * 100, 200);
			glVertex2f(-500 + *lvl * 100, 200);
			glVertex2f(-500 + *lvl * 100, 95);
			glVertex2f(-600 + *lvl * 100, 95);
		}
		else if (*lvl > 10 && *lvl < 21)
		{
			glVertex2f(-600 + (*lvl - 10) * 100, 95);
			glVertex2f(-500 + (*lvl - 10) * 100, 95);
			glVertex2f(-500 + (*lvl - 10) * 100, -10);
			glVertex2f(-600 + (*lvl - 10) * 100, -10);
		}
		else if (*lvl > 20 && *lvl < 31)
		{
			glVertex2f(-600 + (*lvl - 20) * 100, -10);
			glVertex2f(-500 + (*lvl - 20) * 100, -10);
			glVertex2f(-500 + (*lvl - 20) * 100, -115);
			glVertex2f(-600 + (*lvl - 20) * 100, -115);
		}
		else if (*lvl > 30 && *lvl < 41)
		{
			glVertex2f(-600 + (*lvl - 30) * 100, -115);
			glVertex2f(-500 + (*lvl - 30) * 100, -115);
			glVertex2f(-500 + (*lvl - 30) * 100, -220);
			glVertex2f(-600 + (*lvl - 30) * 100, -220);
		}
		else if (*lvl > 40 && *lvl < 51)
		{
			glVertex2f(-600 + (*lvl - 40) * 100, -220);
			glVertex2f(-500 + (*lvl - 40) * 100, -220);
			glVertex2f(-500 + (*lvl - 40) * 100, -325);
			glVertex2f(-600 + (*lvl - 40) * 100, -325);
		}
		glEnd();
	}

	//level frame 
	{
		//blue
		{
			glColor3f(0, 1, 0);
			glLineWidth(9);
			glBegin(GL_LINE_LOOP);

			if (*lvl > 0 && *lvl < 11)
			{
				glVertex2f(-600 + *lvl * 100, 200);
				glVertex2f(-500 + *lvl * 100, 200);
				glVertex2f(-500 + *lvl * 100, 95);
				glVertex2f(-600 + *lvl * 100, 95);
			}
			else if (*lvl > 10 && *lvl < 21)
			{
				glVertex2f(-600 + (*lvl - 10) * 100, 95);
				glVertex2f(-500 + (*lvl - 10) * 100, 95);
				glVertex2f(-500 + (*lvl - 10) * 100, -10);
				glVertex2f(-600 + (*lvl - 10) * 100, -10);
			}
			else if (*lvl > 20 && *lvl < 31)
			{
				glVertex2f(-600 + (*lvl - 20) * 100, -10);
				glVertex2f(-500 + (*lvl - 20) * 100, -10);
				glVertex2f(-500 + (*lvl - 20) * 100, -115);
				glVertex2f(-600 + (*lvl - 20) * 100, -115);
			}
			else if (*lvl > 30 && *lvl < 41)
			{
				glVertex2f(-600 + (*lvl - 30) * 100, -115);
				glVertex2f(-500 + (*lvl - 30) * 100, -115);
				glVertex2f(-500 + (*lvl - 30) * 100, -220);
				glVertex2f(-600 + (*lvl - 30) * 100, -220);
			}
			else if (*lvl > 40 && *lvl < 51)
			{
				glVertex2f(-600 + (*lvl - 40) * 100, -220);
				glVertex2f(-500 + (*lvl - 40) * 100, -220);
				glVertex2f(-500 + (*lvl - 40) * 100, -325);
				glVertex2f(-600 + (*lvl - 40) * 100, -325);
			}
			glEnd();
		}

		//red
		{
			glColor3f(1, 0, 0);
			glLineWidth(7);
			glBegin(GL_LINE_LOOP);

			if (*lvl > 0 && *lvl < 11)
			{
				glVertex2f(-600 + *lvl * 100, 200);
				glVertex2f(-500 + *lvl * 100, 200);
				glVertex2f(-500 + *lvl * 100, 95);
				glVertex2f(-600 + *lvl * 100, 95);
			}
			else if (*lvl > 10 && *lvl < 21)
			{
				glVertex2f(-600 + (*lvl - 10) * 100, 95);
				glVertex2f(-500 + (*lvl - 10) * 100, 95);
				glVertex2f(-500 + (*lvl - 10) * 100, -10);
				glVertex2f(-600 + (*lvl - 10) * 100, -10);
			}
			else if (*lvl > 20 && *lvl < 31)
			{
				glVertex2f(-600 + (*lvl - 20) * 100, -10);
				glVertex2f(-500 + (*lvl - 20) * 100, -10);
				glVertex2f(-500 + (*lvl - 20) * 100, -115);
				glVertex2f(-600 + (*lvl - 20) * 100, -115);
			}
			else if (*lvl > 30 && *lvl < 41)
			{
				glVertex2f(-600 + (*lvl - 30) * 100, -115);
				glVertex2f(-500 + (*lvl - 30) * 100, -115);
				glVertex2f(-500 + (*lvl - 30) * 100, -220);
				glVertex2f(-600 + (*lvl - 30) * 100, -220);
			}
			else if (*lvl > 40 && *lvl < 51)
			{
				glVertex2f(-600 + (*lvl - 40) * 100, -220);
				glVertex2f(-500 + (*lvl - 40) * 100, -220);
				glVertex2f(-500 + (*lvl - 40) * 100, -325);
				glVertex2f(-600 + (*lvl - 40) * 100, -325);
			}
			glEnd();
		}

		//yellow
		{
			glColor3f(1, 1, 0);
			glLineWidth(2);
			glBegin(GL_LINE_LOOP);

			if (*lvl > 0 && *lvl < 11)
			{
				glVertex2f(-600 + *lvl * 100, 200);
				glVertex2f(-500 + *lvl * 100, 200);
				glVertex2f(-500 + *lvl * 100, 95);
				glVertex2f(-600 + *lvl * 100, 95);
			}
			else if (*lvl > 10 && *lvl < 21)
			{
				glVertex2f(-600 + (*lvl - 10) * 100, 95);
				glVertex2f(-500 + (*lvl - 10) * 100, 95);
				glVertex2f(-500 + (*lvl - 10) * 100, -10);
				glVertex2f(-600 + (*lvl - 10) * 100, -10);
			}
			else if (*lvl > 20 && *lvl < 31)
			{
				glVertex2f(-600 + (*lvl - 20) * 100, -10);
				glVertex2f(-500 + (*lvl - 20) * 100, -10);
				glVertex2f(-500 + (*lvl - 20) * 100, -115);
				glVertex2f(-600 + (*lvl - 20) * 100, -115);
			}
			else if (*lvl > 30 && *lvl < 41)
			{
				glVertex2f(-600 + (*lvl - 30) * 100, -115);
				glVertex2f(-500 + (*lvl - 30) * 100, -115);
				glVertex2f(-500 + (*lvl - 30) * 100, -220);
				glVertex2f(-600 + (*lvl - 30) * 100, -220);
			}
			else if (*lvl > 40 && *lvl < 51)
			{
				glVertex2f(-600 + (*lvl - 40) * 100, -220);
				glVertex2f(-500 + (*lvl - 40) * 100, -220);
				glVertex2f(-500 + (*lvl - 40) * 100, -325);
				glVertex2f(-600 + (*lvl - 40) * 100, -325);
			}
			glEnd();
		}
	}
	;

	//digits
	{
		glLineWidth(4);
		glColor3ub(0, 116, 194);

		//1-10
		{
			vprint2(-463, 132, 0.35, "1");
			vprint2(-363, 132, 0.35, "2");
			vprint2(-263, 132, 0.35, "3");
			vprint2(-163, 132, 0.35, "4");
			vprint2(-63, 132, 0.35, "5");
			vprint2(37, 132, 0.35, "6");
			vprint2(137, 132, 0.35, "7");
			vprint2(237, 132, 0.35, "8");
			vprint2(337, 132, 0.35, "9");
			vprint2(422, 132, 0.35, "10");
		}

		//11-50
		{
			//11-20
			for (int i = 0; i < 10; i++)
				vprint2(i * 100 - 475, 25, 0.35, "%d", i + 11);

			//21-30
			for (int i = 0; i < 10; i++)
				vprint2(i * 100 - 475, -80, 0.35, "%d", i + 21);

			//31-40
			for (int i = 0; i < 10; i++)
				vprint2(i * 100 - 475, -185, 0.35, "%d", i + 31);

			//41-50
			for (int i = 0; i < 10; i++)
				vprint2(i * 100 - 475, -290, 0.35, "%d", i + 41);
		}
	}
}

void display() {
	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (screen == PLAYSCREEN)
	{
		//display background big rectangle
		dispbackrec();

		//display frames
		disp_frames();

		//display informations
		disp_info(score, timeremain, lvl, times);

		//display arrow
		disp_arrow(a1, p1, lastpoint1);
		if (playercount == 2)
			disp_arrow(a2, p2, lastpoint2);

		//display player
		disp_player(p1);
		if (playercount == 2)
			disp_player(p2);

		//display balls
		disp_balls(ball);
	}

	else if (screen == LOSESCREEN)
		disp_Lose_Screen(score, lvl);

	else if (screen == WINSCREEN)
		disp_Win_Screen(score, lvl, times, timeremain);

	else if (screen == MENUSCREEN)
		disp_Menu_Screen(playercount, menuchoice);

	else if (screen == INFOSCREEN)
		disp_Info_Screen();

	else if (screen == LEVELSCREEN)
		disp_Level_Screen(&lvl);

	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	switch (key) {
	case 'A':
	case 'a': left1 = true; break;

	case'D':
	case 'd': right1 = true; break;

	case'w':
	case'W':
	case'v':
	case 'V': up1 = true; break;

	case 32: up = true; break;
	}

	switch (key) {
	case 'J':
	case 'j': left = true; break;
	case 'L':
	case 'l': right = true; break;
	case 73:
	case 253: up = true; break;
	}

	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	//lose and win screen -> play screen
	if (screen == LOSESCREEN || screen == WINSCREEN)
	{
		if ((key == 's' || key == 'S' || key == 13) && screen == LOSESCREEN)
			inittolvl(lvl, &lvl, ball, &lastballsize, &p1, &p2, &a1, &a2, &barincrease, &timeremain, &timems, &times, &stop, &screen, &score, playercount);

		if ((key == 's' || key == 'S' || key == 13) && screen == WINSCREEN)
		{
			if (lvl == 50)
				screen = MENUSCREEN;
			else
				inittolvl(lvl + 1, &lvl, ball, &lastballsize, &p1, &p2, &a1, &a2, &barincrease, &timeremain, &timems, &times, &stop, &screen, &score, playercount);
		}
	}

	//menu screen -> play screen, info 
	else if (screen == MENUSCREEN)
	{
		if (key == 13 && menuchoice == 1)
			inittolvl(lvl, &lvl, ball, &lastballsize, &p1, &p2, &a1, &a2, &barincrease, &timeremain, &timems, &times, &stop, &screen, &score, playercount);

		else if (key == 13 && menuchoice == 2)
			screen = LEVELSCREEN;

		else if (key == 13 && menuchoice == 3)
			screen = INFOSCREEN;
	}


	//info screen -> menu screen
	else if (screen == INFOSCREEN && key == 13)
		screen = MENUSCREEN;

	//menu screen
	{
		if (key == '1' && screen == MENUSCREEN)
			playercount = 1;

		else if (key == '2' && screen == MENUSCREEN)
			playercount = 2;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	switch (key) {
	case 'A':
	case 'a': left1 = false; break;

	case'D':
	case 'd': right1 = false; break;

	case'w':
	case'W':
	case'v':
	case 'V': up1 = false; break;

	case 32: up = false; break;
	}

	switch (key) {
	case 'J':
	case 'j': left = false; break;
	case 'L':
	case 'l': right = false; break;
	case 105: up = false; break;
	}

	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	}

	//menu screen choices
	if (screen == MENUSCREEN)
	{
		if (left == true)
			playercount = 1;

		else if (right == true)
			playercount = 2;

		else if (up == true)
		{
			if (menuchoice != 1)
				menuchoice--;
		}

		else if (down == true)
		{
			if (menuchoice != 3)
				menuchoice++;
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// GLUT to OpenGL coordinate conversion:
	x = x - winWidth / 2;
	y = winHeight / 2 - y;

	//return from info screen
	if (screen == INFOSCREEN && stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
		screen = MENUSCREEN;

	//level selection
	else if (screen == LEVELSCREEN && stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		if (y <= 200 && y >= 95)
		{
			for (int i = 1; i <= 10; i++)
				if (x >= -600 + i * 100 && x < -500 + i * 100)
					lvl = i;
		}

		else if (y <= 95 && y >= -10)
		{
			for (int i = 1; i <= 10; i++)
				if (x >= -600 + i * 100 && x < -500 + i * 100)
					lvl = i + 10;
		}

		else if (y <= -10 && y >= -115)
		{
			for (int i = 1; i <= 10; i++)
				if (x >= -600 + i * 100 && x < -500 + i * 100)
					lvl = i + 20;
		}

		else if (y <= -115 && y >= -220)
		{
			for (int i = 1; i <= 10; i++)
				if (x >= -600 + i * 100 && x < -500 + i * 100)
					lvl = i + 30;
		}

		else if (y <= -220 && y >= -325)
		{
			for (int i = 1; i <= 10; i++)
				if (x >= -600 + i * 100 && x < -500 + i * 100)
					lvl = i + 40;
		}

		screen = MENUSCREEN;
		//printf("\n%d", lvl);
	}

	//menu choices
	else if (screen == MENUSCREEN && stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		//double click
		{
			if (stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON && x >= -80 && x <= 80 && y >= -220 && y <= -145 && menuchoice == 1)
				inittolvl(lvl, &lvl, ball, &lastballsize, &p1, &p2, &a1, &a2, &barincrease, &timeremain, &timems, &times, &stop, &screen, &score, playercount);


			if (stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON && x >= -170 && x <= 170 && y >= -290 && y <= -220 && menuchoice == 2)
				screen = LEVELSCREEN;


			if (stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON && x >= -210 && x <= 210 && y >= -360 && y <= -290 && menuchoice == 3)
				screen = INFOSCREEN;
		}

		if (stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON && x >= -320 && x <= 0 && y >= -120 && y <= -40)
			playercount = 1;

		if (stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON && x >= 0 && x <= 320 && y >= -120 && y <= -40)
			playercount = 2;

		if (stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON && x >= -80 && x <= 80 && y >= -220 && y <= -145)
			menuchoice = 1;

		if (stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON && x >= -170 && x <= 170 && y >= -290 && y <= -220)
			menuchoice = 2;

		if (stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON && x >= -210 && x <= 210 && y >= -360 && y <= -290)
			menuchoice = 3;
	}

	//return to menu screen
	else if (screen == PLAYSCREEN && stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON && x >= 150 && x <= 385 && y >= -395 && y <= -353)
		screen = MENUSCREEN;

	if (stat == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
		printf("%d , %d\n", x, y);

	if (stat == GLUT_DOWN && button == GLUT_RIGHT_BUTTON)
		printf("glVertex2f(%d , %d);\n", x, y);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.
	x = x - winWidth / 2;
	y = winHeight / 2 - y;

	//level selection
	if (screen == LEVELSCREEN)
	{
		if (y <= 200 && y >= 95)
		{
			for (int i = 1; i <= 10; i++)
				if (x >= -600 + i * 100 && x < -500 + i * 100)
					lvl = i;
		}

		else if (y <= 95 && y >= -10)
		{
			for (int i = 1; i <= 10; i++)
				if (x >= -600 + i * 100 && x < -500 + i * 100)
					lvl = i + 10;
		}

		else if (y <= -10 && y >= -115)
		{
			for (int i = 1; i <= 10; i++)
				if (x >= -600 + i * 100 && x < -500 + i * 100)
					lvl = i + 20;
		}

		else if (y <= -115 && y >= -225)
		{
			for (int i = 1; i <= 10; i++)
				if (x >= -600 + i * 100 && x < -500 + i * 100)
					lvl = i + 30;
		}

		else if (y <= -225 && y >= -325)
		{
			for (int i = 1; i <= 10; i++)
				if (x >= -600 + i * 100 && x < -500 + i * 100)
					lvl = i + 40;
		}
	}

	//return to menu screen
	if (screen == PLAYSCREEN && x >= 150 && x <= 385 && y >= -395 && y <= -353)
		isDarkMenu = true;
	else
		isDarkMenu = false;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	if (stop <= 0 && screen == PLAYSCREEN)
	{
		//move player
		move_player(&p1, &p2, playercount);

		//activate arrows
		if (up == true)
		{
			if (a1.active == false)
			{
				a1.active = true;

				a1.p.x = p1.p.x;
				a1.p.y = -242;
				lastpoint1.x = p1.p.x;
				lastpoint1.y = p1.p.y;
			}
		}
		if (playercount == 2 && up1 == true)
		{
			if (a2.active == false)
			{
				a2.active = true;

				a2.p.x = p2.p.x;
				a2.p.y = -242;
				lastpoint2.x = p2.p.x;
				lastpoint2.y = p2.p.y;
			}
		}

		//move arrow
		{
			move_arrow(&a1);
			if (playercount == 2)
				move_arrow(&a2);
		}

		//check arrow hits
		arrow_Hit(&a1, &a2, ball, &lastballsize, playercount);

		//check time
		if (timeremain - times == 0)
		{
			stop = 3000;
		}

		//check win 
		if (lastballsize == 0)
		{
			stop = 2000;
		}

		//check if ball hits the player
		check_ball_hits_player(ball, p1, p2, &lastballsize, playercount);

		//move ball
		move_ball(ball, &lastballsize, &score);

		//time related
		{
			//increase time
			timems += 16;
			if (timems >= 1000)
			{
				times++;
				timems -= 1000;
			}
		}
	}

	//wait
	if (stop > 0 && screen == PLAYSCREEN)
	{
		stop -= 16;

		if (stop <= 0)
		{
			if (lastballsize > 0)
				screen = LOSESCREEN;
			else if (lastballsize == 0)
				screen = WINSCREEN;
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	//initialize menu
	{
		menuchoice = 1;
		lvl = 1;
	}

	//initialize player count
	playercount = 2;

	//initialize lvl
	inittolvl(1, &lvl, ball, &lastballsize, &p1, &p2, &a1, &a2, &barincrease, &timeremain, &timems, &times, &stop, &screen, &score, playercount);

	//initialize screen to menu //sil
	screen = MENUSCREEN;

	// Smoothing shapes
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	//time function
	{
		srand(time(NULL));
	}
}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(20, 50);
	glutCreateWindow("Bubble Splash");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}