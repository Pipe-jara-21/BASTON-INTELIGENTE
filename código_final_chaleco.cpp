// Chaleco guia - control de los motores de vibracion por wifi
// Placa: ESP32 DevKit V1 (WROOM), programado en Arduino IDE
//
// La ESP32 crea su propia red wifi. Uno entra desde el celular a la pagina
// y con los botones maneja los tres motores del chaleco. Los tres funcionan
// por pulso: se oprime el boton, el motor vibra un momento y se apaga solo
// (asi no se descarga la bateria).
//   - Centro (frente/estomago): avisa "avanzar".
//   - Izquierda / Derecha: avisan el giro.

#include <WiFi.h>
#include <WebServer.h>

// pines de los tres motores (cada uno va con su transistor)
const int MOTOR_CENTRO = 19;  // D19 - al frente, sobre el estomago (avanzar)
const int MOTOR_DER    = 21;  // D21 - lado derecho
const int MOTOR_IZQ    = 18;  // D18 - lado izquierdo

// cuanto dura la vibracion cada vez que se oprime un boton (ms)
const unsigned long DURACION = 800;

// datos de la red que crea la ESP32
const char* ssid = "Chaleco_Guia";
const char* password = "12345678";   // minimo 8 caracteres

WebServer server(80);

// para apagar solos los motores sin frenar el programa
unsigned long tCen = 0;  bool cenOn = false;
unsigned long tDer = 0;  bool derOn = false;
unsigned long tIzq = 0;  bool izqOn = false;

// aca se arma la pagina web con los botones
String pagina() {
  String h = "<!DOCTYPE html><html lang='es'><head><meta charset='UTF-8'>";
  h += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  h += "<title>Chaleco Guia</title><style>";
  h += "body{font-family:sans-serif;background:#0f172a;color:#e2e8f0;";
  h += "text-align:center;margin:0;padding:32px 16px;}";
  h += "h1{font-size:1.5rem;margin-bottom:4px;} p{color:#94a3b8;margin-top:0;}";
  h += "button{width:100%;max-width:360px;padding:28px 0;margin:8px auto;display:block;";
  h += "font-size:1.3rem;font-weight:bold;border:none;border-radius:18px;color:#fff;cursor:pointer;}";
  h += ".avanzar{background:#16a34a;}";
  h += ".fila{display:flex;gap:12px;max-width:360px;margin:8px auto;}";
  h += ".fila button{margin:0;} .izq{background:#2563eb;} .der{background:#7c3aed;}";
  h += "button:active{opacity:.6;}";
  h += "</style></head><body>";
  h += "<h1>Chaleco Guia</h1><p>Control del guia</p>";
  h += "<button class='avanzar' onclick=\"fetch('/avanzar')\">AVANZAR</button>";
  h += "<div class='fila'>";
  h += "<button class='izq' onclick=\"fetch('/izquierda')\">IZQUIERDA</button>";
  h += "<button class='der' onclick=\"fetch('/derecha')\">DERECHA</button>";
  h += "</div></body></html>";
  return h;
}

void inicio() { server.send(200, "text/html", pagina()); }

void avanzar() {                        // pulso del motor del centro
  digitalWrite(MOTOR_CENTRO, HIGH);
  cenOn = true;  tCen = millis();
  server.send(200, "text/plain", "ok");
}

void izquierda() {                      // pulso de aviso a la izquierda
  digitalWrite(MOTOR_IZQ, HIGH);
  izqOn = true;  tIzq = millis();
  server.send(200, "text/plain", "ok");
}

void derecha() {                        // pulso de aviso a la derecha
  digitalWrite(MOTOR_DER, HIGH);
  derOn = true;  tDer = millis();
  server.send(200, "text/plain", "ok");
}

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_CENTRO, OUTPUT);
  pinMode(MOTOR_DER, OUTPUT);
  pinMode(MOTOR_IZQ, OUTPUT);
  digitalWrite(MOTOR_CENTRO, LOW);
  digitalWrite(MOTOR_DER, LOW);
  digitalWrite(MOTOR_IZQ, LOW);

  WiFi.softAP(ssid, password);     // enciende la red del chaleco
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP()); // casi siempre 192.168.4.1

  server.on("/", inicio);
  server.on("/avanzar", avanzar);
  server.on("/izquierda", izquierda);
  server.on("/derecha", derecha);
  server.begin();
}

void loop() {
  server.handleClient();

  // apaga cada motor cuando ya paso su tiempo de vibracion
  if (cenOn && millis() - tCen >= DURACION) { digitalWrite(MOTOR_CENTRO, LOW); cenOn = false; }
  if (izqOn && millis() - tIzq >= DURACION) { digitalWrite(MOTOR_IZQ, LOW); izqOn = false; }
  if (derOn && millis() - tDer >= DURACION) { digitalWrite(MOTOR_DER, LOW); derOn = false; }
}

