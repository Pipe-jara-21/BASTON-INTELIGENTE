// Prueba de un motor de vibracion
#define MOTOR 25

void setup() {
  pinMode(MOTOR, OUTPUT);
}

void loop() {
  digitalWrite(MOTOR, HIGH);  // vibra
  delay(500);
  digitalWrite(MOTOR, LOW);
  delay(1000);
}
