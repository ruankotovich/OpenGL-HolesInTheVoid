#pragma once
#include "glutils.hpp"
#include "objects.hpp"
#include <thread>
#include <vector>

static void ftl(int x);
// static void playFTL() { system("play ~/Desktop/rocket.wav"); }

struct GameLoop {
    Spaceship spaceship;
  
    std::vector<Moveable*> moveableObjects;
    std::vector<Drawnable*> drawnableObjects;

    std::vector<bool> keys;
    std::vector<bool> keysToggle;
    std::vector<Star> stars;

    GameLoop()
        : keys(256, false)
        , keysToggle(256, false)
    {

        moveableObjects.push_back(&spaceship);
        addStars();
    }

    void addStars()
    {
        stars = std::vector<Star>(50);
        for (auto& star : stars) {
            drawnableObjects.push_back(&star);
        }
    }

    void dismissStars()
    {
        for (auto& star : stars) {
            star.showing = false;
        }
    }

    void restartStars()
    {
        for (auto& star : stars) {
            star.showing = true;
            star.organize();
        }
    }

    void draw()
    {

        for (auto& drawnable : drawnableObjects) {
            glPushMatrix();
            drawnable->performActions();
            drawnable->draw();
            glPopMatrix();
        }

        for (auto& moveable : moveableObjects) {
            moveable->performActions();
            glPushMatrix();
            glTranslatef(moveable->locationX, moveable->locationY, 0);
            glRotatef(moveable->angle, 0, 0, 1.0f);
            moveable->draw();
            glPopMatrix();
        }

        // glPushMatrix();
        // int H = 100;
        // int W = 100;
        // unsigned int data[H][W][3];
        // for (size_t y = 0; y < H; ++y) {
        //     for (size_t x = 0; x < W; ++x) {
        //         data[y][x][0] = (rand() % 256) * 256 * 256 * 256;
        //         data[y][x][1] = (rand() % 256) * 256 * 256 * 256;
        //         data[y][x][2] = (rand() % 256) * 256 * 256 * 256;
        //     }
        // }

        // glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_INT, data);

        // glPopMatrix();
    }

    bool interceptAction()
    {
        bool nointerrupt = true;

        for (unsigned int i = 0; i < keysToggle.size(); i++) {

            switch (i) {

            case 32:
                spaceship.turbo = keysToggle[i];
                break;
            }
        }

        for (unsigned int i = 0; i < keys.size(); i++) {

            if (!keys[i]) {
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
            case 'f':
                if (spaceship.movementState == MovementState::STOPPED) {
                    _setBgColor(0.f, 0.f, 0.f);
                    dismissStars();
                    spaceship.setMovementState(MovementState::FTL);
                    nointerrupt = false;
                    glutTimerFunc(0, ftl, 0);
                }
                break;
            case 'b':
                spaceship.burn();
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
    gameLoop.keysToggle[key] = !gameLoop.keysToggle[key];
    gameLoop.keys[key] = true;
}

static void keyboardUpParser(unsigned char key, int x, int y)
{
    gameLoop.keys[key] = false;
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
        glClearColor(0.01f, 0.035f, .055f, 1.0f);
        gameLoop.restartStars();
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
        gameLoop.spaceship.locationX = gameLoop.spaceship.locationY = 0;
        glutTimerFunc(MS, firstIdle, 0);
    }
}

void initWindowMode()
{
    glutFullScreen();
}

struct Game {
    void start(int argc, char** argv)
    {
        glutInit(&argc, argv); // Initialize GLUT
        glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
        glutInitWindowSize(640, 480); // Set the window's initial width & height - non-square
        glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
        glutCreateWindow("Animation via Idle Function"); // Create window with the given title
        glutReshapeFunc(_reshape); // Register callback handler for window re-size event

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_POLYGON_SMOOTH);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        glDisable(GL_DEPTH_TEST);
        // glEnable(GL_BLEND);

        glutDisplayFunc(loop); // Register callback handler for window re-paint event
        aRatio = .0005;
        glutTimerFunc(0, firstIdle, 0); // First timer call immediately
        glutKeyboardFunc(keyboardParser);
        glutKeyboardUpFunc(keyboardUpParser);
        initWindowMode();
        _initGL(); // Our own OpenGL initialization
        glutMainLoop(); // Enter the infinite event-processing loop
    }
};
