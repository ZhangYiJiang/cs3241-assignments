// CS3241Lab1.cpp : Defines the entry point for the console application.
#include <cmath>
#include <iostream>

/* Include header files depending on platform */
#ifdef _WIN32
	#include "glut.h"

	#define M_PI 3.14159
#elif __APPLE__
	#include <OpenGL/gl.h>
	#include <GLUT/GLUT.h>
#elif __linux__
        #include <GL/glut.h>
#endif

using namespace std;

float alpha = 0.0, k=1.2;
float tx = 0.0, ty = 0.0;

//--------------------------------------------------------------------------
// Utils
//--------------------------------------------------------------------------

// Draws a circle with radii of 1 using 0, 0 as its center
void drawCircle() 
{
	glBegin(GL_TRIANGLE_FAN);
	glVertex2d(0, 0);
	for (int i = 0; i <= 20; i++) {
		float angle = (float)i / 10;
		glVertex2d(sin(angle * M_PI), cos(angle * M_PI));
	}
	glEnd();
}

// Draw a semicircle with a radii of 1 with 0, 0 as center starting from
// the 12 o'clock position down to 6 o'clock
void drawSemicircle()
{
	glBegin(GL_TRIANGLE_FAN);
	glVertex2d(0, 0);
	for (int i = 0; i <= 10; i++) {
		float angle = (float)i / 10;
		glVertex2d(sin(angle * M_PI), cos(angle * M_PI));
	}
	glEnd();
}

void drawBackground() 
{
	glBegin(GL_POLYGON);
		glVertex2f(-4, 0);
		glVertex2f(4, 0);
		glVertex2f(4, 8);
		glVertex2f(-4, 8);
	glEnd();
}

// Wraparound array indices such that -n becomes the nth last element, and 
// (length) becomes first, (length+1) becomes second, etc.
int wrap(int i, int max) {
	if (i < 0) return i + max;
	if (i >= max) return i - max;
	return i;
}

//--------------------------------------------------------------------------
// Twilight Sparkle
//--------------------------------------------------------------------------
GLdouble starOuter[6][2] = {
	{ 0.0, 0.0 },
	{ 1.73, 1.82 },
	{ 1.61, 3.94 },
	{ 0.16, 5.77 },
	{ -1.58, 4.02 },
	{ -1.89, 1.48 },
};

GLdouble starInner[6][2] = {
	{ 0.49, 2.1 },
	{ 0.78f, 2.86f },
	{ 0.43f, 3.57f },
	{ -0.28f, 3.53f },
	{ -0.61f, 2.79f },
	{ -0.32f, 2.12f },
};

GLdouble starPositions[5][2] = {
	{ 1.43, -0.09 },
	{ -1.44, -0.38 },
	{ 1.78, 3.66 },
	{ -0.88, 4.53 },
	{ -2.24, 2.59 },
};

// Draws a six pointed starburst with 0, 0 at the topmost point of the star
void drawSparkle()
{
	// The sparkle is made of 6 triangles for each of the six points
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glVertex2dv(starOuter[i]);
		glVertex2dv(starInner[i]);
		glVertex2dv(starInner[wrap(i - 1, 6)]);
	}
	glEnd();

	// plus the interior polygon created using fan triangulation
	glBegin(GL_POLYGON);
	for (auto &i : starInner) {
		glVertex2dv(i);
	}
	glEnd();
}

void twilightSparkle() 
{
	glPushMatrix();
		glScalef(0.95, -0.95, 1);
		glTranslatef(0.3, -7, 0);

		// Draw background starburst
		glPushMatrix();
			glTranslatef(-0.95, 1.4, 0);
			glScalef(0.593, 0.593, 1);
			glRotatef(-34, 0, 0, 1);

			glColor3f(1, 1, 1);
			drawSparkle();
		glPopMatrix();

		// Draw large magenta sparkle
		glColor3f(0.925, 0.267, 0.549);
		drawSparkle();

		// Draw six surrounding sparkles
		glColor3f(1, 1, 1);
		for (auto &starPosition : starPositions) {
			glPushMatrix();
				glTranslated(starPosition[0], starPosition[1], 0);
				glScalef(0.307, 0.307, 1);
				drawSparkle();
			glPopMatrix();
		}	
	glPopMatrix();
}

//--------------------------------------------------------------------------
// Rarity
//--------------------------------------------------------------------------
GLfloat diamond[4][2] = {
	{0, 0},
	{0.84, 1.67},
	{0.07, 3.57},
	{-0.81, 1.94},
};

GLfloat diamondCross[4][2] = {
	{ 0.6, 1.18 }, 
	{ 0.55, 2.4 }, 
	{ -0.41, 2.66 }, 
	{ -0.51, 1.27 },
};

