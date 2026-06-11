#include <Arduino.h>
#include <encoder.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.print(getAngle());
}