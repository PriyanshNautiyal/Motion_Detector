// Pin  Variables
int ledPin = 8;
int buzzerPin = 10;
int tiltPin = 9;

void setup() {
  // setup pins
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(tiltPin, INPUT_PULLUP);

  // LED and buzzer off

  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW); 
}

void loop() {
  delay(200);
  if (digitalRead(tiltPin) == HIGH) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
  }
}