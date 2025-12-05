#ifndef WALKING_H
#define WALKING_H

#include "main.h"
#include <math.h>

// Forward declarations for external variables (defined in Movimiento.ino)
extern LegAngles currentLeftLeg;
extern LegAngles currentRightLeg;
extern Servo hipL, kneeL, ankleL;
extern Servo hipR, kneeR, ankleR;
extern ESP8266WebServer server;

// Helper functions (implemented in walking.cpp)
int clampServoAngle(int angle);

// Core movement functions
void updateServoPos(int target1, int target2, int target3, char leg);
void pos(float x, float z, char leg);

// Walking execution
void takeStep(float stepLength, int stepVelocity);
void initialize();

// Movement functions
void moveToStandingPosition();

#endif // WALKING_H
