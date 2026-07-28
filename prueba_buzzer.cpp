// Prueba del buzzer
#define BUZZER 4

void setup() {
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  digitalWrite(BUZZER, HIGH);  // suena
  delay(200);
  digitalWrite(BUZZER, LOW);   // calla
  delay(800);
}
