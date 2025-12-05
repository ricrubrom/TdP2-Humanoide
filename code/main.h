#ifndef MAIN_H
#define MAIN_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// -------------------- TYPES --------------------
struct LegAngles { int hip, knee, ankle; };  // degrees

// -------------------- CONSTANTS --------------------
// PINS
#define HIP_R_PIN    D1
#define KNEE_R_PIN   D2
#define ANKLE_R_PIN  D5
#define HIP_L_PIN    D6
#define KNEE_L_PIN   D7
#define ANKLE_L_PIN  D8

// -------------------- CONFIGURABLE PARAMETERS --------------------
struct RobotConfig {
  // OFFSETS
  int hipLOffset = 80;
  int kneeLOffset = 85;
  int ankleLOffset = 70;
  int hipROffset = 77;
  int kneeROffset = 80;
  int ankleROffset = 100;

  // LEG GEOMETRY (cm)
  float l1 = 5.0f;   // upper leg segment
  float l2 = 5.7f;   // lower leg segment

  // STEP PARAMETERS
  float stepClearance = 1.0f;
  float stepHeight = 10.0f;
  float stepLength = 2.5f;    // step length in cm
  int stepVelocity = 100;     // step velocity in ms delay
  float stepOffset = 0.0f;    // offset for second step
  int numSteps = 1;           // number of steps to take
};

// Global configuration instance
extern RobotConfig robotConfig;


#endif // MAIN_H
