#pragma once
#include "glutils.hpp"
#include <cstdio>

#define STEP (0.001f / aRatio);

enum Movement {
    LEFT,
    RIGHT,
    DOWN,
    UP,
    SIDES
};

enum MovementState {
    MOVING,
    FTL,
    STOPPING,
    STOPPED
};

class OrdinaryObject {
public:
    virtual void performActions() = 0;
};

class Drawnable : public OrdinaryObject {
public:
    virtual void draw() = 0;
};

class Moveable : public Drawnable {
public:
    float locationX = 0.0f;
    float locationY = 0.0f;
    float angle = 0.0f;
    float maxSpeed = 1.f;
    float maxRotationSpeed = 1.f;
    float minSpeed = 0.f;
    float minRotationSpeed = 0.f;
    float speed = .5f;
    float acceleration = 0.005f;
    float rotationAcceleration = 0.005f;
    float rotationSpeed = 0.005f;

    virtual void move(Movement movement) = 0;

    virtual void rotate(float _angle)
    {
        this->angle = _angle;
    };
    virtual void rotateAsParamether(float f) = 0;
};

class Spaceship : public Moveable {
public:
    MovementState movementState = STOPPED;
    bool turbo = false;
    GLfloat turboGain = 2.5;

    void toggleTurbo()
    {
        turbo = !turbo;
    }

    Spaceship()
    {
        minSpeed = 1.0f;
        speed = 0;

        rotationSpeed = minRotationSpeed;

        rotationAcceleration = 0.10f;
        acceleration = 0.20f;

        maxSpeed = 10.f;
        maxRotationSpeed = 1.f;
    }

    void setMovementState(MovementState state)
    {
        this->movementState = state;
    }

    virtual void rotateAsParamether(float angle) override
    {
        this->angle += fmod(angle, 360) * (speed / 3);
    }

    virtual void move(Movement movement) override
    {
        float sinA = sin(degrees2rad(this->angle)) * STEP;
        float cosA = cos(degrees2rad(this->angle)) * STEP;

        switch (movement) {
        case LEFT:
        case RIGHT:
            break;
        case SIDES:
            rotateAsParamether(rotationSpeed);
            break;
        case DOWN:
            break;
        case UP:
            locationY += cosA * speed * (turbo ? turboGain : 1);
            locationX -= sinA * speed * (turbo ? turboGain : 1);
            break;
        }
    }

    virtual void performActions() override
    {
        if (movementState == MovementState::MOVING) {

            if (locationY >= uyBoundary) {
                locationY = byBoundary;
            } else if (locationY <= byBoundary) {
                locationY = uyBoundary;
            }

            if (locationX >= rxBoundary) {
                locationX = lxBoundary;
            } else if (locationX <= lxBoundary) {
                locationX = rxBoundary;
            }

            move(Movement::UP);
            move(SIDES);
        } else if (movementState == MovementState::STOPPING) {
            if (speed <= 0) {
                speed = 0;
                movementState = MovementState::STOPPED;
                return;
            }
            speed -= acceleration * mDensity;
            rotationSpeed = 0;
            move(Movement::UP);
            move(SIDES);
        }
    }

