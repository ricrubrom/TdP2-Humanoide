#include "walking.h"

// External reference to robot configuration
extern RobotConfig robotConfig;

// Helper function to clamp servo angles to valid range
int clampServoAngle(int angle) {
  if (angle < 0) return 0;
  if (angle > 180) return 180;
  return angle;
}

void updateServoPos(int target1, int target2, int target3, char leg){
  if (leg == 'l'){
    currentLeftLeg.hip = robotConfig.hipLOffset - target1;
    currentLeftLeg.knee = robotConfig.kneeLOffset - target2;
    currentLeftLeg.ankle = 2*robotConfig.ankleLOffset - target3;
    hipL.write(currentLeftLeg.hip);
    kneeL.write(currentLeftLeg.knee);
    ankleL.write(currentLeftLeg.ankle);
  }
  else if (leg == 'r'){
    currentRightLeg.hip = robotConfig.hipROffset + target1;
    currentRightLeg.knee = robotConfig.kneeROffset + target2;
    currentRightLeg.ankle = target3;
    hipR.write(currentRightLeg.hip);
    kneeR.write(currentRightLeg.knee);
    ankleR.write(currentRightLeg.ankle);
  }
}

void pos(float x, float z, char leg){
  float hipRad2 = atan(x/z);
  float hipDeg2 = hipRad2 * (180/PI);

  float z2 = z/cos(hipRad2);

  float hipRad1 = acos((sq(robotConfig.l1) + sq(z2) - sq(robotConfig.l2))/(2*robotConfig.l1*z2));
  float hipDeg1 = hipRad1 * (180/PI);
  
  float kneeRad = PI - acos((sq(robotConfig.l1) + sq(robotConfig.l2) - sq(z2))/(2*robotConfig.l1*robotConfig.l2));

  float ankleRad = PI/2 + hipRad2 - acos((sq(robotConfig.l2) + sq(z2) - sq(robotConfig.l1))/(2*robotConfig.l2*z2));
  
  float hipDeg = hipDeg1 + hipDeg2;
  float kneeDeg = kneeRad * (180/PI);
  float ankleDeg = ankleRad * (180/PI);

//  Serial.print(hipDeg);
//  Serial.print("\t");
//  Serial.print(kneeDeg);
//  Serial.print("\t");
//  Serial.println(ankleDeg);

  updateServoPos(hipDeg, kneeDeg, ankleDeg, leg);  
}

void takeStep(float stepLength, int stepVelocity){
  for (float i = stepLength; i >= -stepLength; i-=0.5){
    pos(-i, robotConfig.stepHeight - robotConfig.stepClearance, 'r');
    pos(i, robotConfig.stepHeight, 'l');
    delay(stepVelocity);
  }
  for (float i = stepLength; i >= -stepLength; i-=0.5){
    pos(i + robotConfig.stepOffset, robotConfig.stepHeight, 'r');
    pos(-i, robotConfig.stepHeight - robotConfig.stepClearance, 'l');
    delay(stepVelocity);
  }

}

void initialize(){
  for (float i = 10.7; i >= robotConfig.stepHeight; i-=0.1){
    pos(0, i, 'l');
    pos(0, i, 'r');
  }
}

// Move to standing position
void moveToStandingPosition() {
  pos(0, robotConfig.stepHeight, 'l');
  pos(0, robotConfig.stepHeight, 'r');
  delay(500); // Allow servos to reach position
  
  // Update current leg positions
  currentLeftLeg = {robotConfig.hipLOffset, robotConfig.kneeLOffset, robotConfig.ankleLOffset};
  currentRightLeg = {robotConfig.hipROffset, robotConfig.kneeROffset, robotConfig.ankleROffset};
}


