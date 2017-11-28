#pragma once
#include "glutils.hpp"
#include "objects.hpp"
#include <fstream>
#include <thread>
#include <unordered_map>
#include <iostream>
#include <vector>


#define THREESOME_CHANNEL 256* 256* 256

static void ftl(int x);

const void* font=GLUT_BITMAP_9_BY_15;

bool ftl_happening = false;

std::vector<std::string> texts = {
    "F T L    S U C C E S S F U L L Y    P E R F O R M E D.",
    "C O N N E C T I O N   L O S T",
    "Do you chupa cu?",
    "Dobry' uta kakashka, xexexexexe!",
    "Prost!"
};

struct GameLoop {
    Spaceship spaceship;
    UnknownMan man;
    Planet planet;
    size_t H = 100;
    size_t W = 100;

    std::vector<Moveable*> moveableObjects;
    std::vector<Drawnable*> drawnableObjects;

    std::vector<bool> keys;
    std::vector<bool> keysToggle;
    std::vector<Star> stars;
    bool toClean = false;
            
    GameLoop()
        : keys(256, false)
        , keysToggle(256, false)
    {
          addStars();
        drawnableObjects.push_back(&man);
        drawnableObjects.push_back(&planet);
        moveableObjects.push_back(&spaceship);
    }

    // void addCharToBanner(int x, int y, char c){
    //     int toX = x + 5;
    //     int toY = y + 5;
    //     auto &matrix = charmap[c];
    //     for(int i=0;i<5;i++, y++){
    //         for(int j=0;j<5;j++,x++){
    //             banner[y][x] = matrix[j][i] != '@';
    //         }
    //     }
    // }

    void addStars()
    {
        stars = std::vector<Star>(30);
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

    void clean(){
        toClean = true;
    }

    void draw()
    {  

        if(!ftl_happening){
            displayText(lxBoundary+.50,byBoundary+.20,255,255,255,texts[0].c_str());   
            
            glPushMatrix();
            glLoadIdentity();
            glRasterPos2f(lxBoundary+0.10, byBoundary+0.10);
            unsigned int data[H][W][3];
            
            for (size_t y = 0; y < H; ++y) {
                    for (size_t x = 0; x < W; ++x) {
                        data[y][x][0] = hexToColor(255);
                        data[y][x][1] = hexToColor(0);
                        data[y][x][2] = hexToColor(0);
                    }   
                }
            
            glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_INT, data);
            glPopMatrix();


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
        }else{
            displayText(lxBoundary+.50,byBoundary+.20,255,255,255,texts[1].c_str());   
            glPushMatrix();
            glLoadIdentity();
            glRasterPos2f(lxBoundary+0.10, byBoundary+0.10);
            unsigned int data[H][W][3];
            
            for (size_t y = 0; y < H; ++y) {
                    for (size_t x = 0; x < W; ++x) {
                        data[y][x][0] = hexToColor(rand() % 256);
                        data[y][x][1] = hexToColor(rand() % 256);
                        data[y][x][2] = hexToColor(rand() % 256);
                    }   
                }
            
            glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_INT, data);
            glPopMatrix();


                spaceship.performActions();
                glPushMatrix();
                glTranslatef(spaceship.locationX, spaceship.locationY, 0);
                glRotatef(spaceship.angle, 0, 0, 1.0f);
                spaceship.draw();
                glPopMatrix(); 
        }


    }



    long hexToColor(char hexCode){
        return hexCode * THREESOME_CHANNEL;
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
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
                    _setBgColor(0.f, 0.f, 0.f);
                    dismissStars();
                    spaceship.setMovementState(MovementState::FTL);
                    nointerrupt = false;
                    ftl_happening = true;
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
        ftl_happening = false;
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
