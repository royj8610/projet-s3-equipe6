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
#include <AS5047P.h>
#include "encoder.h"
#include "communication_json.h"
#include "board.h"
#include "magnet.h"
#include "motor.h"

//-----------------------------------------
//                 Defines
//-----------------------------------------
#define DEBUG // Commenter pour retirer le mode debug

#ifdef DEBUG // Crée des macros pour faire rapidement disparaitre les prints de débug
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

//-----------------------------------------
//               Constantes
//-----------------------------------------
const unsigned long periodeEnvoi = 1000; // 100 ms = 10 Hz

//-----------------------------------------
//                Objects
//-----------------------------------------
// CS pin 9, SPI speed default from library header (can pass a custom speed)
AS5047P as5047p(AS5047P_CS_PIN);
CommJSON communication(Serial);
Motor motor = Motor();

//-----------------------------------------
//                Variables
//-----------------------------------------
unsigned long dernierEnvoi = 0;
double targetPosition = 0.0;
String robotState = "IDLE";

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
  // while (!as5047p.initSPI())
  // {
  //   DEBUG_PRINT(".");
  //   delay(500);
  // }
  DEBUG_PRINTLN("> AS5047P sensor successfully initialized.");

  DEBUG_PRINTLN("> Init Communication Raspberry ");
  communication.init();

  // Init servo
  magnetInit();

  // Init motor
  motor.init();
}

void loop()
{
  // Lecture de la commande recu depuis le raspbrry
  if (communication.read())
  {
    const CommJSON::Command command = communication.consumeCommand();

    switch (command)
    {
    case CommJSON::Command::Start:
      robotState = "MOVE";
      break;

    case CommJSON::Command::Stop:
      robotState = "IDLE";
      break;

    case CommJSON::Command::SetTarget:
      targetPosition = communication.getTargetPosition();
      robotState = "MOVE_TO_TARGET";
      break;

    case CommJSON::Command::None:
    case CommJSON::Command::Invalid:
      break;
    }
  }

  if (millis() - dernierEnvoi >= periodeEnvoi)
  {
    DEBUG_PRINT("Send state : ");
    dernierEnvoi = millis();

    CommJSON::RobotState state;

    state.position = 10.5; // TODO : Prendre les actual mesures
    state.speed = 1.2;
    state.angle = 0.15;
    state.angularSpeed = 0.02;
    state.accelerationX = 0.4;
    state.slipDetected = false;
    state.state = robotState;
    state.targetPosition = targetPosition;

    bool msgSent = communication.sendState(state);
  }

  if (motor.fetchEncoderEstimates())
  {
    DEBUG_PRINT("Position : ");
    DEBUG_PRINT(motor.getPosition());
    DEBUG_PRINT(", Velocity : ");
    DEBUG_PRINTLN(motor.getVelocity());
  }

  delay(100);

  // DEBUG_PRINTLN(as5047p.readAngleDegree(true));
}
