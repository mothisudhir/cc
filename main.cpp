#include <windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>

// Struct to represent star objects
struct StarFieldObject {
    GLfloat X, Y, Z;
};
std::vector<StarFieldObject> stars;

// Struct to represent asteroid objects
struct AsteroidObject {
    GLfloat radius;
    GLfloat orbitRadius;  // Distance from the Sun
};
std::vector<AsteroidObject> asteroids;

static float HourOfDay = 0.0;
static float DayOfYear = 0.0;
static float AnimateIncrement = 0.00001;  // Speed factor for the animation

GLfloat angleX = 0.0, angleY = 0.0;   // Angles to rotate the solar system
GLfloat lastX = -1.0, lastY = -1.0;   // Last mouse positions
bool isDragging = false;              // Mouse dragging status

GLfloat zoom = -20.0;                 // Zoom level (distance from camera)
GLfloat moveX = 0.0, moveY = 0.0;    // Movement offsets

// Revolution periods for planets (in Earth years)
const float mercuryYear = 88.0 / 365.0;
const float venusYear = 225.0 / 365.0;
const float earthYear = 1.0;
const float marsYear = 687.0 / 365.0;
const float jupiterYear = 11.86;
const float saturnYear = 29.46;
const float uranusYear = 84.0;
const float neptuneYear = 164.8;
const float moonOrbitPeriod = 1.0 / 365.0;  // Moon orbits Earth 365 times per Earth year

// Generate random stars
void GenerateStars(int numStars) {
    stars.clear();  // Clear any previously generated stars
    for (int i = 0; i < numStars; i++) {
        StarFieldObject star;
        star.X = (rand() % 800 - 400) / 10.0f;  // Increased range for X
        star.Y = (rand() % 800 - 400) / 10.0f;  // Increased range for Y
        star.Z = (rand() % 800 - 400) / 10.0f;  // Increased range for Z
        stars.push_back(star);
    }
}

// Generate asteroids in the asteroid belt
void GenerateAsteroids(int numAsteroids) {
    asteroids.clear();  // Clear any previous asteroids
    for (int i = 0; i < numAsteroids; i++) {
        AsteroidObject asteroid;
        asteroid.orbitRadius = 10.0f + (rand() % 301) / 100.0f; // Random distance between 10 and 13
        asteroid.radius = 0.05f;  // Small size for asteroids
        asteroids.push_back(asteroid);
    }
}

// Render stars in the background
void RenderStars() {
    glPointSize(2.0);
    glBegin(GL_POINTS);
    for (unsigned int i = 0; i < stars.size(); i++) {
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(stars[i].X, stars[i].Y, stars[i].Z);
    }
    glEnd();
}

// Render asteroid belt
void RenderAsteroids() {
    for (unsigned int i = 0; i < asteroids.size(); i++) {
        glPushMatrix();
        float angle = (DayOfYear / 365.0) * 360.0 + i * (360.0 / asteroids.size()); // Calculate angle based on DayOfYear and index
        float x = asteroids[i].orbitRadius * cos(angle * M_PI / 180.0);
        float z = asteroids[i].orbitRadius * sin(angle * M_PI / 180.0);
        glTranslatef(x, 0.0, z);
        glColor3f(0.5, 0.5, 0.5);
        glutSolidSphere(asteroids[i].radius, 8, 8);
        glPopMatrix();
    }
}

// Render the planet names
void DrawText(const char* text, float x, float y, float z) {
    glRasterPos3f(x, y, z);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *text);
        text++;
    }
}

// Handle mouse press events
void MousePress(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isDragging = true;
        lastX = x;
        lastY = y;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        isDragging = false;
    }
}

// Handle mouse drag events for rotating the solar system
void MouseDrag(int x, int y) {
    if (isDragging) {
        float dx = (x - lastX) * 0.2f;
        float dy = (y - lastY) * 0.2f;

        angleX += dy;
        angleY += dx;

        lastX = x;
        lastY = y;

        glutPostRedisplay();
    }
}

// Keyboard input handling
void KeyboardInput(unsigned char key, int x, int y) {
    switch (key) {
        case 'z':
        case 'Z':
            zoom += 0.5;
            break;
        case 'c':
        case 'C':
            zoom -= 0.5;
            break;
        case 'w':
        case 'W':
            moveY += 0.5;
            break;
        case 's':
        case 'S':
            moveY -= 0.5;
            break;
        case 'a':
        case 'A':
            moveX -= 0.5;
            break;
        case 'd':
        case 'D':
            moveX += 0.5;
            break;
    }
    glutPostRedisplay();
}

