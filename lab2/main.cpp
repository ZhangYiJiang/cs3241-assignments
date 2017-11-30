// CS3241Lab2.cpp : Defines the entry point for the console application.
#include <cmath>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <memory>

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

#define numStars 100

#define TIME_MULTIPLIER 0.5

#define DELTA_V 0.00000008
#define G 0.000000095

#define MAX_STAR_BRIGHTNESS 0.2
#define D_STAR_OPACITY 0.0001

#define PLANET_GLOW_SIZE 1.8
#define LAUNCH_PLANET 4

// Global variables
default_random_engine generator;
uniform_real_distribution<double> dist(0.0, 1.0);

const GLfloat clockColor[] = { 0.13, 0.59, 0.95 };
const GLfloat clockTickColor[] = { 0.13, 0.59, 0.95, 0.4 };
float alpha = 0.0, k = 1;
float tx = 0.0, ty = 0.0;
bool showTime = false;
bool mouseDown = false;
float mouseX, mouseY;

// --------------------------------------------------------------------------------------------
// Helper functions
// --------------------------------------------------------------------------------------------
double randBetween(double min, double max) {
    return dist(generator) * (max - min) + min;
}

double randBetween(double max = 1) {
    return randBetween(0, max);
}

void drawCircle(float size = 1, float amount = 1, float startAngle = 0)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(0, 0);
    for (int i = 0; i <= 20; i++) {
        float angle = (float)i / 10;
        glVertex2d(size * sin(amount * angle * M_PI), size * cos(amount * angle * M_PI));
    }
    glEnd();
}

double norm(double dx, double dy) {
    return sqrt(dx * dx + dy * dy);
}

double toRad(double angle) {
    return (angle * M_PI) / 180;
}

double toDeg(double angle) {
    return (angle * 180) / M_PI;
}

void drawClockFace() {
	glColor4fv(clockTickColor);

	for (int i = 0; i < 12; i++) {
		int angle = i * 30;
		float length = angle % 90 == 0 ? 2 : 0.6;

		glPushMatrix();
			glRotatef(angle, 0, 0, 1);
			glTranslatef(0, 5, 0);
			glBegin(GL_LINES);
				glVertex2d(0, 0);
				glVertex2d(0, length);
			glEnd();
		glPopMatrix();
	}
}

class Renderable {
public:
    virtual void render() = 0;
};

class Planet: Renderable
{
public:
    enum TimePart { NONE, SECONDS, MINUTES, HOURS };

    float size;
    TimePart timePart = NONE;

    Planet(float dist, float speed, const float color[3], float size) :
            distFromRef(dist), angularSpeed(speed), size(size)
	{
        this->color[0] = color[0];
        this->color[1] = color[1];
        this->color[2] = color[2];

        angle = randBetween(360);
    }

    void addMoon(const Planet &moon) {
        moons.push_back(moon);
    }

    void update()
    {
		angle += (angularSpeed * TIME_MULTIPLIER);

        for (Planet &moon : moons) {
            moon.update();
        }
    }

    void render() override
    {
        glPushMatrix();
            glRotatef(renderedAngle(), 0, 0, 1);
            if (showTime && timePart != NONE) drawLine();

            glTranslatef(0, distFromRef, 0);
            glRotatef(90, 0, 0, 1);

			// Draw outer glow
            glColor4f(color[0], color[1], color[2], opacity());
			glBegin(GL_TRIANGLE_FAN);
				glVertex2d(0, 0);
				glColor4f(color[0], color[1], color[2], 0);
				for (int i = 0; i <= 20; i++) {
					float angle = (float)i / 10;
					glVertex2d(size * PLANET_GLOW_SIZE * sin(angle * M_PI), 
							   size * PLANET_GLOW_SIZE * cos(angle * M_PI));
				}
			glEnd();

			// Draw the planet itself
			glColor4f(color[0], color[1], color[2], opacity());
            glBegin(GL_TRIANGLE_FAN);
                glVertex2d(0, 0);
                for (int i = 0; i <= 20; i++) {
                    // Draw a semicircle gradient
                    if (i > 10) {
                        float colorDiff = (float) i / 100;
                        glColor4f(color[0] + colorDiff,
                                  color[1] + colorDiff,
                                  color[2] + colorDiff,
                                  opacity());
                    }

                    float angle = (float) i / 10;
                    glVertex2d(size * sin(angle * M_PI), size * cos(angle * M_PI));
                }
            glEnd();

            for (Planet &moon : moons) {
                moon.render();
            }
        glPopMatrix();
    }

