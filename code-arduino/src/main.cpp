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
const unsigned long DELAI_ENVOI = 1000; // 100 ms = 10 Hz
// k_stab: [[ 44.72135955  24.48511986 -54.18831837  -5.26798942]]
// k_goto: [[31.6227766   8.40881948  1.06721826  0.26126971]]
const float LQR_MOVE[4] = {31.6227766, 8.40881948, 1.06721826, 0.26126971};
const float LQR_STAB[4] = {44.72135955, 24.48511986, -54.18831837, -5.26798942};

//-----------------------------------------
//                Objects
//-----------------------------------------
PendulumEncoder encoPendule(46); // CS pin 9, SPI speed default from library header (can pass a custom speed)
CommJSON communication(Serial);
Motor motor = Motor();

//-----------------------------------------
//                Variables
//-----------------------------------------
unsigned long dernierEnvoi = 0;
unsigned long startTime = 0;

unsigned long timerEnvoi = 0;
unsigned long lastMeasureTime = millis();
double lastAngle = 0;

float targetAngle = 0;

double targetPosition = 0.0;
States robotState = States::Idle;

//-----------------------------------------
//                  Setup
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

  // Init motor
  Serial.print("Wait for calibration...");
  while (Serial.available() == 0)
  {
    // Do nothing, just wait
  }
  motor.init();

  motor.setControllerMode(ControlMode::TORQUE, InputMode::PASSTHROUGH);
  motor.setAxisState(AxisState::IDLE);

  Serial.print("Set robot to starting position...");
  while (Serial.available() == 0)
  {
    // Do nothing, just wait
  }
  motor.setOffset();
  motor.setAxisState(AxisState::CLOSED_LOOP_CONTROL);

  startTime = millis();
  // Init pour les mesures
  lastAngle = encoPendule.readAngle();
  lastMeasureTime = millis();

  targetAngle = encoPendule.readAngle();
}

//-----------------------------------------
//                  Loop
//-----------------------------------------
void loop()
{
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
  unsigned long currentTime = millis();
  float dt = (currentTime - lastMeasureTime) / 1000.0; // En sec

  if (motor.fetchEncoderEstimates())
  {
    // Serial.println("Connection to motor OK");
  }
  else
  {
    Serial.println("Can't establish connection");
  }

  double position = motor.getPosition(); // TODO
  double speed = motor.getVelocity();
  double angle = encoPendule.readAngle();
  double angularSpeed = (angle - lastAngle) / dt;
  double accelerationX = 0;

  lastMeasureTime = currentTime;
  lastAngle = angle;

  if (currentTime - startTime < 15000)
  {
    robotState = States::Stabilize;
  }
  else
  {
    robotState = States::Idle;
  }

  // Calcul de la commande moteur
  float goal[4] = {position - targetPosition, speed, sin(radians(angle - targetAngle)), radians(angularSpeed)};
  switch (robotState)
  {
  case States::MoveToX:
  {
    float u = LQR_MOVE[0] * goal[0] + LQR_MOVE[1] * goal[1] + LQR_MOVE[2] * goal[2] + LQR_MOVE[3] * goal[3];
    // TODO : Torque de u
    break;
  }

  case States::Stabilize:
  {
    float u = LQR_STAB[0] * goal[0] + LQR_STAB[1] * goal[1] + LQR_STAB[2] * goal[2] + LQR_STAB[3] * goal[3];

    motor.setForce(-u);

    break;
  }

  case States::Swing:
  {
    // TODO : Torque fixe (10 selon simu)
    break;
  }

  case States::Idle:
  {
    motor.setAxisState(AxisState::IDLE);
    break;
  }
  }

  // Envoie de l'état robot au raspberry
  if (millis() - timerEnvoi >= DELAI_ENVOI)
  {
    DEBUG_PRINT("Send state : ");
    timerEnvoi = millis();

    CommJSON::RobotState state;

    state.position = position;
    state.speed = speed;
    state.angle = angle;
    state.angularSpeed = angularSpeed;
    state.accelerationX = accelerationX;
    state.slipDetected = false;
    state.state = robotState;
    state.targetPosition = targetPosition;

    bool msgSent = communication.sendState(state);
  }

  // delay(50);

  // DEBUG_PRINTLN(as5047p.readAngleDegree(true));
}
