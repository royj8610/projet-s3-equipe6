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
const float LQR_BACK[4] = {31.6227766, 7.98244291, 0.77851821, 0.1250544};
const float LQR_STAB[4] = {44.72135955, 24.48511986, -54.18831837, -5.26798942};

const float L_ROD = 0.25;
const float CLEARANCE = 0.015;

const float TOL = 0.05;

const float TARGET = 1.2 * MOTOR_SIGN;

//-----------------------------------------
//                Objects
//-----------------------------------------
PendulumEncoder encoPendule(AS5047P_CS_PIN); // CS pin 9, SPI speed default from library header (can pass a custom speed)
CommJSON communication(Serial);
Motor motor = Motor();
Magnet magnet = Magnet();

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
//                Fonctions
//-----------------------------------------
bool checkTol(float val, float target, float tol)
{
  return val <= target + tol && val >= target - tol;
}

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
  magnet.init();

  // Init motor
  Serial.println("Wait for calibration...");
  while (Serial.available() == 0)
  {
    // Do nothing, just wait
  }
  Serial.read();
  motor.init();

  motor.setControllerMode(ControlMode::TORQUE, InputMode::PASSTHROUGH);
  motor.setAxisState(AxisState::IDLE);

  delay(500);

  Serial.println("Set robot to starting position...");
  Serial.read();
  while (Serial.available() == 0)
  {
    // Do nothing, just wait
  }
  motor.setOffset();
  motor.setForce(0.0);

  delay(500);

  startTime = millis();
  // Init pour les mesures
  lastAngle = encoPendule.readAngleRad();
  lastMeasureTime = millis();

  targetAngle = encoPendule.readAngleRad();

  Serial.println("Ready to start");
}

//-----------------------------------------
//                  Loop
//-----------------------------------------
void loop()
{
  // Lecture de la commande recu depuis le raspbrry
  // if (communication.read())
  // {
  //   const CommJSON::Command command = communication.consumeCommand();

  //   switch (command)
  //   {
  //   case CommJSON::Command::Start:
  //     robotState = States::Swing;
  //     break;

  //   case CommJSON::Command::Stop:
  //     robotState = States::Idle;
  //     break;

  //   case CommJSON::Command::SetTarget:
  //     targetPosition = communication.getTargetPosition();
  //     robotState = States::Stabilize;
  //     break;

  //   case CommJSON::Command::None:
  //   case CommJSON::Command::Invalid:
  //     break;
  //   }
  // }

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

  double position = motor.getPosition();
  double speed = motor.getVelocity();
  double angle = encoPendule.readAngleRad();
  double angularSpeed = (angle - lastAngle) / dt;
  double accelerationX = 0;

  // Get power
  motor.fetchVoltageCurrent();
  double power = motor.getElectricalPower();

  // Update magnet state
  magnet.update();

  lastMeasureTime = currentTime;
  lastAngle = angle;

  // State machine temporaire - Condition depuis la state machine
  Serial.read();
  if (Serial.available() && robotState == States::Idle)
  {
    // motor.setOffset();
    delay(100);
    motor.setAxisState(AxisState::CLOSED_LOOP_CONTROL);
    robotState = States::Stabilize;
    targetPosition = 0.5 * MOTOR_SIGN;
  }
  else if (Serial.available() && robotState != States::Idle)
  {
    Serial.println("Bailed out");
    motor.setAxisState(AxisState::IDLE);
    motor.setForce(0.0);
    robotState = States::Idle;
    delay(500);
  }
  else if (robotState == States::Stabilize && targetPosition == 0.5 * MOTOR_SIGN && position * MOTOR_SIGN > 0.3)
  {
    robotState = States::Swing;
  }
  // && angle > 1cm au dessus de obstacle L - L*np.cos(theta) > self.height_target and theta < 0 and dtheta <= 0
  else if (robotState == States::Swing && (L_ROD - L_ROD * cos(angle - targetAngle) > CLEARANCE) && (angle < targetAngle) && (angularSpeed <= 0))
  {
    robotState = States::Stabilize;
    targetPosition = TARGET;
  }
  // && angle > 1cm au dessus de obstacle L - L*np.cos(theta) > self.height_target and theta < 0 and dtheta <= 0
  else if (robotState == States::Swing && position * MOTOR_SIGN > 0.6)
  {
    robotState = States::Stabilize;
    targetPosition = TARGET;
  }
  else if (robotState == States::MoveToX && targetPosition == TARGET && position * MOTOR_SIGN >= 0.7)
  {
    robotState = States::Stabilize;
  }
  else if (robotState == States::Stabilize && checkTol(angle, targetAngle, TOL) && checkTol(angularSpeed, 0, TOL) && checkTol(position, targetPosition, 0.04))
  {
    magnet.detach();
    robotState = States::MoveBack;
    targetPosition = TARGET;
  }
  else if (robotState == States::MoveBack && checkTol(position, TARGET, 0.04) && !magnet.isExtended())
  {
    robotState = States::MoveBack;
    targetPosition = 0.0;
  }
  else if (robotState == States::MoveBack && checkTol(position, 0.0, 0.04) && checkTol(speed, 0.0, 0.04))
  {
    robotState = States::Idle;
    motor.setAxisState(AxisState::IDLE);
  }

  // Calcul de la commande moteur
  float goal[4] = {position - targetPosition, speed, sin(angle - targetAngle), angularSpeed};
  switch (robotState)
  {
  case States::MoveToX:
  {
    float u = LQR_MOVE[0] * goal[0] + LQR_MOVE[1] * goal[1] + LQR_MOVE[2] * goal[2] + LQR_MOVE[3] * goal[3];

    motor.setForce(-u);

    break;
  }

  case States::MoveBack:
  {
    float u = LQR_BACK[0] * goal[0] + LQR_BACK[1] * goal[1] + LQR_BACK[2] * goal[2] + LQR_BACK[3] * goal[3];

    motor.setForce(-u);

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
    motor.setForce(100 * MOTOR_SIGN);

    break;
  }

  case States::Idle:
  {
    motor.setForce(0.0);
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
    state.power = 0;

    bool msgSent = communication.sendState(state);
  }

} // Loop end
