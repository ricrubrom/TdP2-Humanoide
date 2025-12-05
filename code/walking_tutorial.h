#ifndef WALKING_TUTORIAL_H
#define WALKING_TUTORIAL_H

#include "main.h"
#include "walking.h"

// Structure to hold angle calculation result with explanation
struct AngleCalculation {
  float hip;      // degrees
  float knee;     // degrees
  float ankle;    // degrees
  String explanation; // human-readable explanation
};

// Structure to hold tutorial step information
struct TutorialStep {
  int phase;           // 1 or 2
  char leg;            // 'l' or 'r'
  int writeNumber;     // 1-4 (for the 4 key writes per leg)
  float x;             // target x position
  float z;             // target z position
  AngleCalculation angles;
  bool isExecuted;     // whether this step has been executed
};

// Structure to hold tutorial state
struct TutorialState {
  int currentPhase;      // 1 or 2
  int currentWriteIndex; // 0-7 (8 total writes: 4 per leg)
  bool isComplete;       // whether the full stepForward is complete
  TutorialStep steps[8]; // 4 writes per leg × 2 legs
};

// Initialize tutorial state for a new stepForward
void initTutorialStepForward(float stepLength);

// Get the current tutorial step (without executing)
TutorialStep* getCurrentTutorialStep();

// Get angle calculation with explanation for given x, z, leg, phase, writeNumber
AngleCalculation calculateAnglesWithExplanation(float x, float z, char leg, int phase, int writeNumber);

// Execute the current tutorial step (writes servos)
bool executeCurrentTutorialStep();

// Move to next tutorial step
bool advanceTutorialStep();

// Get current tutorial state
TutorialState* getTutorialState();

// Reset tutorial to initial state
void resetTutorial();

#endif // WALKING_TUTORIAL_H