    double x() {
        return distFromRef * -sin(toRad(angle));
    }

    double y() {
        return distFromRef * cos(toRad(angle));
    }
private:
    float distFromRef;
    float angularSpeed;
    GLfloat color[3];
    float angle;
    vector <Planet> moons;

    float renderedAngle() {
        if (showTime) {
            time_t current_time = time(NULL);
            struct tm timeinfo;

            #ifdef _WIN32
			    localtime_s(&timeinfo, &current_time);
            #else
                localtime_r(&current_time, &timeinfo);
            #endif

            switch (timePart) {
                case SECONDS:
                    return -360 * ((float) timeinfo.tm_sec / 60);
                case MINUTES:
                    return -360 * ((float) timeinfo.tm_min / 60);
                case HOURS:
                    return -360 * ((float) timeinfo.tm_hour / 12);
                case NONE: break;
            }
        }

        return angle;
    }

    float opacity() {
        return showTime && timePart == NONE ? 0.5f : 1;
    }

    void drawLine() {
        glBegin(GL_LINES);
			glColor3fv(clockColor);
            glVertex2f(0, 0);
            glVertex2f(0, distFromRef);
        glEnd();
    }
};

class Star: Renderable
{
public:
    Star(double x, double y, double size): x(x), y(y), size(size) {}

    void update() {
        switch (state) {
            case STATIC:
				if (randBetween() < 0.0001 * TIME_MULTIPLIER) {
                    state = BLINK_OUT;
                }
                break;

            case BLINK_OUT:
				brightness -= D_STAR_OPACITY * TIME_MULTIPLIER;
                if (brightness < 0.05) {
                    state = BLINK_IN;
                }
                break;

            case BLINK_IN:
				brightness += D_STAR_OPACITY * TIME_MULTIPLIER;
                if (brightness >= MAX_STAR_BRIGHTNESS) {
					brightness = MAX_STAR_BRIGHTNESS;
                    state = STATIC;
                }
        }
    }

    void render() override
    {
        glPushMatrix();
            glTranslated(x, y, 0);
            glColor4f(1, 1, 1, brightness);
            glBegin(GL_POLYGON);
                glVertex2f(0, 0);
                glVertex2f(size, size);
                glVertex2f(0, size * 2);
                glVertex2f(-size, size);
            glEnd();
        glPopMatrix();
    }

private:
    enum StarState { STATIC, BLINK_IN, BLINK_OUT };

    StarState state = STATIC;
    double x;
    double y;
    float size;
    float brightness = MAX_STAR_BRIGHTNESS;
};

class Particle
{
public:
    Particle(double size, double dx, double dy, double dopacity, const float color[3]):
            size(size), dx(dx), dy(dy), dopacity(dopacity)
    {
        for (int i = 0; i < 3; i++) {
            this->color[i] = color[i];
        }
    }

    void render() {
        glBegin(GL_POLYGON);
            glColor4f(color[0], color[1], color[2], opacity);

            glVertex2d(x, y - size);
            glVertex2d(x + size, y);
            glVertex2d(x, y + size);
            glVertex2d(x - size, y);
        glEnd();
    }

    void update() {
        x += dx;
        y += dy;
        opacity -= dopacity;
    }

    bool isTransparent() {
        return opacity <= 0;
    }

private:
    float x = 0;
    float y = 0;

    double dx;
    double dy;
    double dopacity;

    float color[3];
    float opacity = 1;
    double size = 0.05;
};

class ParticleEmitter: Renderable
{
public:
    float x;
    float y;
    float rate;
    float lifetime;

