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

int textPosition = 1;
unsigned long text_timestamp = 0;

std::vector<std::string> texts = {
    "",
    "What do you think are you doing?",
    "Do you chupa cu?",
    "Dobry' uta kakashka, xexexexexe!",
    "Prost!"
};

struct GameLoop {
    Spaceship spaceship;
    UnknownMan man;
    // Planet planet;
    size_t H = 100;
    size_t W = 100;

    std::vector<Moveable*> moveableObjects;
    std::vector<Drawnable*> drawnableObjects;

    std::vector<bool> keys;
    std::vector<bool> keysToggle;
    std::vector<Star> stars;
    bool toClean = false;
    // std::unordered_map<int, std::unordered_map<int, bool>> banner;

    // std::unordered_map<char,char[5][5]> charmap;
            
            
    GameLoop()
        : keys(256, false)
        , keysToggle(256, false)
    {
        // std::ifstream filestream("charmap.cm");

        // std::string line;
        
        // while(std::getline(filestream,line)){
        //     auto &matrix = charmap[line[0]];
        //     for(int i=0;i<5;i++){
        //         std::getline(filestream,line);
        //         for(int j=0;j<5;j++){
        //             char l = line[j];
        //             matrix[i][j] = l;
        //         }
        //     }
        // }

        // addCharToBanner(100,100,'a');

        // auto &matrix = charmap['c'];
        // for(int i=0;i<5;i++){
        //     for(int j=0;j<5;j++){
        //         std::cout<<matrix[i][j];
        //     }
        //     std::cout << std::endl;
        // }

        drawnableObjects.push_back(&man);
        // drawnableObjects.push_back(&planet);
        moveableObjects.push_back(&spaceship);
        addStars();
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
        text_timestamp++;

        if(text_timestamp>100){
            text_timestamp = 0;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
            
            textPosition++;
            if(textPosition >= texts.size()){
                textPosition = 0;   
            }
        }

        // if(toClean){
        //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        //     toClean = false;
        // }

        displayText(lxBoundary+.50,byBoundary+.20,255,255,255,texts[textPosition].c_str());   

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
