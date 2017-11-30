
// CS3241Lab1.cpp : Defines the entry point for the console application.
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory>
#include <random>
#include <array>

#ifdef _WIN32
    #include <Windows.h>
    #include "glut.h"
    #define M_PI 3.141592654
#elif __APPLE__
    #include <OpenGL/gl.h>
    #include <GLUT/GLUT.h>
#elif __linux__
    #include <GL/glut.h>
#endif

using namespace std;

// global variable
default_random_engine generator;
uniform_real_distribution<double> dist(0.0, 1.0);

// pi value can be accessed using the built-in M_PI
bool m_Smooth = false;
bool m_Highlight = false;
bool useBestViewingAngle = false;
GLfloat angle;   /* in degrees */
GLfloat angle2;   /* in degrees */
GLfloat zoom;
double zNear, zFar, fovy;
int mouseButton = 0;
int moving, startx, starty;
double currentFrame = 0;

int current_object = 0;

double randBetween(double min, double max) {
    return dist(generator) * (max - min) + min;
}

double randBetween(double max = 1) {
    return randBetween(0, max);
}

void resetCamera()
{
    angle = 0;
    angle2 = 0;
    zoom = 1.0;
    zNear = 1.0;
    zFar = 80.0;
    fovy = 40.0;
    useBestViewingAngle = false;
}

void setupLighting()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
    
	// Lights, material properties
    GLfloat	ambientProperties[]  = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat	diffuseProperties[]  = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat	specularProperties[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lightPosition[] = {-100.0f,100.0f,100.0f,1.0f};
	
    glClearDepth( 1.0 );
    
	glLightfv( GL_LIGHT0, GL_POSITION, lightPosition);
	
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambientProperties);
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseProperties);
    glLightfv( GL_LIGHT0, GL_SPECULAR, specularProperties);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);
    
	// Default : lighting
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}

void setBlueMaterial()
{
    float no_mat[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float mat_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    float mat_ambient_color[] = {0.8f, 0.8f, 0.2f, 1.0f};
    float mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float mat_diffuse[] = {0.1f, 0.5f, 0.8f, 1.0f};
    float no_shininess = 0.0f;
    float high_shininess = 100.0f;
    float mat_emission[] = {0.3f, 0.2f, 0.2f, 0.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    // glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    if (m_Highlight) {
        // your codes for highlight here
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, high_shininess);
    } else {
        glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
        glMaterialf(GL_FRONT, GL_SHININESS, no_shininess);
    }

    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
}