// Render the solar system animation
void Animate() {
    HourOfDay += AnimateIncrement;
    DayOfYear += AnimateIncrement / 24.0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Apply zoom and rotation from mouse interaction
    glTranslatef(moveX, moveY, zoom);  // Adjust position based on movement and zoom
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);

    RenderStars();  // Stars in the background

    glPushMatrix();  // Save the current transformation matrix
    glRotatef(360.0 * (DayOfYear / 365.0), 0.0, 1.0, 0.0);  // Rotate the asteroid belt around the Sun
    RenderAsteroids();  // Asteroid belt
    glPopMatrix();  // Restore the previous transformation matrix

    // Draw the Sun
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glutSolidSphere(0.7, 20, 16);
    DrawText("Sun", 0.0, 0.8, 0.0);
    glPopMatrix();

    // Mercury
    glPushMatrix();
    glRotatef(360.0 * (DayOfYear / mercuryYear), 0.0, 1.0, 0.0);
    glTranslatef(3.0, 0.0, 0.0);
    glColor3f(0.5, 0.5, 0.5);
    glutSolidSphere(0.2, 16, 16);
    DrawText("Mercury", 0.0, 0.3, 0.0);
    glPopMatrix();

    // Venus
    glPushMatrix();
    glRotatef(360.0 * (DayOfYear / venusYear), 0.0, 1.0, 0.0);
    glTranslatef(5.0, 0.0, 0.0);
    glColor3f(1.0, 0.5, 0.0);
    glutSolidSphere(0.3, 16, 16);
    DrawText("Venus", 0.0, 0.35, 0.0);
    glPopMatrix();

    // Earth
    glPushMatrix();
    glRotatef(360.0 * (DayOfYear / earthYear), 0.0, 1.0, 0.0);
    glTranslatef(7.0, 0.0, 0.0);
    glColor3f(0.0, 0.0, 1.0 );
    glutSolidSphere(0.4, 16, 16);
    DrawText("Earth", 0.0 , 0.45, 0.0);

    // Moon around Earth
    glPushMatrix();
    glRotatef(360.0 * (DayOfYear / moonOrbitPeriod), 0.0, 1.0, 0.0);
    glTranslatef(0.7, 0.0, 0.0);
    glColor3f(0.8, 0.8, 0.8);
    glutSolidSphere(0.1, 10, 10);
    DrawText("Moon", 0.0, 0.15, 0.0);
    glPopMatrix();  //QZ End of Moon
    glPopMatrix();  // End of Earth

    // Mars
    glPushMatrix();
    glRotatef(360.0 * (DayOfYear / marsYear), 0.0, 1.0, 0.0);
    glTranslatef(10.0, 0.0, 0.0);
    glColor3f(1.0, 0.0, 0.0);
    glutSolidSphere(0.3, 16, 16);
    DrawText("Mars", 0.0, 0.35, 0.0);
    glPopMatrix();

    // Jupiter
    glPushMatrix();
    glRotatef(360.0 * (DayOfYear / jupiterYear), 0.0, 1.0, 0.0);
    glTranslatef(13.0, 0.0, 0.0);
    glColor3f(0.9, 0.6, 0.0);
    glutSolidSphere(0.7, 16, 16);
    DrawText("Jupiter", 0.0, 0.75, 0.0);
    glPopMatrix();

    // Saturn
    glPushMatrix();
    glRotatef(360.0 * (DayOfYear / saturnYear), 0.0, 1.0, 0.0);
    glTranslatef(17.0, 0.0, 0.0);
    glColor3f(0.9, 0.7, 0.3);
    glutSolidSphere(0.6, 16, 16);
    DrawText("Saturn", 0.0, 0.7, 0.0);

    // Saturn's rings
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glColor3f(0.8, 0.7, 0.6);
    glutSolidTorus(0.05, 0.9, 16, 16);
    glPopMatrix();

    // Uranus
    glPushMatrix();
    glRotatef(360.0 * (DayOfYear / uranusYear), 0.0, 1.0, 0.0);
    glTranslatef(21.0, 0.0, 0.0);
    glColor3f(0.6, 0.8, 0.9);
    glutSolidSphere(0.5, 16, 16);
    DrawText("Uranus", 0.0, 0.55, 0.0);
    glPopMatrix();

    // Neptune
    glPushMatrix();
    glRotatef(360.0 * (DayOfYear / neptuneYear), 0.0, 1.0, 0.0);
    glTranslatef(25.0, 0.0, 0.0);
    glColor3f(0.2, 0.3, 0.9);
    glutSolidSphere(0.45, 16, 16);
    DrawText("Neptune", 0.0, 0.5, 0.0);
    glPopMatrix();

    glutSwapBuffers();  // Double buffer display
}

void Initialize() {
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GenerateStars(100);  // Generate 100 stars
    GenerateAsteroids(150);  // Generate 150 asteroids in the belt
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1910, 1070);
    glutCreateWindow("Solar System with Asteroid Belt");

    Initialize();

    glutDisplayFunc(Animate);
    glutMouseFunc(MousePress);
    glutMotionFunc(MouseDrag);
    glutKeyboardFunc(KeyboardInput);
    glutIdleFunc(Animate);

    glutMainLoop();
    return 0;
}