    virtual void draw() override
    {
        static GLfloat i = speed;
        static int kleff = 0;

        glBegin(GL_POLYGON);
        glColor3f(0.2f, 0.0f, 0.0f);
        _glAspectVertex2f(-0.020f, 0.050f);
        _glAspectVertex2f(-0.010f, 0.030f);
        _glAspectVertex2f(0.010f, 0.030f);
        _glAspectVertex2f(0.020f, 0.050f);
        glColor3f(0.5f, 0.0f, 0.0f);
        _glAspectVertex2f(0.020f, 0.130f);
        _glAspectVertex2f(0, .180f);
        _glAspectVertex2f(-0.020, 0.130f);
        glEnd();

        glBegin(GL_POLYGON);
        glColor3f(0.4f, 0.0f, 0.0f);
        _glAspectVertex2f(-0.012f, -0.010f);
        _glAspectVertex2f(-0.012f, 0.020f);
        _glAspectVertex2f(-0.006f, 0.070f);
        glColor3f(0.7f, 0.0f, 0.0f);
        _glAspectVertex2f(0.006f, 0.070f);
        _glAspectVertex2f(0.012f, 0.020f);
        _glAspectVertex2f(0.012f, -0.010f);
        glEnd();

        glBegin(GL_POLYGON);
        glColor3f(0.3f, 0.0f, 0.0f);
        _glAspectVertex2f(-0.040f, -0.030f);
        _glAspectVertex2f(-0.020f, 0.020f);
        _glAspectVertex2f(-0.016f, -0.010f);
        _glAspectVertex2f(-0.012f, -0.010f);
        _glAspectVertex2f(-0.012f, 0.020f);
        _glAspectVertex2f(-0.016f, 0.020f);
        _glAspectVertex2f(-0.020f, 0.020f);
        glEnd();

        glBegin(GL_POLYGON);
        glColor3f(0.3f, 0.0f, 0.0f);
        _glAspectVertex2f(0.040f, -0.030f);
        _glAspectVertex2f(0.020f, 0.020f);
        _glAspectVertex2f(0.016f, -0.010f);
        _glAspectVertex2f(0.012f, -0.010f);
        _glAspectVertex2f(0.012f, 0.020f);
        _glAspectVertex2f(0.016f, 0.020f);
        _glAspectVertex2f(0.020f, 0.020f);
        glEnd();

        glBegin(GL_POLYGON);
        glColor3f(0.6f, 0.0f, 0.0f);
        _glAspectVertex2f(-0.032f, 0.130f);
        _glAspectVertex2f(-0.048f, 0.020f);
        _glAspectVertex2f(-0.035f, 0.010f);
        _glAspectVertex2f(-0.030f, 0.020f);
        _glAspectVertex2f(-0.030f, 0.050f);
        glEnd();

        glBegin(GL_POLYGON);
        glColor3f(0.4f, 0.0f, 0.0f);
        _glAspectVertex2f(-0.030f, 0.020f);
        _glAspectVertex2f(-0.020f, 0.050f);
        _glAspectVertex2f(-0.010f, 0.030f);
        _glAspectVertex2f(-0.030f, 0.050f);
        glEnd();

        glBegin(GL_POLYGON);
        glColor3f(0.6f, 0.0f, 0.0f);
        _glAspectVertex2f(0.032f, 0.130f);
        _glAspectVertex2f(0.048f, 0.020f);
        _glAspectVertex2f(0.035f, 0.010f);
        _glAspectVertex2f(0.030f, 0.020f);
        _glAspectVertex2f(0.030f, 0.050f);
        glEnd();

        glBegin(GL_POLYGON);
        glColor3f(0.4f, 0.0f, 0.0f);
        _glAspectVertex2f(0.030f, 0.020f);
        _glAspectVertex2f(0.020f, 0.050f);
        _glAspectVertex2f(0.010f, 0.030f);
        _glAspectVertex2f(0.030f, 0.050f);
        glEnd();

        if (movementState != MovementState::FTL) {

            if (i > 0) {
                i = 0.0;
            } else {
                i = speed * .005 * (turbo ? turboGain : 1);
                kleff = (kleff + 1) % 5;
            }

            glBegin(GL_POLYGON);

            if (!turbo) {
                glColor3f(1.f, 1.f, kleff < 3);
            } else {
                glColor3f(kleff < 3, 1.f, 1.f);
            }
            _glAspectVertex2f(-0.012f, -0.010f);
            _glAspectVertex2f(0.000f, -0.010f - i);
            glColor3f(1.f, kleff < 3, 1.f);

            _glAspectVertex2f(0.012f, -0.010f);
            glEnd();

        } else {

            if (i > 0) {
                i = 0;
            } else {
                i = 100;
            }

            glBegin(GL_POLYGON);
            glColor3f(1.f, .5f, .5f);
            _glAspectVertex2f(-0.012f, -0.010f);
            _glAspectVertex2f(0.000f, -0.010f - i);
            _glAspectVertex2f(0.012f, -0.010f);
            glEnd();
        }
    }
};