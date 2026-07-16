#include <Arduino.h>
#include <encoder.h>
#include <encoderAlt.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.print(encoderAltGetAngle());
}