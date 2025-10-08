#include <Servo.h>
#define HIP_R_PIN    D1
#define KNEE_R_PIN   D2
#define ANKLE_R_PIN  D5
#define HIP_L_PIN    D6
#define KNEE_L_PIN   D7
#define ANKLE_L_PIN  D8   // DEBE ESTAR EN BAJO AL BOOTEAR
#define BUTTON_PIN   D0   // Input Pullup para iniciar

Servo hipL, kneeL, ankleL;
Servo hipR, kneeR, ankleR;

void setup() {
  hipR.attach(HIP_R_PIN);   kneeR.attach(KNEE_R_PIN);   ankleR.attach(ANKLE_R_PIN);
  hipL.attach(HIP_L_PIN);   kneeL.attach(KNEE_L_PIN);   ankleL.attach(ANKLE_L_PIN);

}

void loop() {
  hipL.write(80); kneeL.write(85); ankleL.write(90);
  hipR.write(77); kneeR.write(80); ankleR.write(100);
  delay(10);
}

// HiPL = 80 (-1)
// KneeL = 85 (-1)
// ancleL = 90 (-1)

// hipR = 77 (+1)
// kneeR = 80 (+1)
// ankleR = 100 (+1)
