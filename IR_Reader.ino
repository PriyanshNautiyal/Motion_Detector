#include <IRremote.hpp>

const int IR_PIN = 15;

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(IR_PIN);
  Serial.println("Point the remote at the receiver and press a button...");
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.print("Code: 0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    IrReceiver.resume();
  }
}