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
#include "board.h"
#include "magnet.h"

//-----------------------------------------
//                 Defines
//-----------------------------------------
#define DEBUG // Commenter pour retirer le mode debug

#ifdef DEBUG // Crée des macros pour faire rapidement disparaitre les prints de débug
  #define DEBUG_PRINT(x)     Serial.print(x)
  #define DEBUG_PRINTLN(x)   Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

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
  DEBUG_PRINT("> Init AS5047P ");
  while (!as5047p.initSPI()) {
    DEBUG_PRINT(".");
    delay(500); 
  }
  DEBUG_PRINTLN("> AS5047P sensor successfully initialized.");

  initialiserCommunicationJson();
  
  // Init servo
  magnetInit();
}

void loop(){
  lireCommandeDuRaspberry();

  if (millis() - dernierEnvoi >= periodeEnvoi)
  {
    dernierEnvoi = millis();
    envoyerEtatAuRaspberry();
  }

  DEBUG_PRINTLN(as5047p.readAngleDegree(true));
}
