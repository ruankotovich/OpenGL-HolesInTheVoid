#pragma once
#include <GL/glut.h>
#include <cmath>
#define PI 3.14159265
#define MS 15
#define BOUNDARY 1.1

static float aRatio = 1;
static float mDensity = 2.05f;

static float lxBoundary = -1;
static float rxBoundary = 1;
static float uyBoundary = 1;
static float byBoundary = -1;

float degrees2rad(float _degrees)
{
    return _degrees * PI / 180;
}

void _reshape(GLsizei width, GLsizei height)
{ // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0)
        height = 1; // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
    glLoadIdentity(); // Reset the projection matrix
    if (width >= height) {
        // aspect >= 1, set the height from -1 to 1, with larger width
        gluOrtho2D(lxBoundary = (-1.0 * aspect), rxBoundary = (1.0 * aspect), byBoundary = -1.0, uyBoundary = 1.0);
    } else {
        // aspect < 1, set the width to -1 to 1, with larger height
        gluOrtho2D(lxBoundary = -1.0, rxBoundary = 1.0, byBoundary = (-1.0 / aspect), uyBoundary = (1.0 / aspect));
    }

    lxBoundary -= 0.1;
    rxBoundary += 0.1;
    uyBoundary += 0.1;
    byBoundary -= 0.1;
}

void _glAspectVertex2f(GLfloat _w, GLfloat _k)
{
    glVertex2f(_w / aRatio, _k / aRatio);
}

void _initGL()
{
    // Set "clearing" or background color
    glClearColor(0.01f, 0.0f, .05f, 1.0f);
    glMatrixMode(GL_PROJECTION); // Select the Projection matrix for operation
    glLoadIdentity(); // Reset Projection matrix
    // glutFullScreen();
}
