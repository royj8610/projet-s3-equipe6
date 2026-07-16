#include <Arduino.h>
#include <encoder.h>
#include <encoderAlt.h>

//     imu_init();
// }

// void loop() {
//     imu_afficherAcceleration();
//     delay(300);
// }


#include <Arduino.h>
#include "communication_json.h"
unsigned long dernierEnvoi = 0;
const unsigned long periodeEnvoi = 100; // 100 ms = 10 Hz
void setup() {
   initialiserCommunicationJson();
  Serial.begin(115200);
}
void loop() {
   lireCommandeDuRaspberry();
   // Plus tard ici :
   // lire IMU
   // lire encodeurs
   // faire calculs
   // contrôler moteur
   if (millis() - dernierEnvoi >= periodeEnvoi) {
       dernierEnvoi = millis();
       envoyerEtatAuRaspberry();
   }
}
  Serial.print(encoderAltGetAngle());
}
