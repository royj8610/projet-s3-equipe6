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
#include "encoder.h"
#include "communication_json.h"
#include "board.h"
#include "magnet.h"
#include "states.h"

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
PendulumEncoder encoPendule(46); // CS pin 9, SPI speed default from library header (can pass a custom speed)
CommJSON communication(Serial);

//-----------------------------------------
//                Variables
//-----------------------------------------
unsigned long dernierEnvoi = 0;
double targetPosition = 0.0;
States robotState = States::Idle;

//-----------------------------------------
//              Main Program
//-----------------------------------------
void setup()
{
  // Init Serial monitor
  Serial.begin(115200);
  delay(1000); // Attent un peu pour le start du sérial port

  // Init encoder AS50407
  DEBUG_PRINTLN("> Init encoPendule ");
  while (!encoPendule.init())
  {
    DEBUG_PRINTLN("! Failed, retrying...");
    delay(500);
  }
  DEBUG_PRINTLN("> encoPendule sensor successfully initialized.");

  // Zeroing angle pendule
  DEBUG_PRINTLN("> Initial angle:");
  DEBUG_PRINTLN(encoPendule.readAngle());

  if (encoPendule.setZero())
  {
    DEBUG_PRINTLN("> Zero successfully configured.");
  }
  else
  {
    DEBUG_PRINTLN("! Failed to configure zero.");
  }

  delay(10);

  DEBUG_PRINTLN("> Angle after zero:");
  DEBUG_PRINTLN(encoPendule.readAngle());

  DEBUG_PRINTLN("> Init Communication Raspberry ");
  communication.init();

  // Init servo
  magnetInit();
}

void loop()
{
  DEBUG_PRINTLN(encoPendule.readAngle());
  return;

  // Lecture de la commande recu depuis le raspbrry
  if (communication.read())
  {
    const CommJSON::Command command = communication.consumeCommand();

    switch (command)
    {
    case CommJSON::Command::Start:
      robotState = States::Swing;
      break;

    case CommJSON::Command::Stop:
      robotState = States::Idle;
      break;

    case CommJSON::Command::SetTarget:
      targetPosition = communication.getTargetPosition();
      robotState = States::Stabilize;
      break;

    case CommJSON::Command::None:
    case CommJSON::Command::Invalid:
      break;
    }
  }

  // Prise des mesures
  double position = 0; // TODO
  double speed = 0;
  double angle = 0;
  double angularSpeed = 0;
  double accelerationX = 0;

  // Calcul de la commande moteur
  switch (robotState)
  {
  case States::MoveToX:
    // TODO
    break;

  case States::Stabilize:
    // TODO
    break;

  case States::Swing:
    // TODO
    break;

  case States::Idle:
    break;
  }

  // Envoie de l'état robot au raspberry
  if (millis() - dernierEnvoi >= periodeEnvoi)
  {
    DEBUG_PRINT("Send state : ");
    dernierEnvoi = millis();

    CommJSON::RobotState state;

    state.position = position; // TODO : Prendre les actual mesures
    state.speed = speed;
    state.angle = angle;
    state.angularSpeed = angularSpeed;
    state.accelerationX = accelerationX;
    state.slipDetected = false;
    state.state = robotState;
    state.targetPosition = targetPosition;

    bool msgSent = communication.sendState(state);
  }
}
