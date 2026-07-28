// Prueba del sensor ultrasonico HC-SR04
#define TRIG 5
#define ECHO 18

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {
  // mandar el pulso de disparo
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // medir cuanto tarda en volver el eco
  long t = pulseIn(ECHO, HIGH);
  float dist = t * 0.0343 / 2;   // pasar el tiempo a cm

  Serial.print(dist);
  Serial.println(" cm");
  delay(300);
}
