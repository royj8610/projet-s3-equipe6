/*
 * Projet S3 - Robot transporteur de sapin - Moitier de robot
 *
 * Programme principal pour le Arduino
 *
 */

//-----------------------------------------
//                 IMPORTS
//-----------------------------------------
#include <Arduino.h>
#include <SPI.h>
#include <AS5047P.h>
#include "encoder.h"
#include "communication_json.h"

//-----------------------------------------
//               Constantes
//-----------------------------------------
const unsigned long periodeEnvoi = 100; // 100 ms = 10 Hz

//-----------------------------------------
//                Objects
//-----------------------------------------
// CS pin 9, SPI speed default from library header (can pass a custom speed)
AS5047P as5047p(9);

//-----------------------------------------
//                Variables
//-----------------------------------------
unsigned long dernierEnvoi = 0;

//-----------------------------------------
//              Main Program
//-----------------------------------------
void setup()
{
  // Init Serial monitor
  Serial.begin(115200);
  delay(1000); // Attent un peu pour le start du sérial port

  // Init encoder AS50407
  Serial.print("> Init AS5047P ");
  while (!as5047p.initSPI()) {
    Serial.print(".");
    delay(500); 
  }
  Serial.println(F("> AS5047P sensor successfully initialized."));

  initialiserCommunicationJson();
  
}

void loop(){
  lireCommandeDuRaspberry();

  if (millis() - dernierEnvoi >= periodeEnvoi)
  {
    dernierEnvoi = millis();
    envoyerEtatAuRaspberry();
  }

  Serial.print(as5047p.readAngleDegree(true));
}