void setMetalMaterial()
{
    float no_mat[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float mat_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    float mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float no_shininess = 0.0f;
    float high_shininess = 100.0f;
    float mat_emission[] = {0.1f, 0.1f, 0.1f, 0.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    if (m_Highlight) {
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, high_shininess);
    } else {
        glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
        glMaterialf(GL_FRONT, GL_SHININESS, no_shininess);
    }
}

void setBlackMaterial()
{
    float no_mat[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float mat_ambient[] = {0.05f, 0.05f, 0.05f, 1.0f};
    float mat_ambient_color[] = {0.8f, 0.8f, 0.2f, 1.0f};
    float mat_specular[] = {0.2f, 0.2f, 0.2f, 1.0f};
    float mat_diffuse[] = { 0.35f, 0.35f, 0.35f, 1.0f };
    float no_shininess = 0.0f;
    float high_shininess = 10.0f;
    float mat_emission[] = {0.1f, 0.1f, 0.1f, 0.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    if (m_Highlight) {
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, high_shininess);
    } else {
        glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
        glMaterialf(GL_FRONT, GL_SHININESS, no_shininess);
    }
}

void drawSpherePoint(int n, double r, double x, double y, bool drawNormal)
{
    double m = M_PI / n;

    if (drawNormal) {
        // The normal at each vertex is equivalent to a vector pointing from the center of the sphere
        // towards the vertex
        glNormal3d(r * sin(x * m) * cos(y * m), r * cos(x * m) * cos(y * m), r * sin(y * m));
    }

    glVertex3d(r * sin(x * m) * cos(y * m), r * cos(x * m) * cos(y * m), r * sin(y * m));
}

void drawSphere(double r, int n = 30)
{
    int i,j;
    double m = M_PI / n;
    for (i = 0; i < n; i++)
		for (j = 0; j < 2 * n;j++) {
            glBegin(GL_POLYGON);
                if (!m_Smooth) {
                    // Explanation: the normal of the whole polygon is the coordinate of the center of the polygon for a sphere
                    glNormal3d(sin((i + 0.5) * m) * cos((j + 0.5) * m), cos((i + 0.5) * m) * cos((j + 0.5) * m), sin((j + 0.5) * m));
                }

                drawSpherePoint(n, r, i, j, m_Smooth);
                drawSpherePoint(n, r, i + 1, j, m_Smooth);
                drawSpherePoint(n, r, i + 1, j + 1, m_Smooth);
                drawSpherePoint(n, r, i, j + 1, m_Smooth);
            glEnd();
        }
}

void drawCircle(double r)
{
    double nr = 30, m = M_PI / nr;

    glBegin(GL_TRIANGLE_FAN);
        glNormal3d(0, 0, -1);
        glVertex3d(0, 0, 0);

        for (int i = 0; i <= 2 * nr; i++) {
            glVertex3d(r * cos(i * m), r * sin(i * m), 0);
        }
    glEnd();
}

void drawTriangle()
{
    glBegin(GL_TRIANGLES);
        glVertex3d(0, 0, 0);
        glVertex3d(1, 0, -1);
        glVertex3d(0, 0, -1);
    glEnd();
}

void drawCylinderPoint(double r, double h, double p, bool drawNormal)
{
    double nr = 20;
    double m = M_PI / nr;

    if (drawNormal) glNormal3d(r * cos(p * m), r * sin(p * m), 0);
    glVertex3d(r * cos(p * m), r * sin(p * m), h);
}

void drawCylinder(double r1, double r2, double h, bool drawCaps = true)
{
    double nr = 20;
    double m = M_PI / nr;

    // Draw caps
    if (drawCaps) {
        drawCircle(r1);

        glPushMatrix();
            glTranslated(0, 0, h);
            glRotatef(180, 1, 0, 0);
            drawCircle(r2);
        glPopMatrix();
    }

    // Draw body
    for (int i = 0; i < 2 * nr; i++) {
        glBegin(GL_POLYGON);
        if (!m_Smooth) {
            double r = r1 + (r2 - r1) / 2;
            glNormal3d(r * cos((i+0.5) * m), r * sin((i+0.5) * m), h/2);
        }

        drawCylinderPoint(r1, 0, i, m_Smooth);
        drawCylinderPoint(r1, 0, i+1, m_Smooth);
        drawCylinderPoint(r2, h, i+1, m_Smooth);
        drawCylinderPoint(r2, h, i, m_Smooth);
        glEnd();
    }
}

void drawCylinder(double r, double h)
{
    drawCylinder(r, r, h);
}

void drawCone(double r, double h)
{
    drawCylinder(0, r, h);
}

class Particle {
public:
    double r = 0.8;
	array<double, 3> color = { { 0.976, 0.659, 0.145 } };
	array<double, 3> dcolor = { { 0, -0.05, -0.03 } };
    double opacity = 0.9, dopacity = -0.085;
    double x = 0, y = 0, z = 0, dx, dy, dz;

    Particle() {
        dx = randBetween(0, -0.12);
        dy = randBetween(0, -0.12);
        dz = randBetween(1.2, 1.8);
    }

    void update() {
        x += dx;
        y += dy;
        z += dz;
        opacity += dopacity;

        for (int i = 0; i < 3; i++) color[i] += dcolor[i];
    }

    void draw() {
        glPushMatrix();
            glTranslated(x, y, z);
            glColor4d(color[0], color[1], color[2], opacity);
            drawCircle(r);
        glPopMatrix();
    }

    bool isTransparent() {
        return opacity <= 0;
    }
};

class ParticleEmitter {
public:
    double rate = 0.5;

    ParticleEmitter() = default;

    void update() {
        if (randBetween() < rate) {
            particles.emplace_back(new Particle());
        }

        for (unique_ptr<Particle> &p : particles) {
            p->update();
        }

        particles.erase(remove_if(particles.begin(),
                                  particles.end(),
                                  [](unique_ptr<Particle> &p){ return p->isTransparent(); }), particles.end());
    }

    void draw() {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);

        for (unique_ptr<Particle> &p : particles) {
            p->draw();
        }

        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);
    }

private:
    vector<unique_ptr<Particle>> particles;
};

