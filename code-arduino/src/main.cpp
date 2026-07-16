/*
 * Projet S3 - Robot transporteur de sapin - Moitier de robot
 *
 * Programme principal pour le Arduino
 *
 */

#include <Arduino.h>
#include <encoder.h>
#include <encoderAlt.h>
#include "communication_json.h"

unsigned long dernierEnvoi = 0;
const unsigned long periodeEnvoi = 100; // 100 ms = 10 Hz
void setup()
{
  initialiserCommunicationJson();
  Serial.begin(115200);
}

void loop(){
  lireCommandeDuRaspberry();

  if (millis() - dernierEnvoi >= periodeEnvoi)
  {
    dernierEnvoi = millis();
    envoyerEtatAuRaspberry();
  }
  
  Serial.print(encoderAltGetAngle());
}
