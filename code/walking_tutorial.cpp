#include "walking_tutorial.h"
#include <math.h>

// External reference to robot configuration
extern RobotConfig robotConfig;

// Tutorial state
static TutorialState tutorialState;
static float tutorialStepLength = 2.5f;

// Helper function to calculate angles from x, z (similar to pos function but returns angles without writing)
AngleCalculation calculateAngles(float x, float z) {
  AngleCalculation result;
  
  // Calculate hipRad2 = atan(x/z) - angle of foot position in X-Z plane
  float hipRad2 = atan(x/z);
  float hipDeg2 = hipRad2 * (180.0/PI);
  
  // Calculate z2 = z/cos(hipRad2) - distance along leg plane
  float z2 = z/cos(hipRad2);
  
  // Calculate hipRad1 using law of cosines: acos((l1² + z2² - l2²)/(2*l1*z2))
  float hipRad1 = acos((sq(robotConfig.l1) + sq(z2) - sq(robotConfig.l2))/(2*robotConfig.l1*z2));
  float hipDeg1 = hipRad1 * (180.0/PI);
  
  // Calculate kneeRad using law of cosines: π - acos((l1² + l2² - z2²)/(2*l1*l2))
  float kneeRad = PI - acos((sq(robotConfig.l1) + sq(robotConfig.l2) - sq(z2))/(2*robotConfig.l1*robotConfig.l2));
  
  // Calculate ankleRad: π/2 + hipRad2 - acos((l2² + z2² - l1²)/(2*l2*z2))
  float ankleRad = PI/2 + hipRad2 - acos((sq(robotConfig.l2) + sq(z2) - sq(robotConfig.l1))/(2*robotConfig.l2*z2));
  
  result.hip = hipDeg1 + hipDeg2;
  result.knee = kneeRad * (180.0/PI);
  result.ankle = ankleRad * (180.0/PI);
  
  return result;
}

