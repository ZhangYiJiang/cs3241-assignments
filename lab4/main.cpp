// CS3241Lab4.cpp : Defines the entry point for the console application.
//#include <cmath>
#include "math.h"
#include <iostream>
#include <fstream>

/* Include header files depending on platform */

#ifdef _WIN32
#include <Windows.h>
#include "glut.h"
#define M_PI 3.141592654
#elif __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#elif __linux__

#include <GL/glut.h>
#include <vector>

#endif

#define MAXPTNO 1000
#define NLINESEGMENT 32
#define NOBJECTONCURVE 8

using namespace std;

// Global variables that you can use
int mouseX, mouseY;

const GLfloat BLACK[3] = { 0, 0, 0 };
const GLfloat GREEN[3] = { 0, 1, 0 };
const GLfloat RED[3] = { 0.83, 0.18, 0.18 };
const GLfloat PURPLE[3] = { 0.61, 0.15, 0.69 };
const GLfloat GRAY[3] = { 0.7, 0.7, 0.7 };
const GLfloat ORANGE[3] = { 1.0, 0.56, 0.0 };

const string QUOTE = "Be not afraid of greatness: some are born great, some achieve greatness and some have greatness thrust upon them";
const double MIN_DISTANCE = 40;
const double MAX_DISTANCE = 120;

struct Point {
    double x, y;
};

std::ostream &operator<<(std::ostream &os, Point p) { // Make Point cout'able
    return os << '(' << p.x << ", " << p.y << ')';
}

// Storage of control points
int nPt = 0;
vector<Point> ptList;

// Display options
bool displayControlPoints = true;
bool displayControlLines = true;
bool displayTangentVectors = false;
bool displayObjects = false;
bool C1Continuity = false;

void drawRightArrow(int i) {
    glColor3fv(ORANGE);

    glBegin(GL_LINE_STRIP);
    glVertex2f(0, 0);
    glVertex2f(100, 0);
    glVertex2f(95, 5);
    glVertex2f(100, 0);
    glVertex2f(95, -5);
    glEnd();
}

void drawTextCharacter(int i) {
    glColor3fv(BLACK);
    glPushMatrix();
        glScaled(0.2, -0.2, 1);
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, QUOTE[i % QUOTE.size()]);
    glPopMatrix();
}

// Bezier curve calculation functions
Point interpolate(Point p1, Point p2, double p) {
    Point px{
        p1.x + (p2.x - p1.x) * p,
        p1.y + (p2.y - p1.y) * p,
    };

    return px;
}

Point interpolate(vector<Point> points, double p) {
    // Repeatedly interpolate pairs of points, reducing the number of points by one
    // each iteration
    vector<Point> nextPoints;
    while (points.size() > 2) {
        for (int i = 0; i < points.size() - 1; i++) {
            points[i];
            nextPoints.push_back(interpolate(points[i], points[i + 1], p));
        }

        points = nextPoints;
        nextPoints.clear();
    }

    return interpolate(points[0], points[1], p);
}

bool isC1ControlPt(int index, long total) {
    long segments = (total - 1) / 3;
    if (index - (segments * 3) > 0) return false;
    return index != 1 && (index - 1) % 3 == 0;
}

