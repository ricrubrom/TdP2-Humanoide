#include "main.h"
#include "webinterface.h"
#include "walking.h"
#include "walking_tutorial.h"


// -------------------- WIFI / SERVER --------------------
const char* ssid     = "Humanoide_ESP8266";
const char* password = "12345678";
ESP8266WebServer server(80);
// IP: 192.168.4.1

// -------------------- SERVOS --------------------
Servo hipL, kneeL, ankleL;
Servo hipR, kneeR, ankleR;


// -------------------- STATE --------------------
RobotConfig robotConfig;
LegAngles currentLeftLeg = {robotConfig.hipLOffset, robotConfig.kneeLOffset, robotConfig.ankleLOffset};
LegAngles currentRightLeg = {robotConfig.hipROffset, robotConfig.kneeROffset, robotConfig.ankleROffset};

volatile bool shouldWalkForward = false;  // set by /forward handler



// -------------------- HANDLERS --------------------
void handleRoot() { server.send(200, "text/html", htmlPage(currentLeftLeg, currentRightLeg)); }
void handleKinematics() { server.send(200, "text/html", htmlKinematicsPage()); }

void handleSetServos() {
  Serial.println("Actualizando posiciones de servos:");
  if (server.hasArg("hipL"))   { currentLeftLeg.hip   = clampServoAngle(server.arg("hipL").toInt());   hipL.write(currentLeftLeg.hip);   Serial.println("Left Hip -> "   + server.arg("hipL")); }
  if (server.hasArg("kneeL"))  { currentLeftLeg.knee  = clampServoAngle(server.arg("kneeL").toInt());  kneeL.write(currentLeftLeg.knee);  Serial.println("Left Knee -> "  + server.arg("kneeL")); }
  if (server.hasArg("ankleL")) { currentLeftLeg.ankle = clampServoAngle(server.arg("ankleL").toInt()); ankleL.write(currentLeftLeg.ankle);Serial.println("Left Ankle -> " + server.arg("ankleL")); }
  if (server.hasArg("hipR"))   { currentRightLeg.hip   = clampServoAngle(server.arg("hipR").toInt());   hipR.write(currentRightLeg.hip);   Serial.println("Right Hip -> "   + server.arg("hipR")); }
  if (server.hasArg("kneeR"))  { currentRightLeg.knee  = clampServoAngle(server.arg("kneeR").toInt());  kneeR.write(currentRightLeg.knee);  Serial.println("Right Knee -> "  + server.arg("kneeR")); }
  if (server.hasArg("ankleR")) { currentRightLeg.ankle = clampServoAngle(server.arg("ankleR").toInt()); ankleR.write(currentRightLeg.ankle);Serial.println("Right Ankle -> " + server.arg("ankleR")); }

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSetConfig() {
  Serial.println("Actualizando configuración del robot:");
  
  // Update offsets
  if (server.hasArg("hipLOffset"))   { robotConfig.hipLOffset = clampServoAngle(server.arg("hipLOffset").toInt());   Serial.println("Hip L Offset -> " + server.arg("hipLOffset")); }
  if (server.hasArg("kneeLOffset"))  { robotConfig.kneeLOffset = clampServoAngle(server.arg("kneeLOffset").toInt()); Serial.println("Knee L Offset -> " + server.arg("kneeLOffset")); }
  if (server.hasArg("ankleLOffset")) { robotConfig.ankleLOffset = clampServoAngle(server.arg("ankleLOffset").toInt()); Serial.println("Ankle L Offset -> " + server.arg("ankleLOffset")); }
  if (server.hasArg("hipROffset"))   { robotConfig.hipROffset = clampServoAngle(server.arg("hipROffset").toInt());   Serial.println("Hip R Offset -> " + server.arg("hipROffset")); }
  if (server.hasArg("kneeROffset"))  { robotConfig.kneeROffset = clampServoAngle(server.arg("kneeROffset").toInt()); Serial.println("Knee R Offset -> " + server.arg("kneeROffset")); }
  if (server.hasArg("ankleROffset")) { robotConfig.ankleROffset = clampServoAngle(server.arg("ankleROffset").toInt()); Serial.println("Ankle R Offset -> " + server.arg("ankleROffset")); }
  
  // Update leg geometry
  if (server.hasArg("l1")) { robotConfig.l1 = server.arg("l1").toFloat(); Serial.println("L1 -> " + server.arg("l1")); }
  if (server.hasArg("l2")) { robotConfig.l2 = server.arg("l2").toFloat(); Serial.println("L2 -> " + server.arg("l2")); }
  
  // Update step parameters
  if (server.hasArg("stepClearance")) { robotConfig.stepClearance = server.arg("stepClearance").toFloat(); Serial.println("Step Clearance -> " + server.arg("stepClearance")); }
  if (server.hasArg("stepHeight")) { robotConfig.stepHeight = server.arg("stepHeight").toFloat(); Serial.println("Step Height -> " + server.arg("stepHeight")); }
  if (server.hasArg("stepLength")) { robotConfig.stepLength = server.arg("stepLength").toFloat(); Serial.println("Step Length -> " + server.arg("stepLength")); }
  if (server.hasArg("stepVelocity")) { robotConfig.stepVelocity = server.arg("stepVelocity").toInt(); Serial.println("Step Velocity -> " + server.arg("stepVelocity")); }
  if (server.hasArg("stepOffset")) { robotConfig.stepOffset = server.arg("stepOffset").toFloat(); Serial.println("Step Offset -> " + server.arg("stepOffset")); }
  if (server.hasArg("numSteps")) { robotConfig.numSteps = server.arg("numSteps").toInt(); Serial.println("Number of Steps -> " + server.arg("numSteps")); }

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleForward()  { Serial.println("UI: Adelante"); shouldWalkForward = true;  server.sendHeader("Location", "/kinematics"); server.send(303); }
void handleBackward() { Serial.println("UI: Atras");                       server.sendHeader("Location", "/kinematics"); server.send(303); }
void handleLeft()     { Serial.println("UI: Izquierda");                   server.sendHeader("Location", "/kinematics"); server.send(303); }
void handleRight()    { Serial.println("UI: Derecha");                     server.sendHeader("Location", "/kinematics"); server.send(303); }

void handleGetLegs() {
  String json = "{"
                "\"left\":{"
                "\"hip\":" + String(currentLeftLeg.hip) + ","
                "\"knee\":" + String(currentLeftLeg.knee) + ","
                "\"ankle\":" + String(currentLeftLeg.ankle) + "},"
                "\"right\":{"
                "\"hip\":" + String(currentRightLeg.hip) + ","
                "\"knee\":" + String(currentRightLeg.knee) + ","
                "\"ankle\":" + String(currentRightLeg.ankle) + "}"
                "}";
  server.send(200, "application/json", json);
}

void handleReset() {
  Serial.println("Resetting to neutral position");
  currentLeftLeg.hip = robotConfig.hipLOffset;
  currentLeftLeg.knee = robotConfig.kneeLOffset;
  currentLeftLeg.ankle = robotConfig.ankleLOffset;
  currentRightLeg.hip = robotConfig.hipROffset;
  currentRightLeg.knee = robotConfig.kneeROffset;
  currentRightLeg.ankle = robotConfig.ankleROffset;
  
  hipL.write(currentLeftLeg.hip);
  kneeL.write(currentLeftLeg.knee);
  ankleL.write(currentLeftLeg.ankle);
  hipR.write(currentRightLeg.hip);
  kneeR.write(currentRightLeg.knee);
  ankleR.write(currentRightLeg.ankle);
  
  // Reset tutorial state
  resetTutorial();
  
  // Redirect based on query parameter
  if (server.hasArg("from") && server.arg("from") == "kinematics") {
    server.sendHeader("Location", "/kinematics");
  } else {
    server.sendHeader("Location", "/");
  }
  server.send(303);
}

// Tutorial handlers
void handleTutorialInit() {
  Serial.println("Initializing tutorial stepForward");
  resetTutorial();
  initTutorialStepForward(robotConfig.stepLength);
  server.send(200, "application/json", "{\"success\":true}");
}

void handleTutorialStep() {
  TutorialStep* step = getCurrentTutorialStep();
  TutorialState* state = getTutorialState();
  
  if (step == nullptr || state->isComplete) {
    server.send(200, "application/json", "{\"complete\":true}");
    return;
  }
  
  // Escape quotes in explanation for JSON
  String escapedExplanation = step->angles.explanation;
  escapedExplanation.replace("\"", "\\\"");
  escapedExplanation.replace("\n", "\\n");
  
  String json = "{";
  json += "\"success\":true,";
  json += "\"currentIndex\":" + String(state->currentWriteIndex) + ",";
  json += "\"totalSteps\":8,";
  json += "\"step\":{";
  json += "\"phase\":" + String(step->phase) + ",";
  json += "\"leg\":\"" + String(step->leg) + "\",";
  json += "\"writeNumber\":" + String(step->writeNumber) + ",";
  json += "\"x\":" + String(step->x, 2) + ",";
  json += "\"z\":" + String(step->z, 2) + ",";
  json += "\"angles\":{";
  json += "\"hip\":" + String(step->angles.hip, 2) + ",";
  json += "\"knee\":" + String(step->angles.knee, 2) + ",";
  json += "\"ankle\":" + String(step->angles.ankle, 2) + ",";
  json += "\"explanation\":\"" + escapedExplanation + "\"";
  json += "}";
  json += "}";
  json += "}";
  
  server.send(200, "application/json", json);
}

void handleTutorialExecute() {
  Serial.println("Executing current tutorial step");
  bool success = executeCurrentTutorialStep();
  if (success) {
    server.send(200, "application/json", "{\"success\":true}");
  } else {
    server.send(200, "application/json", "{\"success\":false,\"error\":\"No step to execute\"}");
  }
}

void handleTutorialAdvance() {
  Serial.println("Advancing to next tutorial step");
  bool hasMore = advanceTutorialStep();
  if (hasMore) {
    server.send(200, "application/json", "{\"hasMore\":true}");
  } else {
    server.send(200, "application/json", "{\"hasMore\":false,\"complete\":true}");
  }
}

// -------------------- INITIALIZATION --------------------
void initializeWiFi() {
  Serial.println("\nIniciando Access Point...");
  WiFi.softAP(ssid, password);
  Serial.print("IP del AP: "); 
  Serial.println(WiFi.softAPIP());
  Serial.println("IP: 192.168.4.1");
}

void initializeServos() {
  // Attach servos to pins
  hipR.attach(HIP_R_PIN); 
  kneeR.attach(KNEE_R_PIN); 
  ankleR.attach(ANKLE_R_PIN);
  hipL.attach(HIP_L_PIN); 
  kneeL.attach(KNEE_L_PIN); 
  ankleL.attach(ANKLE_L_PIN);

  // Initialize servos to standing position
  hipL.write(robotConfig.hipLOffset); 
  kneeL.write(robotConfig.kneeLOffset); 
  ankleL.write(robotConfig.ankleLOffset);
  hipR.write(robotConfig.hipROffset); 
  kneeR.write(robotConfig.kneeROffset); 
  ankleR.write(robotConfig.ankleROffset);
  
  // Initialize current leg positions
  currentLeftLeg = {robotConfig.hipLOffset, robotConfig.kneeLOffset, robotConfig.ankleLOffset};
  currentRightLeg = {robotConfig.hipROffset, robotConfig.kneeROffset, robotConfig.ankleROffset};
  
  delay(300); // Allow servos to reach initial position
  Serial.println("Servos inicializados en posición de pie");
}

void initializeWebServer() {
  server.on("/", handleRoot);
  server.on("/kinematics", handleKinematics);
  server.on("/setservos", handleSetServos);
  server.on("/setconfig", handleSetConfig);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/getlegs", handleGetLegs);
  server.on("/reset", handleReset);
  
  // Tutorial endpoints
  server.on("/tutorial/init", handleTutorialInit);
  server.on("/tutorial/step", handleTutorialStep);
  server.on("/tutorial/execute", handleTutorialExecute);
  server.on("/tutorial/advance", handleTutorialAdvance);

  server.begin();
  Serial.println("Servidor web iniciado.");
}

void initializeSystem() {
  Serial.begin(115200);
  
  initializeWiFi();
  initializeServos();
  initializeWebServer();
  
  Serial.println("Sistema inicializado completamente");
}

// -------------------- SETUP --------------------
void setup() {
  initializeSystem();
}

// -------------------- LOOP --------------------
void loop() {
  server.handleClient();

  // run any requested action non-blocking from the main loop
  if (shouldWalkForward) {
    Serial.println("== WALK FORWARD ==");
    Serial.print("Taking ");
    Serial.print(robotConfig.numSteps);
    Serial.println(" step(s)");
    
    for (int i = 0; i < robotConfig.numSteps; i++) {
      Serial.print("Step ");
      Serial.print(i + 1);
      Serial.print(" of ");
      Serial.println(robotConfig.numSteps);
      takeStep(robotConfig.stepLength, robotConfig.stepVelocity);
    }
    
    // moveToStandingPosition();
    shouldWalkForward = false;
    Serial.println("== DONE ==");
  }
}