    float angleBegin;
    float angleEnd;
    float speed;
    float size;

    float color[3];
    float dcolor[3];

    bool isActive = true;

    ParticleEmitter() = default;

    void render() override {
        for (unique_ptr<Particle> &p : particles) {
            p->render();
        }
    }

    void update() {
        if (isActive && randBetween() < rate) {
            double angle = randBetween(angleBegin, angleEnd);
            double dopacity = 1 / (lifetime + randBetween(lifetime / 10));
            particles.emplace_back(new Particle(size,
                                                sin(angle) * speed,
                                                cos(angle) * speed,
                                                dopacity,
                                                color));
        }

        for (unique_ptr<Particle> &p : particles) {
            p->update();
        }

        particles.erase(remove_if(particles.begin(),
                                  particles.end(),
                                  [](unique_ptr<Particle> &p){ return p->isTransparent(); }), particles.end());
    }

private:
    vector<unique_ptr<Particle>> particles;
};

class Rocket
{
public:
    Rocket(double x, double y, double vx, double vy):
            x(x), y(y), vx(vx), vy(vy)
    {
        particles->color[0] = 1.0;
        particles->color[1] = 0.922;
        particles->color[2] = 0.231;

        particles->angleBegin = toRad(150);
        particles->angleEnd = toRad(210);
        particles->lifetime = 600;
        particles->rate = 0.05;
        particles->speed = 0.001;
        particles->size = 0.03;
    };

    void update(vector<Planet> planets) {
        // Rocket engines
        bool enginesFiring = mouseDown;
        particles->isActive = enginesFiring;

        if (enginesFiring) {
            double angle = heading();
			vx += DELTA_V * sin(angle);
			vy += DELTA_V * cos(angle);
        }

        // Influence of gravity
        for (Planet &p : planets) {
            gravity(p);
        }

        x += vx;
        y += vy;

        particles->update();
    }

    void render() {
        glPushMatrix();
            glTranslated(x, y, 0);
            glRotated(-toDeg(heading()), 0, 0, 1);
            glColor3f(1, 1, 1);

            glBegin(GL_LINES);
                glVertex2d(0, 0);
                glVertex2d(0, 0.3);
            glEnd();

            particles->render();
        glPopMatrix();
    }

    bool collide(Planet &p) {
        double dx = p.x() - x;
        double dy = p.y() - y;
        return norm(dx, dy) < p.size;
    }

    bool outOfBounds() {
        return x > 11 || x < -11 || y > 11 || y < -11;
    }

    static Rocket* launch(Planet &p) {
        double dx = mouseX - p.x();
        double dy = mouseY - p.y();
        double dist = norm(dx, dy);
        double height = p.size + 0.1;

        double x = p.x() + (dx / dist) * height;
        double y = p.y() + (dy / dist) * height;

		return new Rocket(x, y, (dx / dist) * DELTA_V, (dy / dist) * DELTA_V);
    }

private:
    double x;
    double y;

    double vx;
    double vy;

    unique_ptr<ParticleEmitter> particles = unique_ptr<ParticleEmitter>(new ParticleEmitter());

    double heading() {
        return atan2(mouseX - x, mouseY - y);
    }

    void gravity(Planet &p) {
        double dx = p.x() - x;
        double dy = p.y() - y;
        double magnitude = norm(dx, dy);

        double r2 = dx * dx + dy * dy;
        double f = (G * p.size * p.size) / r2;

        vx += f * (dx / magnitude);
        vy += f * (dy / magnitude);
    }
};