void drawOnLine(int i, Point p1, Point p2, void (*draw)(int), bool scaleToMouse = false) {
    glPushMatrix();
        glTranslated(p1.x, p1.y, 0);
        glRotated(180 * (atan2(p2.y - p1.y, p2.x - p1.x) / M_PI), 0, 0, 1);

        // Inversely interpolate size of object with the distance to mouse
        if (scaleToMouse) {
            double dx = mouseX - p1.x;
            double dy = mouseY - p1.y;
            double distance = sqrt(dx * dx + dy * dy);

            double scale;
            if (distance < MIN_DISTANCE) {
                scale = 1;
            } else if (distance > MAX_DISTANCE) {
                scale = 0;
            } else {
                scale = 1 - (distance - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE);
            }

            glScaled(scale * scale, scale * scale, 1);
        }

        draw(i);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glPointSize(5);

    // Make a copy of the original points so we can modify it for (Q4) C1 continuity
    vector<Point> drawnPoints = std::vector<Point>(ptList);

    // Q4: C1 continuity - if C1 continuity is enabled, modify all affected points
    //                     to their new location
    if (C1Continuity) {
        int i = 3;
        while (i + 3 < drawnPoints.size()) {
            drawnPoints[i + 1] = interpolate(drawnPoints[i - 1], drawnPoints[i], 2);
            i += 3;
        }

        glEnd();
    }

    if (displayControlPoints) {
        glBegin(GL_POINTS);

        for (int i = 0; i < drawnPoints.size(); i++) {
            if (C1Continuity && isC1ControlPt(i, drawnPoints.size())) {
                // Draw both the original and the new point and color the new point red
                // if C1 is enabled
                glColor3fv(GRAY); // Gray
                glVertex2d(ptList[i].x, ptList[i].y);
                glColor3fv(RED); // Red
            } else {
                // Otherwise the point is black
                glColor3fv(BLACK); // Black
            }

            glVertex2d(drawnPoints[i].x, drawnPoints[i].y);
        }

        glEnd();
    }

    // Q1: Drawing control lines
    if (displayControlLines) {
        glColor3fv(GREEN);

        glBegin(GL_LINE_STRIP);
        for (Point p : drawnPoints) {
            glVertex2d(p.x, p.y);
        }
        glEnd();
    }

    // Generate bezier curve vertices
    int i = 0;
    vector<Point> vertices;
    while (i + 3 < drawnPoints.size()) {
        vector<Point> segment = vector<Point>(drawnPoints.begin() + i, drawnPoints.begin() + i + 4);

        for (int j = 0; j <= NLINESEGMENT; j++) {
            double t = (double) j / (NLINESEGMENT);
            vertices.push_back(interpolate(segment, t));
        }

        i += 3;
    }

    // Q3: Draw bezier curve
    glBegin(GL_LINE_STRIP);
    glColor3fv(PURPLE); // Purple
    for (Point v : vertices) {
        glVertex2d(v.x, v.y);
    }
    glEnd();

    // Q5 and Q6: Draw tangents / objects
    if (displayTangentVectors || displayObjects) {
        // List slicing algorithm taken from https://stackoverflow.com/a/2135920/313758
        int n = NLINESEGMENT + 1;
        int k = n / (NOBJECTONCURVE);
        int m = n % (NOBJECTONCURVE);

        for (int c = 0; c < vertices.size() / n; c++) {
            for (i = 0; i < NOBJECTONCURVE; i++) {
                int j = c * NOBJECTONCURVE + i;
                Point v1 = vertices[c * n + i * k + min(i, m)];
                Point v2 = vertices[c * n + i * k + min(i, m) + 1];
                if (displayTangentVectors) drawOnLine(j, v1, v2, drawRightArrow);
                if (displayObjects) drawOnLine(j, v1, v2, drawTextCharacter, true);
            }
        }
    }

    glPopMatrix();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

void readFile() {

    std::ifstream file;
    file.open("savefile.txt");
    file >> nPt;

    ptList.clear();

    for (int i = 0; i < nPt; i++) {
        Point p{};
        file >> p.x;
        file >> p.y;
        ptList.push_back(p);
    }

    file.close();// is not necessary because the destructor closes the open file by default
}

void writeFile() {
    std::ofstream file;
    file.open("savefile.txt");
    file << ptList.size() << endl;

    for (Point p : ptList) {
        file << p.x << " ";
        file << p.y << endl;
    }
    file.close();// is not necessary because the destructor closes the open file by default
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'r':
        case 'R':
            readFile();
            break;

        case 'w':
        case 'W':
            writeFile();
            break;

        case 'T':
        case 't':
            displayTangentVectors = !displayTangentVectors;
            break;

        case 'o':
        case 'O':
            displayObjects = !displayObjects;
            break;

        case 'p':
        case 'P':
            displayControlPoints = !displayControlPoints;
            break;

        case 'L':
        case 'l':
            displayControlLines = !displayControlLines;
            break;

        case 'C':
        case 'c':
            C1Continuity = !C1Continuity;
            break;

        case 'e':
        case 'E':
            // Q2: Erase all the control points added
            ptList.clear();
            break;

        case 'z':
        case 'Z':
            ptList.pop_back();
            break;

        case 27:
            exit(0);
            break;

        default:
            break;
    }

    glutPostRedisplay();
}


void mouse(int button, int state, int x, int y) {
    /*button: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON */
    /*state: GLUT_UP or GLUT_DOWN */
    enum {
        MOUSE_LEFT_BUTTON = 0,
        MOUSE_MIDDLE_BUTTON = 1,
        MOUSE_RIGHT_BUTTON = 2,
        MOUSE_SCROLL_UP = 3,
        MOUSE_SCROLL_DOWN = 4
    };

    if ((button == MOUSE_LEFT_BUTTON) && (state == GLUT_UP)) {
        ptList.push_back({(double) x, (double) y});
        nPt++;

        glutPostRedisplay();

    }
}

void mouseMove(int x, int y) {
    mouseX = x;
    mouseY = y;

    glutPostRedisplay();
}

int main(int argc, char **argv) {
    cout << "CS3241 Lab 4" << endl << endl;
    cout << "Left mouse click: Add a control point" << endl;
    cout << "ESC: Quit" << endl;
    cout << "P: Toggle displaying control points" << endl;
    cout << "L: Toggle displaying control lines" << endl;
    cout << "E: Erase all points (Clear)" << endl;
    cout << "C: Toggle C1 continuity" << endl;
    cout << "T: Toggle displaying tangent vectors" << endl;
    cout << "O: Toggle displaying objects" << endl;
    cout << "R: Read in control points from \"savefile.txt\"" << endl;
    cout << "W: Write control points to \"savefile.txt\"" << endl;
    cout << "Z: Erase last control point" << endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("CS3241 Assignment 4");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(mouseMove);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}
