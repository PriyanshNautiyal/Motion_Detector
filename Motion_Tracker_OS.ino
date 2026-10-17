#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <IRremote.hpp>

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

const int TRIG_PIN = 5;
const int ECHO_PIN = 18;
const int GREEN_LED = 2;
const int RED_LED = 4;
const int IR_PIN = 15;

// IR remote codes
const unsigned long IR_POWER_CODE = 0xBA45FF00;      // put in YOUR code for power button
const unsigned long IR_CALIBRATE_CODE = 0xE916FF00;  // put in YOUR code for "0" button

const float CHANGE_THRESHOLD_CM = 3.0;
const unsigned long NOTIFY_COOLDOWN_MS = 10000;  // don't spam notifications (10 second cooldown)

bool systemOn = true;
float baselineDistance = 0;
unsigned long lastNotifyTime = 0;

float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  return (duration * 0.0343) / 2.0;
}

void calibrate() {
  Serial.println("Calibrating...");
  float total = 0;
  int validReadings = 0;
  for (int i = 0; i < 5; i++) {
    float d = readDistanceCM();
    if (d > 0) {
      total += d;
      validReadings++;
    }
    delay(100);
  }
  if (validReadings > 0) {
    baselineDistance = total / validReadings;
    Serial.print("New baseline distance: ");
    Serial.print(baselineDistance);
    Serial.println(" cm");
  } else {
    Serial.println("Calibration failed - no valid readings. Check wiring.");
  }
}

void togglePower() {
  systemOn = !systemOn;
  Serial.println(systemOn ? "System turned ON" : "System turned OFF");
  digitalWrite(GREEN_LED, systemOn ? HIGH : LOW);
  digitalWrite(RED_LED, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  IrReceiver.begin(IR_PIN);

  Serial.println("Connecting to WiFi and Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Connected!");

  calibrate();
  digitalWrite(GREEN_LED, HIGH);
}

void loop() {
  Blynk.run();

  // IR remote
  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;

    if (code == IR_POWER_CODE) {
      togglePower();
    } else if (code == IR_CALIBRATE_CODE) {
      calibrate();
    }

    IrReceiver.resume();
  }

  // Motion check
  if (systemOn) {
    float distance = readDistanceCM();

    if (baselineDistance > 0 && distance > 0 && abs(distance - baselineDistance) > CHANGE_THRESHOLD_CM) {
      Serial.print("MOTION DETECTED! Distance: ");
      Serial.print(distance);
      Serial.println(" cm");

      unsigned long now = millis();
      if (now - lastNotifyTime > NOTIFY_COOLDOWN_MS) {
        Blynk.logEvent("motion_detected", "Motion Has Been Detected!");
        lastNotifyTime = now;
      }

      for (int i = 0; i < 4; i++) {
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, HIGH);
        delay(150);
        digitalWrite(RED_LED, LOW);
        delay(150);
      }
      digitalWrite(GREEN_LED, HIGH);
    }
  }

  delay(100);
}