ParticleEmitter engines[5];

void drawRocket()
{
    glTranslated(0, 0, 2);
    glScaled(0.4, 0.4, 0.4);

    // Setup camera
    if (useBestViewingAngle) {
        double shake = 0.03;
        gluLookAt(7, 3, 0,
                  randBetween(-shake, shake), randBetween(-shake, shake), -8,
                  -0.1, -0.4, 1);

        glRotated(currentFrame / 3, 0, 0, 1);
    }

    double r1 = 1, r2 = 1.1, noseConeH = 2, height = 12;
    double boosterZPos = 3, boosterR = 0.6, boosterConeH = 1.5, boosterH = 8, boosterBodyH = boosterH - boosterConeH;
    double engineH = 1.1, engineConeH = 0.4;

    // Nose cone
    setBlackMaterial();
    drawCone(r1, -noseConeH);

    glPushMatrix();
        setMetalMaterial();
        glTranslated(0, 0, -noseConeH);

        drawCylinder(r1, r2, -height);

        glPushMatrix();
            setBlackMaterial();
            glTranslated(0, 0, -2.6);
            drawCylinder(1.05, -1.4);
        glPopMatrix();

        // Draw boosters
        for (int i = 0; i < 4; i++) {
            glPushMatrix();
                glRotated(i * 90, 0, 0, 1);
                glTranslated(0.8, 0, -boosterZPos);
                glRotated(-4, 0, 1, 0);

                setMetalMaterial();
                drawCone(boosterR, -boosterConeH);

                glTranslated(0, 0, -boosterConeH);
                setBlackMaterial();
                drawCylinder(boosterR, boosterR, -boosterBodyH, false);

                // Draw engine
                glTranslated(0, 0, -boosterBodyH + 0.1);
                glRotated(4, 0, 1, 0);
                setMetalMaterial();

                drawCylinder(boosterR, -engineH);

                // Draw fins
                glPushMatrix();
                    glTranslated(0.5, 0, 0.6);
                    glScaled(0.5, 0.3, 1.5);
                    drawTriangle();
                glPopMatrix();

                // Draw engine cone
                glTranslated(0, 0, -engineH);
                setBlackMaterial();
                drawCylinder(0.2, 0.6, -engineConeH, false);

                // Draw engine particles
                glScaled(1, 1, -1);
                engines[i].draw();
            glPopMatrix();
        }

        // Draw main engine
        glTranslated(0, 0, -height - 0.2);
        glScaled(2, 2, -1);
        setBlackMaterial();
        drawCylinder(0.2, 0.6, -engineConeH, false);
        engines[4].draw();
    glPopMatrix();
}

void drawSputnik()
{
    setMetalMaterial();
    glColor3f(0.914f, 0.914f, 0.914f);

    if (useBestViewingAngle) {
        gluLookAt(3, 3, 3,
                  0, 0, 1,
                  0, -1, -1);
        glRotated(currentFrame / 2, 0, 0, 1);
    }

    // Draw body
    drawSphere(1);
    drawCylinder(1.02, 0.13);

    for (int i = 0; i < 4; i++) {
        glPushMatrix();
            glRotated(i * 90, 0, 0, 1);
            glRotated(30, 1, 0, 0);
            glTranslated(0, -0.925, 0);

            drawCylinder(0.05, 0.01, 10);
        glPopMatrix();
    }
}

