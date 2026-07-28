// --- CONFIGURACIÓN DEL PULSADOR (ON/OFF) ---
const int botonPin = 4;
bool sistemaActivo = true;      
int ultimoEstadoBoton = HIGH;   

// --- CONFIGURACIÓN SENSOR SUPERIOR (RAMAS / VIBRACIÓN) ---
const int trigSup = 5;
const int echoSup = 18;
const int motorPin = 23; 

// --- CONFIGURACIÓN SENSOR INFERIOR (SUELO / SONIDO) ---
const int trigInf = 19;
const int echoInf = 21;
const int buzzerPin = 22; 

// --- VARIABLES DE MUESTREO DE SENSORES ---
unsigned long tiempoAnteriorSensores = 0;
const long intervaloSensores = 60; // Muestreo base

// --- VARIABLES PARA INTERMITENCIA DEL MOTOR ---
unsigned long tiempoAnteriorMotor = 0;
bool estadoMotor = false;
int intervaloMotor = -1; // -1 significa apagado

// --- VARIABLES PARA INTERMITENCIA DEL BUZZER ---
unsigned long tiempoAnteriorBuzzer = 0;
bool estadoBuzzer = false;
int intervaloBuzzer = -1; // -1 significa apagado

void setup() {
  Serial.begin(115200);
  
  pinMode(botonPin, INPUT_PULLUP);
  
  pinMode(trigSup, OUTPUT);
  pinMode(echoSup, INPUT);
  pinMode(motorPin, OUTPUT); 
  
  pinMode(trigInf, OUTPUT);
  pinMode(echoInf, INPUT);
  pinMode(buzzerPin, OUTPUT);
  
  Serial.println("Sistema Iniciado con Nuevos Rangos (Sup: 1m, Inf: 1.5m).");
}

void loop() {
  // --- 1. GESTIÓN DEL PULSADOR ---
  int estadoBoton = digitalRead(botonPin);
  if (estadoBoton == LOW && ultimoEstadoBoton == HIGH) {
    sistemaActivo = !sistemaActivo;
    delay(200); // Anti-rebote
  }
  ultimoEstadoBoton = estadoBoton;

  // --- 2. RUTINA PRINCIPAL ---
  if (sistemaActivo) {
    unsigned long tiempoActual = millis();

    // Bloque A: Adquisición de distancias
    if (tiempoActual - tiempoAnteriorSensores >= intervaloSensores) {
      tiempoAnteriorSensores = tiempoActual;

      // Lectura Sensor Superior
      long distSup = medirDistancia(trigSup, echoSup);
      delay(30); // Pausa acústica para evitar interferencia entre sensores
      // Lectura Sensor Inferior
      long distInf = medirDistancia(trigInf, echoInf);

      // --- CÁLCULO DE RITMO PARA EL MOTOR (Superior - LÍMITE: 1 METRO) ---
      // Si la distancia es mayor a 2cm y menor o igual a 100cm (1 metro)
      if (distSup > 2 && distSup <= 100) { 
        // Mapeamos el ritmo: de muy rápido (50ms) a muy cerca, hasta lento (600ms) a 1 metro
        intervaloMotor = map(distSup, 2, 100, 50, 600); 
        intervaloMotor = constrain(intervaloMotor, 50, 600);
      } else {
        intervaloMotor = -1; // Apagado si está fuera de rango
      }

      // --- CÁLCULO DE RITMO PARA EL BUZZER (Inferior - LÍMITE: 1.5 METROS) ---
      // Si la distancia es mayor a 2cm y menor o igual a 150cm (1.5 metros)
      if (distInf > 2 && distInf <= 150) {
        // Mapeamos el ritmo: de muy rápido (50ms) a muy cerca, hasta lento (600ms) a 1.5 metros
        intervaloBuzzer = map(distInf, 2, 150, 50, 600); 
        intervaloBuzzer = constrain(intervaloBuzzer, 50, 600);
      } else {
        intervaloBuzzer = -1; // Apagado si está fuera de rango
      }
    }

    // Bloque B: Ejecución de la vibración del Motor
    if (intervaloMotor != -1) {
      if (tiempoActual - tiempoAnteriorMotor >= intervaloMotor) {
        tiempoAnteriorMotor = tiempoActual;
        estadoMotor = !estadoMotor; 
        
        if (estadoMotor) {
          digitalWrite(motorPin, HIGH); 
        } else {
          digitalWrite(motorPin, LOW);
        }
      }
    } else {
      digitalWrite(motorPin, LOW);
      estadoMotor = false;
    }

    // Bloque C: Ejecución de la intermitencia del Buzzer
    if (intervaloBuzzer != -1) {
      if (tiempoActual - tiempoAnteriorBuzzer >= intervaloBuzzer) {
        tiempoAnteriorBuzzer = tiempoActual;
        estadoBuzzer = !estadoBuzzer; 
        
        if (estadoBuzzer) {
          tone(buzzerPin, 1000); 
        } else {
          noTone(buzzerPin);     
        }
      }
    } else {
      noTone(buzzerPin);
      estadoBuzzer = false;
    }

  } else {
    // Apagado de seguridad
    digitalWrite(motorPin, LOW);
    noTone(buzzerPin);
    estadoMotor = false;
    estadoBuzzer = false;
  }
}

// --- SUBRUTINA DE ADQUISICIÓN ---
long medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duracion = pulseIn(echoPin, HIGH, 30000); 
  if (duracion == 0) return 999; 
  
  return (duracion * 0.034 / 2);
}

