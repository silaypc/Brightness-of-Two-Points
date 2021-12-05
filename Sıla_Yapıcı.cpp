
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON       1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

//define color structure
typedef struct {
	float r, g, b;
} color_t;

//define light structure
typedef struct {
	vec_t   pos;
	color_t color;
	bool alive;
} light_t;

//define vertex structure
typedef struct {
	vec_t pos;
	vec_t N;
} vertex_t;

//define the lights with their respective position, color and status
light_t lightA = { { -60 , 80 }, { 0, 1, 0 }, 1};
light_t lightB = { { 0 , 100 }, { 1, 0, 0 }, 0};
   

color_t mulColor(float k, color_t c) {
	color_t tmp = { k * c.r, k * c.g, k * c.b };
	return tmp;
}


// To add distance into calculation
// when distance is 0  => its impact is 1.0
// when distance is 1000000 => impact is 0.0
// Linear impact of distance on light calculation.
double distanceImpact(double d) {
	return (-1.0 / 1000000.0) * d + 1.0;
}

color_t calculateColor(light_t source, vertex_t v) {
	vec_t L = subV(source.pos, v.pos);
	vec_t uL = unitV(L);
	float factor = dotP(uL, v.N) * distanceImpact(magV(L));
	return mulColor(factor, source.color);
}


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
		angle = 2 * PI*i / 100;
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
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char *string, void *font)
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
void vprint(int x, int y, void *font, const char *string, ...)
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

void vprint2(int x, int y, float size, const char *string, ...) {
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

//
// To display onto window using OpenGL commands
//
void display() {

	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1, 1, 0);
	vprint(-270, 270, GLUT_BITMAP_HELVETICA_18, "Press [a] or [b] to change position");

	if (lightA.alive) {
		glColor3f(0, 1, 0);
		vprint(-66, 94, GLUT_BITMAP_HELVETICA_18, "A");
		vprint(-122, 94, GLUT_BITMAP_HELVETICA_12, "(-60, 80)");
		glColor3f(0.4, 0, 0);
		vprint(-6, 114, GLUT_BITMAP_HELVETICA_18, "B");
		vprint(10, 114, GLUT_BITMAP_HELVETICA_12, "(0, 100)");
	}
	else {
		glColor3f(0, 0.3, 0);
		vprint(-66, 94, GLUT_BITMAP_HELVETICA_18, "A");
		vprint(-122, 94, GLUT_BITMAP_HELVETICA_12, "(-60, 80)");
		glColor3f(1, 0, 0);
		vprint(-6, 114, GLUT_BITMAP_HELVETICA_18, "B");
		vprint(10, 114, GLUT_BITMAP_HELVETICA_12, "(0, 100)");
	}

	// light source 
	if (lightA.alive) {
		glColor3f(0,1,0);
		circle(lightA.pos.x, lightA.pos.y, 5);
	}
	if (lightB.alive)
	{
		glColor3f(lightB.color.r, lightB.color.g, lightB.color.b);
		circle(lightB.pos.x, lightB.pos.y, 5);
	}
	color_t pointP;
	vertex_t point = { {0,0}, {0,1} };
	for (int x = -300; x <= 300; x++) {
		vertex_t P = { { x, 0}, { 0, 1 } };

		color_t res = { 0, 0, 0 };
		if (lightA.alive)
			res = calculateColor(lightA, P);
		if (lightB.alive)
			res = calculateColor(lightB, P);
		glBegin(GL_LINES);
		glColor3f(res.r, res.g, res.b);
		glVertex2f(x, 0);

		glColor3f(0, 0, 0);
		glVertex2f(x, -300);
		glEnd();
	}
	
	glColor3f(1, 1, 0);
	vprint(6, 8, GLUT_BITMAP_HELVETICA_18, "P (0,0)");
	circle(0, 0, 3);

	if (lightA.alive)
	pointP = calculateColor(lightA, point);
	if (lightB.alive)
	pointP = calculateColor(lightB, point);

	vprint(-270, -270, GLUT_BITMAP_HELVETICA_18, "r = %.1lf  g = %.1lf  b = %.1lf", pointP.r, pointP.g, pointP.b);
	double brightness;
	brightness = (pointP.r * 0.21) + (pointP.g * 0.72) + (pointP.b * 0.07);
	vprint(120, -270, GLUT_BITMAP_HELVETICA_18, "brightness is: %.3lf", brightness);
	glutSwapBuffers();

}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == 'b' || key == 'B')
	{
		lightA.alive = 0;
		lightB.alive = 1;
	}
	if (key == 'a' || key == 'A')
	{
		lightA.alive = 1;
		lightB.alive = 0;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
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
	// Write your codes here.



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

void onMove(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.


	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char *argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Sila Yapici");

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