void display()
{

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( /* field of view in degree */ fovy,
            /* aspect ratio */ 1.0,
            /* Z near */ zNear, /* Z far */ zFar);
    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
    glTranslatef(0, 0, -6);
    
    glRotatef(angle2, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);
    
    glScalef(zoom, zoom, zoom);
    
    switch (current_object) {
		case 0:
            setBlueMaterial();
			drawSphere(1);
			break;
		case 1:
            setBlueMaterial();
			drawCylinder(1, 1);
			break;
		case 2:
            drawRocket();
			break;
		case 3:
            drawSputnik();
			break;
		default:
			break;
    };
	glPopMatrix();
	glutSwapBuffers ();
}

void keyboard (unsigned char key, int x, int y)
{
	switch (key) {
        case 'p':
        case 'P':
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            break;
        case 'w':
        case 'W':
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
            break;
        case 'v':
        case 'V':
            glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
            break;
        case 's':
        case 'S':
            m_Smooth = !m_Smooth;
            break;
        case 'h':
        case 'H':
            m_Highlight = !m_Highlight;
            break;

        case 'n':
            zNear -= 1;
            break;
        case 'N':
            zNear += 1;
            break;

        case 'f':
            zFar -= 1;
            break;
        case 'F':
            zFar += 1;

        case 'o':
            fovy -= 5;
            break;
        case 'O':
            fovy += 5;
            break;

        case 'r':
            resetCamera();
            break;

        case 'R':
            useBestViewingAngle = !useBestViewingAngle;
            break;
            
        case '1':
        case '2':
        case '3':
        case '4':
            current_object = key - '1';
            break;
            
        case 'Q':
        case 'q':
            exit(0);
            break;
            
        default:
            break;
	}
    
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        mouseButton = button;
        moving = 1;
        startx = x;
        starty = y;
    }
    if (state == GLUT_UP) {
        mouseButton = button;
        moving = 0;
    }
}

void update()
{
    currentFrame++;
    glutPostRedisplay();

    for (ParticleEmitter &p : engines) {
        p.update();
    }
}

void motion(int x, int y)
{
    if (moving) {
        if(mouseButton==GLUT_LEFT_BUTTON)
        {
            angle = angle + (x - startx);
            angle2 = angle2 + (y - starty);
        }
        else zoom += ((y-starty)*0.001);
        startx = x;
        starty = y;
        glutPostRedisplay();
    }
    
}

int main(int argc, char **argv)
{
	cout<<"CS3241 Lab 3"<< endl<< endl;
    
	cout << "1-4: Draw different objects"<<endl;
	cout << "S: Toggle Smooth Shading"<<endl;
	cout << "H: Toggle Highlight"<<endl;
	cout << "W: Draw Wireframe"<<endl;
	cout << "P: Draw Polygon"<<endl;
    cout << "P: Draw Polygon"<<endl;
    cout << "V: Draw Vertices"<<endl;
    cout << "r: Reset camera"<<endl;
    cout << "R: Switch to best viewing angle"<<endl;
    cout << "o/O: Decrease/Increase FOV"<<endl;
    cout << "n/N: Decrease/Increase near draw distance"<<endl;
    cout << "f/F: Decrease/Increase far draw distance"<<endl;
	cout << "Q: Quit" <<endl<< endl;
    
	cout << "Left mouse click and drag: rotate the object"<<endl;
	cout << "Right mouse click and drag: zooming"<<endl;
    
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (800, 800);
	glutInitWindowPosition (50, 50);
	glutCreateWindow ("CS3241 Assignment 3");
	glClearColor(1, 1, 1, 1);
	glutDisplayFunc(display);
    glutIdleFunc(update);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	setupLighting();
    resetCamera();

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

    glDepthMask(GL_TRUE);
	glutMainLoop();
    
	return 0;
}