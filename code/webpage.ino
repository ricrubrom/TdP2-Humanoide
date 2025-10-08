#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// -------------------- CONFIGURACIÓN --------------------
const char* ssid = "Humanoide_ESP8266";
const char* password = "robot1234";

ESP8266WebServer server(80);

// Pines para los servos
#define HIP_R_PIN    D1
#define KNEE_R_PIN   D2
#define ANKLE_R_PIN  D5
#define HIP_L_PIN    D6
#define KNEE_L_PIN   D7
#define ANKLE_L_PIN  D8

const char HIPL = 80;
const char KNEEL = 85; 
const char ANKLEL = 90; 

const char HIPR = 77; 
const char KNEER = 80; 
const char ANKLER = 100; 

Servo hipL, kneeL, ankleL;
Servo hipR, kneeR, ankleR;

// -------------------- PÁGINA HTML --------------------
String htmlPage() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Control Humanoide</title>";
  html += "<style>"
          "body{font-family:sans-serif;text-align:center;background:#f4f4f4;}"
          ".dpad{display:inline-block; margin:20px;}"
          ".row{display:flex; justify-content:center;}"
          "button{width:80px;height:80px;margin:5px;font-size:20px;}"
          "input{width:60px;margin:5px;}"
          "form{margin-top:20px;padding:10px;background:#fff;display:inline-block;border-radius:10px;box-shadow:0 0 10px rgba(0,0,0,0.2);}"
          "</style>";
  html += "<script>"
          "document.addEventListener('keydown', function(e){"
          "  if(e.key==='w'){ window.location.href='/forward'; }"
          "  else if(e.key==='s'){ window.location.href='/backward'; }"
          "  else if(e.key==='a'){ window.location.href='/left'; }"
          "  else if(e.key==='d'){ window.location.href='/right'; }"
          "});"
          "</script>";
  html += "</head><body><h2>Control de Humanoide</h2>";

  // D-Pad
  html += "<div class='dpad'>";
  html += "<div class='row'><button onclick=\"location.href='/forward'\">&#9650;</button></div>"; // Arriba
  html += "<div class='row'><button onclick=\"location.href='/left'\">&#9664;</button>";
  html += "<button onclick=\"location.href='/right'\">&#9654;</button></div>"; // Izq y Der
  html += "<div class='row'><button onclick=\"location.href='/backward'\">&#9660;</button></div>"; // Abajo
  html += "</div>";

  // Formulario de servos con valores actuales
  html += "<form action='/setservos' method='GET'><h3>Posiciones de Servos (0°–180°)</h3>";
  html += "Hip L: <input type='number' name='hipL' min='0' max='180' value='" + String(hipL.read()) + "'><br>";
  html += "Knee L: <input type='number' name='kneeL' min='0' max='180' value='" + String(kneeL.read()) + "'><br>";
  html += "Ankle L: <input type='number' name='ankleL' min='0' max='180' value='" + String(ankleL.read()) + "'><br>";
  html += "Hip R: <input type='number' name='hipR' min='0' max='180' value='" + String(hipR.read()) + "'><br>";
  html += "Knee R: <input type='number' name='kneeR' min='0' max='180' value='" + String(kneeR.read()) + "'><br>";
  html += "Ankle R: <input type='number' name='ankleR' min='0' max='180' value='" + String(ankleR.read()) + "'><br>";
  html += "<br><input type='submit' style='width:120px; height:50px; font-size:18px;' value='Actualizar'></form>";
  html += "</body></html>";
  return html;
}

// -------------------- HANDLERS --------------------
void handleRoot() { server.send(200, "text/html", htmlPage()); }

void handleSetServos() {
  Serial.println("Actualizando posiciones de servos:");
  if (server.hasArg("hipL")) { hipL.write(server.arg("hipL").toInt()); Serial.println("hipL -> " + server.arg("hipL")); }
  if (server.hasArg("kneeL")) { kneeL.write(server.arg("kneeL").toInt()); Serial.println("kneeL -> " + server.arg("kneeL")); }
  if (server.hasArg("ankleL")) { ankleL.write(server.arg("ankleL").toInt()); Serial.println("ankleL -> " + server.arg("ankleL")); }
  if (server.hasArg("hipR")) { hipR.write(server.arg("hipR").toInt()); Serial.println("hipR -> " + server.arg("hipR")); }
  if (server.hasArg("kneeR")) { kneeR.write(server.arg("kneeR").toInt()); Serial.println("kneeR -> " + server.arg("kneeR")); }
  if (server.hasArg("ankleR")) { ankleR.write(server.arg("ankleR").toInt()); Serial.println("ankleR -> " + server.arg("ankleR")); }

  server.sendHeader("Location", "/");
  server.send(303);
}

// -------------------- HANDLERS DE MOVIMIENTO --------------------
void handleForward() { Serial.println("Botón Adelante presionado"); server.sendHeader("Location", "/"); server.send(303); }
void handleBackward() { Serial.println("Botón Atrás presionado"); server.sendHeader("Location", "/"); server.send(303); }
void handleLeft() { Serial.println("Botón Izquierda presionado"); server.sendHeader("Location", "/"); server.send(303); }
void handleRight() { Serial.println("Botón Derecha presionado"); server.sendHeader("Location", "/"); server.send(303); }

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Access Point...");

  WiFi.softAP(ssid, password);
  Serial.print("IP del AP: ");
  Serial.println(WiFi.softAPIP());

  hipR.attach(HIP_R_PIN);   kneeR.attach(KNEE_R_PIN);   ankleR.attach(ANKLE_R_PIN);
  hipL.attach(HIP_L_PIN);   kneeL.attach(KNEE_L_PIN);   ankleL.attach(ANKLE_L_PIN);

  // Posición neutra
  hipL.write(HIPL); kneeL.write(KNEEL); ankleL.write(ANKLEL);
  hipR.write(HIPR); kneeR.write(KNEER); ankleR.write(ANKLER);

  server.on("/", handleRoot);
  server.on("/setservos", handleSetServos);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);

  server.begin();
  Serial.println("Servidor web iniciado.");
}

// -------------------- LOOP --------------------
void loop() {
  server.handleClient();
}