// Draw one of Rarity's diamonds with 0, 0 at the top, including the outer rhombus,
// lines that between each corner and center of the diamond and
// the oval in the center
void drawDiamond()
{
	GLfloat foreground[3] = { 0.16, 0.59, 0.83 };
	GLfloat background[3] = { 0.51, 0.82, 0.96 };

	// Fill the diamond background
	glColor3fv(background);
	glBegin(GL_POLYGON);
		for (auto &i : diamond) {
			glVertex2fv(i);
		}
	glEnd();

	// Draw the star shaped lines on the diamond
	glColor3fv(foreground);
	glBegin(GL_LINES);
		glVertex2fv(diamond[0]);
		glVertex2fv(diamond[2]);

		glVertex2fv(diamond[1]);
		glVertex2fv(diamond[3]);

		glVertex2fv(diamondCross[0]);
		glVertex2fv(diamondCross[2]);

		glVertex2fv(diamondCross[1]);
		glVertex2fv(diamondCross[3]);
	glEnd();

	// Create a circular outline by drawing a circle then filling the middle 
	// with a smaller circle
	glPushMatrix();
		glTranslatef(0, 1.86, 0);
		glScalef(0.21, 0.425, 1);

		drawCircle();

		glPushMatrix();
			glScalef(0.85, 0.85, 1);
			glColor3fv(background);

			drawCircle();
		glPopMatrix();
	glPopMatrix();
}

void rarity()
{
	glPushMatrix();
		glScalef(1, -1, 1);
		glTranslatef(0, -7.95, 0);

		// Rarity's mark consists of the same diamond repeated three times
		drawDiamond();

		glPushMatrix();
			glTranslatef(1.42, 2.57, 0);
			drawDiamond();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-0.8, 3.9, 0);
			drawDiamond();
		glPopMatrix();
	glPopMatrix();
}

//--------------------------------------------------------------------------
// Cheerilee
//--------------------------------------------------------------------------
GLfloat cheerileePetal[3] = { 0.957, 0.843, 0.89 };
GLfloat cheerileeCenter[3] = { 0.969, 0.957, 0.694 };
GLfloat cheerileeBackground[3] = { 0.675, 0.263, 0.537 };

// Trace out a semi-circular outline to represent an eye or mouth
void drawEye()
{
	glPushMatrix();
		glColor3fv(cheerileeBackground);
		glRotatef(90, 0, 0, 1);
		glScalef(0.65, 0.45, 1);
		drawSemicircle();

		glScalef(0.8, 1, 1);
		glColor3fv(cheerileeCenter);
		drawSemicircle();
	glPopMatrix();
}

void drawFlower()
{
	// Eight surrounding circles for the petals
	glColor3fv(cheerileePetal);
	float distance = 1.7;
	float radius = 0.72;
	for (int i = 0; i < 8; i++) {
		float angle = (float)i / 4;
		glPushMatrix();
			glTranslatef(distance * sin(angle * M_PI), distance * cos(angle * M_PI), 0);
			glScalef(radius, radius, 1);
			drawCircle();
		glPopMatrix();
	}

	// Single big circle in the middle for the face
	glColor3fv(cheerileeCenter);
	glPushMatrix();
		glScalef(1.56, 1.56, 1);
		drawCircle();
	glPopMatrix();

	// Two eyes
	float eyeX = 0.58;
	float eyeY = 0.2;

	glPushMatrix();
		glTranslatef(-eyeX, eyeY, 0);
		drawEye();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(eyeX, eyeY, 0);
		drawEye();
	glPopMatrix();

	// One smiling mouth - flip the eye shape 180 degrees and 
	glPushMatrix();
		glTranslatef(0, -0.2, 0);
		glScalef(1.2, 1.2, 1);
		glRotatef(180, 0, 0, 1);

		drawEye();
	glPopMatrix();
}

void cheerilee()
{
	// Cheerilee's mark is three smiling flowers. The first and last have the same angle and
	// x displacement, while the second lies between the two on the y axis and is rotated in
	// the opposite direction
	float angle = 28;

	glPushMatrix();
		glScalef(0.78, 0.78, 1);
		glTranslatef(-2, 2.6, 0);

		glPushMatrix();
			glRotatef(angle, 0, 0, 1);
			drawFlower();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(4.3, 2.6, 0);
			glRotatef(-angle, 0, 0, 1);
			drawFlower();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0, 5, 0);
			glRotatef(angle, 0, 0, 1);
			drawFlower();
		glPopMatrix();
	glPopMatrix();
}