// Generate explanation text for angle calculation
String generateExplanation(float x, float z, char leg, AngleCalculation angles, int phase, int writeNumber) {
  String legName = (leg == 'l') ? "izquierda" : "derecha";
  String expl = "";
  
  // Context explanation based on phase and write number
  if (phase == 1) {
    if (leg == 'l') {
      if (writeNumber == 1) {
        expl = "En la <strong>Fase 1</strong>, la pierna " + legName + " está <strong>avanzando</strong> (soporta el peso del robot). ";
        expl += "Comenzamos moviendo el pie a la posición inicial del paso: X=" + String(x, 2) + "cm (adelante), Z=" + String(z, 2) + "cm (altura de apoyo). ";
        expl += "Esta es la pierna que está en contacto con el suelo y empuja el robot hacia adelante. ";
      } else if (writeNumber == 2) {
        expl = "Continuamos con la pierna " + legName + " en <strong>Fase 1</strong> (avanzando y soportando peso). ";
        expl += "Movemos el pie a la posición final del paso: X=" + String(x, 2) + "cm (atrás), Z=" + String(z, 2) + "cm (altura de apoyo). ";
        expl += "El pie se desplaza hacia atrás mientras mantiene contacto con el suelo, completando la mitad del paso. ";
      }
    } else { // right leg in phase 1
      if (writeNumber == 1) {
        expl = "En la <strong>Fase 1</strong>, la pierna " + legName + " está <strong>elevada</strong> (no soporta peso). ";
        expl += "Comenzamos moviendo el pie a la posición inicial: X=" + String(x, 2) + "cm (atrás), Z=" + String(z, 2) + "cm (altura reducida para no tocar el suelo). ";
        expl += "Esta pierna se eleva para evitar arrastrarse mientras la otra pierna avanza. ";
      } else if (writeNumber == 2) {
        expl = "Continuamos con la pierna " + legName + " en <strong>Fase 1</strong> (elevada). ";
        expl += "Movemos el pie a la posición final: X=" + String(x, 2) + "cm (adelante), Z=" + String(z, 2) + "cm (altura reducida). ";
        expl += "El pie se mueve hacia adelante en el aire, preparándose para el siguiente contacto con el suelo. ";
      }
    }
  } else { // phase 2
    if (leg == 'r') {
      if (writeNumber == 3) {
        expl = "En la <strong>Fase 2</strong>, ahora la pierna " + legName + " está <strong>avanzando</strong> (soporta el peso). ";
        expl += "Comenzamos moviendo el pie a la posición inicial: X=" + String(x, 2) + "cm (adelante), Z=" + String(z, 2) + "cm (altura de apoyo). ";
        expl += "Los roles se invierten: ahora esta pierna toma contacto con el suelo y empuja el robot. ";
      } else if (writeNumber == 4) {
        expl = "Continuamos con la pierna " + legName + " en <strong>Fase 2</strong> (avanzando y soportando peso). ";
        expl += "Movemos el pie a la posición final: X=" + String(x, 2) + "cm (atrás), Z=" + String(z, 2) + "cm (altura de apoyo). ";
        expl += "El pie completa su desplazamiento hacia atrás, finalizando el paso completo del robot. ";
      }
    } else { // left leg in phase 2
      if (writeNumber == 3) {
        expl = "En la <strong>Fase 2</strong>, ahora la pierna " + legName + " está <strong>elevada</strong> (no soporta peso). ";
        expl += "Comenzamos moviendo el pie a la posición inicial: X=" + String(x, 2) + "cm (atrás), Z=" + String(z, 2) + "cm (altura reducida). ";
        expl += "Esta pierna se eleva mientras la otra toma el control del movimiento. ";
      } else if (writeNumber == 4) {
        expl = "Continuamos con la pierna " + legName + " en <strong>Fase 2</strong> (elevada). ";
        expl += "Movemos el pie a la posición final: X=" + String(x, 2) + "cm (adelante), Z=" + String(z, 2) + "cm (altura reducida). ";
        expl += "El pie se prepara en el aire para el siguiente ciclo de marcha. ";
      }
    }
  }
  
  expl += "<br><br><strong>Cálculo de cinemática inversa:</strong> ";
  expl += "Para alcanzar esta posición, calculamos los ángulos usando trigonometría. ";
  expl += "Primero, hipRad2 = atan(" + String(x, 2) + "/" + String(z, 2) + ") = " + String(atan(x/z) * 180.0/PI, 1) + "° (ángulo del pie en el plano X-Z). ";
  expl += "Luego z2 = " + String(z, 2) + "/cos(hipRad2) = " + String(z/cos(atan(x/z)), 2) + "cm (distancia en el plano de la pierna). ";
  expl += "Aplicando la ley de cosenos a un triángulo formado por los segmentos L1 y L2, obtenemos: Hip = " + String(angles.hip, 1) + "°, Knee = " + String(angles.knee, 1) + "°, Ankle = " + String(angles.ankle, 1) + "°.";
  
  expl += "<br><br><strong>Conversión a posiciones de servo:</strong> ";
  if (leg == 'l') {
    expl += "Para la pierna izquierda, aplicamos offsets inversos: ";
    expl += "Hip servo = " + String((int)(robotConfig.hipLOffset - angles.hip)) + "° (offset " + String(robotConfig.hipLOffset) + "° - " + String(angles.hip, 1) + "°), ";
    expl += "Knee servo = " + String((int)(robotConfig.kneeLOffset - angles.knee)) + "° (offset " + String(robotConfig.kneeLOffset) + "° - " + String(angles.knee, 1) + "°), ";
    expl += "Ankle servo = " + String((int)(2*robotConfig.ankleLOffset - angles.ankle)) + "° (2×offset " + String(robotConfig.ankleLOffset) + "° - " + String(angles.ankle, 1) + "°).";
  } else {
    expl += "Para la pierna derecha, aplicamos offsets directos: ";
    expl += "Hip servo = " + String((int)(robotConfig.hipROffset + angles.hip)) + "° (offset " + String(robotConfig.hipROffset) + "° + " + String(angles.hip, 1) + "°), ";
    expl += "Knee servo = " + String((int)(robotConfig.kneeROffset + angles.knee)) + "° (offset " + String(robotConfig.kneeROffset) + "° + " + String(angles.knee, 1) + "°), ";
    expl += "Ankle servo = " + String((int)(robotConfig.ankleROffset + angles.ankle)) + "° (offset " + String(robotConfig.ankleROffset) + "° + " + String(angles.ankle, 1) + "°).";
  }
  
  return expl;
}

// Calculate angles with explanation
AngleCalculation calculateAnglesWithExplanation(float x, float z, char leg, int phase, int writeNumber) {
  AngleCalculation result = calculateAngles(x, z);
  result.explanation = generateExplanation(x, z, leg, result, phase, writeNumber);
  return result;
}