vector<Planet> planetList;
vector<Star> starList;
unique_ptr<Rocket> activeRocket;

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10, 10, -10, 10, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init()
{
	glClearColor (0.0, 0.0, 0.3, 1.0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Init planets
    Planet *p;
    Planet *m;

	const float purple[] = { 0.671, 0.278, 0.737 };
	const float red[] = { 0.898, 0.224, 0.208 };
	const float limeGreen[] = { 0.831, 0.882, 0.341 };
	const float blue[] = { 0.392, 0.71, 0.965 };
	const float blueMoon[] = { 0.891, 0.965, 0.965 };
	const float green[] = { 0.149, 0.651, 0.604 };
	const float greenMoon[] = { 0.549, 0.651, 0.604 };
	const float brightYellow[] = { 1.0, 0.839, 0.0 };

	p = new Planet(8.5, 0.00126, green, 0.23);
		p->timePart = Planet::SECONDS;
		m = new Planet(0.6, -0.03f, greenMoon, 0.1);
		p->addMoon(*m);
		planetList.push_back(*p);

	p = new Planet(6.8, 0.00172, blue, 0.8);
		p->timePart = Planet::SECONDS;
		m = new Planet(1.0, 0.0413, blueMoon, 0.16);
		p->addMoon(*m);
		m = new Planet(1.4, -0.0149f, blueMoon, 0.13);
		p->addMoon(*m);
		planetList.push_back(*p);

	p = new Planet(5.2, -0.003f, limeGreen, 0.45);
		p->timePart = Planet::MINUTES;
		planetList.push_back(*p);

	p = new Planet(4, 0.00183, red, 0.6);
		p->timePart = Planet::MINUTES;
		planetList.push_back(*p);

    p = new Planet(2.4, -0.0007f, purple, 0.3);
        p->timePart = Planet::HOURS;
        planetList.push_back(*p);

    // Sun
    p = new Planet(0, 0, brightYellow, 1.3);
        planetList.push_back(*p);

    // Init stars
    for (int i = 0; i < numStars; i++) {
        auto *s = new Star(randBetween(-10, 10), randBetween(-10, 10), randBetween(0.05, 0.08));
        starList.push_back(*s);
    }
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();

	//controls transformation
	glScalef(k, k, k);
	glTranslatef(tx, ty, 0);	
	glRotatef(alpha, 0, 0, 1);

	//draw stuff here!
    for (Star &s: starList) {
        s.render();
    }

    for (Planet &p : planetList) {
        p.render();
    }

    if (activeRocket) {
        activeRocket->render();
    }

	if (showTime) {
		drawClockFace();
	}

	glPopMatrix();
	glFlush ();
}

void idle()
{
	//update animation here
    for (Star &s: starList) {
        s.update();
    }

    for (Planet &p : planetList) {
        p.update();
    }

    if (activeRocket) {
        activeRocket->update(planetList);

        bool outOfBounds = activeRocket->outOfBounds();
        bool collidedWithPlanet = any_of(planetList.begin(), planetList.end(), [](Planet p) { return activeRocket->collide(p); });

        if (outOfBounds || collidedWithPlanet) {
            activeRocket.reset();
        }
    }
	
	glutPostRedisplay();	//after updating, draw the screen again
}

void mouseMove(int x, int y)
{
    int stageWidth = glutGet(GLUT_WINDOW_WIDTH);
    int stageHeight = glutGet(GLUT_WINDOW_HEIGHT);

    mouseX = ((float) x - (stageWidth / 2)) * 20/ stageWidth;
    mouseY = ((stageHeight / 2) - (float) y) * 20 / stageHeight;
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        mouseDown = state == GLUT_DOWN;
        mouseMove(x, y);

        if (!activeRocket) {
			activeRocket = unique_ptr<Rocket>(Rocket::launch(planetList.at(LAUNCH_PLANET)));
        }
    }
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

        case 't':
            showTime = !showTime;
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
	cout<<"CS3241 Lab 2\n\n";
	cout<<"+++++CONTROL BUTTONS+++++++\n\n";
	cout<<"Scale Up/Down: Q/E\n";
	cout<<"Rotate clockwise/anticlockwise: A/D\n";
	cout<<"Move Up/Down: W/S\n";
	cout<<"Move Left/Right: Z/C\n";
	cout<<"Toggle clock: T\n";
	cout<<"Launch/Aim rocket: Left Mouse\n";
	cout <<"ESC: Quit\n";

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (600, 600);
	glutInitWindowPosition (50, 50);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMove);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