//--------------------------------------------------------------------------
// Pinkie Pie
//--------------------------------------------------------------------------
void drawBalloon(const GLfloat *balloonColor, const GLfloat *stringColor)
{
	// Draw balloon - first an oval rotated, then the tail portion
	glColor3fv(balloonColor);
	glPushMatrix();
		glRotatef(10, 0, 0, 1);
		glScalef(1, 1.46, 1);

		drawCircle();
	glPopMatrix();

	glBegin(GL_POLYGON);
		glVertex2f(-0.1, -1.35);
		glVertex2f(0.4, -1.35);
		glVertex2f(0.6, -1.85);
		glVertex2f(-0.2, -1.75);
	glEnd();

	// String
	glColor3fv(stringColor);
	glBegin(GL_LINE_STRIP);
		// Start from somewhere around where the balloon ends
		float x = 0.1;
		float y = -1.9;

		// A small sine wave for the first part.
		for (int i = 0; i < 15; i++) {
			float dx = sin(((float)i / 7 + 0.5) * M_PI) * 0.16;
			float dy = -((float)i / 15) * 2.2;
			glVertex2f(x + dx, y + dy);
		}

		// A partial circle for the second part
		float r = 0.4;
		x += sin(2 * M_PI) * 0.16 - 0.24;
		y += -2.3;
		for (int i = 0; i < 15; i++) {
			float angle = ((float)i / 13 + 0.5) * M_PI;
			glVertex2f(x + sin(angle) * r, y + cos(angle) * r);
		}
	glEnd();
}

void pinkiePie()
{
	GLfloat blue[3] = { 0.522, 0.816, 0.957 };
	GLfloat yellow[3] = { 0.984, 0.976, 0.706 };

	glPushMatrix();
		glTranslatef(0, 6.3, 0);
		glScalef(0.9, 0.9, 1);

		drawBalloon(yellow, blue);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(1.8, 4.2, 0);
		glScalef(0.9, 0.9, 1);
		glRotatef(-15, 0, 0, 1);

		drawBalloon(blue, yellow);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-1.5, 4.5, 0);
		glScalef(-0.85, 0.85, 1);
		glRotatef(-15, 0, 0, 1);

		drawBalloon(blue, yellow);
	glPopMatrix();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();

	//controls transformation
	glScalef(k, k, k);	
	glTranslatef(tx, ty, 0);	
	glRotatef(alpha, 0, 0, 1);
	
	//draw your stuff here
	glTranslatef(-4, 0, 0);

	// Twilight Sparkle
	glColor3f(0.862, 0.682, 0.933);
	drawBackground();
	twilightSparkle();
	
	// Rarity
	glPushMatrix();
		glTranslatef(8, 0, 0);

		glColor3f(0.945, 0.945, 0.953);
		drawBackground();
		rarity();
	glPopMatrix();

	// Cheerilee
	glPushMatrix();
		glTranslatef(0, -8, 0);

		glColor3fv(cheerileeBackground);
		drawBackground();
		cheerilee();
	glPopMatrix();

	// Pinkie Pie
	glPushMatrix();
		glTranslatef(8, -8, 0);

		glColor3f(0.98, 0.702, 0.82);
		drawBackground();
		pinkiePie();
	glPopMatrix();

	glPopMatrix();
	glFlush();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10, 10, -10, 10, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init(void)
{
	glClearColor (0.333, 0.675, 0.953, 1.0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
}

void keyboard (unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {

		case 'a':
			alpha+=10;
			glutPostRedisplay();
		break;

		case 'd':
			alpha-=10;
			glutPostRedisplay();
		break;

		case 'q':
			k+=0.1;
			glutPostRedisplay();
		break;

		case 'e':
			if(k>0.1)
				k-=0.1;
			glutPostRedisplay();
		break;

		case 'z':
			tx-=0.1;
			glutPostRedisplay();
		break;

		case 'c':
			tx+=0.1;
			glutPostRedisplay();
		break;

		case 's':
			ty-=0.1;
			glutPostRedisplay();
		break;

		case 'w':
			ty+=0.1;
			glutPostRedisplay();
		break;
			
		case 27:
			exit(0);
		break;

		default:
		break;
	}
}

int main(int argc, char **argv)
{
	cout<<"CS3241 Lab 1\n\n";
	cout<<"+++++CONTROL BUTTONS+++++++\n\n";
	cout<<"Scale Up/Down: Q/E\n";
	cout<<"Rotate Clockwise/Counter-clockwise: A/D\n";
	cout<<"Move Up/Down: W/S\n";
	cout<<"Move Left/Right: Z/C\n";
	cout <<"ESC: Quit\n";

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (600, 600);
	glutInitWindowPosition (50, 50);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	//glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