// Initialize tutorial state for a new stepForward
void initTutorialStepForward(float stepLength) {
  tutorialStepLength = stepLength;
  tutorialState.currentPhase = 1;
  tutorialState.currentWriteIndex = 0;
  tutorialState.isComplete = false;
  
  // Define the 8 key writes (4 per leg):
  // Phase 1: Left leg advancing (stepHeight), Right leg holding (stepHeight - stepClearance)
  // Phase 2: Right leg advancing (stepHeight), Left leg holding (stepHeight - stepClearance)
  
  float sh = robotConfig.stepHeight;
  float sc = robotConfig.stepClearance;
  float so = robotConfig.stepOffset;
  
  // Phase 1: Left leg start (stepLength, stepHeight)
  tutorialState.steps[0].phase = 1;
  tutorialState.steps[0].leg = 'l';
  tutorialState.steps[0].writeNumber = 1;
  tutorialState.steps[0].x = stepLength;
  tutorialState.steps[0].z = sh;
  tutorialState.steps[0].angles = calculateAnglesWithExplanation(stepLength, sh, 'l', 1, 1);
  tutorialState.steps[0].isExecuted = false;
  
  // Phase 1: Right leg start (-stepLength, stepHeight - stepClearance)
  tutorialState.steps[1].phase = 1;
  tutorialState.steps[1].leg = 'r';
  tutorialState.steps[1].writeNumber = 1;
  tutorialState.steps[1].x = -stepLength;
  tutorialState.steps[1].z = sh - sc;
  tutorialState.steps[1].angles = calculateAnglesWithExplanation(-stepLength, sh - sc, 'r', 1, 1);
  tutorialState.steps[1].isExecuted = false;
  
  // Phase 1: Left leg end (-stepLength, stepHeight)
  tutorialState.steps[2].phase = 1;
  tutorialState.steps[2].leg = 'l';
  tutorialState.steps[2].writeNumber = 2;
  tutorialState.steps[2].x = -stepLength;
  tutorialState.steps[2].z = sh;
  tutorialState.steps[2].angles = calculateAnglesWithExplanation(-stepLength, sh, 'l', 1, 2);
  tutorialState.steps[2].isExecuted = false;
  
  // Phase 1: Right leg end (stepLength, stepHeight - stepClearance)
  tutorialState.steps[3].phase = 1;
  tutorialState.steps[3].leg = 'r';
  tutorialState.steps[3].writeNumber = 2;
  tutorialState.steps[3].x = stepLength;
  tutorialState.steps[3].z = sh - sc;
  tutorialState.steps[3].angles = calculateAnglesWithExplanation(stepLength, sh - sc, 'r', 1, 2);
  tutorialState.steps[3].isExecuted = false;
  
  // Phase 2: Right leg start (stepLength + stepOffset, stepHeight)
  tutorialState.steps[4].phase = 2;
  tutorialState.steps[4].leg = 'r';
  tutorialState.steps[4].writeNumber = 3;
  tutorialState.steps[4].x = stepLength + so;
  tutorialState.steps[4].z = sh;
  tutorialState.steps[4].angles = calculateAnglesWithExplanation(stepLength + so, sh, 'r', 2, 3);
  tutorialState.steps[4].isExecuted = false;
  
  // Phase 2: Left leg start (-stepLength, stepHeight - stepClearance)
  tutorialState.steps[5].phase = 2;
  tutorialState.steps[5].leg = 'l';
  tutorialState.steps[5].writeNumber = 3;
  tutorialState.steps[5].x = -stepLength;
  tutorialState.steps[5].z = sh - sc;
  tutorialState.steps[5].angles = calculateAnglesWithExplanation(-stepLength, sh - sc, 'l', 2, 3);
  tutorialState.steps[5].isExecuted = false;
  
  // Phase 2: Right leg end (-stepLength + stepOffset, stepHeight)
  tutorialState.steps[6].phase = 2;
  tutorialState.steps[6].leg = 'r';
  tutorialState.steps[6].writeNumber = 4;
  tutorialState.steps[6].x = -stepLength + so;
  tutorialState.steps[6].z = sh;
  tutorialState.steps[6].angles = calculateAnglesWithExplanation(-stepLength + so, sh, 'r', 2, 4);
  tutorialState.steps[6].isExecuted = false;
  
  // Phase 2: Left leg end (stepLength, stepHeight - stepClearance)
  tutorialState.steps[7].phase = 2;
  tutorialState.steps[7].leg = 'l';
  tutorialState.steps[7].writeNumber = 4;
  tutorialState.steps[7].x = stepLength;
  tutorialState.steps[7].z = sh - sc;
  tutorialState.steps[7].angles = calculateAnglesWithExplanation(stepLength, sh - sc, 'l', 2, 4);
  tutorialState.steps[7].isExecuted = false;
}

// Get the current tutorial step
TutorialStep* getCurrentTutorialStep() {
  if (tutorialState.isComplete || tutorialState.currentWriteIndex >= 8) {
    return nullptr;
  }
  return &tutorialState.steps[tutorialState.currentWriteIndex];
}

// Execute the current tutorial step (writes servos)
bool executeCurrentTutorialStep() {
  TutorialStep* step = getCurrentTutorialStep();
  if (step == nullptr) {
    return false;
  }
  
  // Use the existing pos function to execute the servo write
  pos(step->x, step->z, step->leg);
  
  step->isExecuted = true;
  return true;
}

// Move to next tutorial step
bool advanceTutorialStep() {
  if (tutorialState.isComplete) {
    return false;
  }
  
  tutorialState.currentWriteIndex++;
  
  if (tutorialState.currentWriteIndex >= 8) {
    tutorialState.isComplete = true;
    return false;
  }
  
  // Update current phase based on step index
  if (tutorialState.currentWriteIndex < 4) {
    tutorialState.currentPhase = 1;
  } else {
    tutorialState.currentPhase = 2;
  }
  
  return true;
}

// Get current tutorial state
TutorialState* getTutorialState() {
  return &tutorialState;
}

// Reset tutorial to initial state
void resetTutorial() {
  tutorialState.currentPhase = 1;
  tutorialState.currentWriteIndex = 0;
  tutorialState.isComplete = false;
  for (int i = 0; i < 8; i++) {
    tutorialState.steps[i].isExecuted = false;
  }
}

