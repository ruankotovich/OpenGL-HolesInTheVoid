#pragma once
#include "glutils.hpp"
#include "objects.hpp"
#include <thread>
#include <vector>

static void ftl(int x);
static void playFTL() { system("play ~/Desktop/rocket.wav"); }


struct GameLoop {
    Spaceship spaceship;
    std::vector<Moveable*> moveableObjects;

    std::vector<char> keys;
    std::vector<char> keysAction;

    GameLoop()
        : keys(256, 0)
    {
        moveableObjects.push_back(&spaceship);
    }

    void draw()
    {
        for (auto& moveable : moveableObjects) {
            moveable->performActions();
            glPushMatrix();
            glTranslatef(moveable->locationX, moveable->locationY, 0);
            glRotatef(moveable->angle, 0, 0, 1.0f);
            moveable->draw();
            glPopMatrix();
        }
    }

    bool interceptAction()
    {
        bool nointerrupt = true;
        for (unsigned int i = 0; i < keys.size(); i++) {

            if (keys[i] == 0) {
                continue;
            }

            switch (i) {
            case 'w':
                if (spaceship.speed < spaceship.maxSpeed) {
                    if (spaceship.movementState == MovementState::MOVING) {
                        spaceship.speed += spaceship.acceleration;
                    } else {
                        spaceship.speed = spaceship.acceleration + spaceship.minSpeed;
                        spaceship.setMovementState(MovementState::MOVING);
                    }
                }
                break;
            case 'a':
                if (spaceship.rotationSpeed < spaceship.maxRotationSpeed) {
                    spaceship.rotationSpeed += spaceship.rotationAcceleration;
                }
                break;
            case 'd':
                if (spaceship.rotationSpeed > -spaceship.maxRotationSpeed) {
                    spaceship.rotationSpeed -= spaceship.rotationAcceleration;
                }
                break;
            case 's':
                spaceship.setMovementState(MovementState::STOPPING);
                break;
            case '+':
                aRatio -= 0.1;
                break;
            case '-':
                aRatio += 0.1;
                break;
            case 32:

                spaceship.toggleTurbo();

                break;
            case 'f':
                if (spaceship.movementState == MovementState::STOPPED) {
                    spaceship.setMovementState(MovementState::FTL);
                    nointerrupt = false;
                    glutTimerFunc(0, ftl, 0);
                    // new std::thread(playFTL);
                }
                break;
            }
        }

        return nointerrupt;
    }
} gameLoop;

static void loop()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gameLoop.draw();

    glutSwapBuffers();
}

static void keyboardParser(unsigned char key, int x, int y)
{
    gameLoop.keys[key] = 1;
}

static void keyboardUpParser(unsigned char key, int x, int y)
{
    gameLoop.keys[key] = 0;
}

static void idle(int time)
{
    if (gameLoop.interceptAction()) {
        glutPostRedisplay();
        glutTimerFunc(MS, idle, 0);
    }
}

static float incrementFI = 0.005;
static float incrementFTL = 0.05;

static void firstIdle(int time)
{
    glutPostRedisplay();
    if (aRatio < 1) {
        aRatio += (incrementFI + 0.00005);
        glutTimerFunc(MS, firstIdle, 0);
    } else {
        gameLoop.spaceship.setMovementState(MovementState::STOPPED);
        aRatio = 1;
        glutTimerFunc(MS, idle, 0);
    }
}

static void ftl(int x)
{
    glutPostRedisplay();
    if (aRatio < 8) {
        aRatio += (incrementFTL + 0.005);
        glutTimerFunc(MS, ftl, 0);
    } else {
        aRatio = .00005;
        glutTimerFunc(MS, firstIdle, 0);
    }
}

struct HaloKeeper {
    void start(int argc, char** argv)
    {
        glutInit(&argc, argv); // Initialize GLUT
        glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
        glutInitWindowSize(640, 480); // Set the window's initial width & height - non-square
        glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
        glutCreateWindow("Animation via Idle Function"); // Create window with the given title
        glutReshapeFunc(_reshape); // Register callback handler for window re-size event
        glutDisplayFunc(loop); // Register callback handler for window re-paint event
        aRatio = .0005;
        glutTimerFunc(0, firstIdle, 0); // First timer call immediately
        glutKeyboardFunc(keyboardParser);
        glutKeyboardUpFunc(keyboardUpParser);
        glutFullScreen();
        _initGL(); // Our own OpenGL initialization
        glutMainLoop(); // Enter the infinite event-processing loop
    }
};