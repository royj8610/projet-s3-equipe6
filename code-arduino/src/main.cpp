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
// #define DEBUG // Commenter pour retirer le mode debug

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
const unsigned long DELAI_ENVOI = 100; // 100 ms = 10 Hz
// k_stab : [[ 44.72135955  24.30961974 -53.24889054  -5.29367469]]
// k_goto : [[31.6227766  12.38004435  0.8421009   0.08961256]]
const float LQR_MOVE[4] = {31.6227766, 8.40881948, 1.06721826, 0.26126971};
const float LQR_BACK[4] = {31.6227766, 12.38004435, 0.8421009, 0.08961256};
const float LQR_STAB[4] = {44.72135955, 24.30961974, -53.24889054, -5.29367469};

const float L_ROD = 0.25;
const float CLEARANCE = 0.015;

const float TOL = 0.1;
const float POS_TOL = 0.04;

const float OBSTACLE_OFFSET = 0.06;

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

bool isErrorState = false;

int nbTree = 0;

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
  motor.init();

  motor.setControllerMode(ControlMode::TORQUE, InputMode::PASSTHROUGH);
  motor.setAxisState(AxisState::IDLE);

  delay(100);

  motor.setOffset();
  motor.setForce(0.0);

  delay(100);

  startTime = millis();
  // Init pour les mesures
  lastAngle = encoPendule.readAngleRad();
  lastMeasureTime = millis();

  targetAngle = encoPendule.readAngleRad();

  nbTree = 0;

  DEBUG_PRINTLN("Ready to start");
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
    case CommJSON::Command::Swing:
    {
      // robotState = States::Swing;
      break;
    }
    case CommJSON::Command::Stop:
    {
      if (robotState != States::Idle)
      {
        motor.setAxisState(AxisState::IDLE);
        magnet.retract();
      }

      robotState = States::Idle;
      break;
    }
    case CommJSON::Command::Stabilize:
    {
      if (robotState == States::Idle)
      {
        motor.setAxisState(AxisState::CLOSED_LOOP_CONTROL);
        robotState = States::Stabilize;
        targetPosition = 0.5 + OBSTACLE_OFFSET;
      }
      // targetPosition = communication.getTargetPosition();
      // robotState = States::Stabilize;
      break;
    }
    case CommJSON::Command::MoveBack:
    {
      // if (robotState != States::MoveBack)
      // {
      //   magnet.retract();
      // }

      // targetPosition = communication.getTargetPosition();
      // robotState = States::MoveBack;
      break;
    }
    case CommJSON::Command::Drop:
    {
      // if (robotState != States::Drop)
      // {
      //   magnet.extend();
      // }

      // robotState = States::Drop;
      break;
    }
    case CommJSON::Command::None:
    case CommJSON::Command::Invalid:
    {
      break;
    }
    }
  }

  // Prise des mesures
  unsigned long currentTime = millis();
  float dt = (currentTime - lastMeasureTime) / 1000.0; // En sec

  if (motor.fetchEncoderEstimates())
  {
    if (isErrorState)
    {
      isErrorState = false;

      communication.sendError("Regained connection");
    }
    isErrorState = false;
    // Serial.println("Connection to motor OK");
  }
  else
  {
    if (!isErrorState)
    {
      isErrorState = true;

      communication.sendError("Lost connection with controller");
    }
  }

  double position = motor.getPosition();
  double speed = motor.getVelocity();
  double angle = encoPendule.readAngleRad();
  double angularSpeed = (angle - lastAngle) / dt;
  double accelerationX = 0;

  // Get power
  motor.fetchVoltageCurrent();
  double power = motor.getElectricalPower();

  lastMeasureTime = currentTime;
  lastAngle = angle;

  if (robotState == States::Stabilize && targetPosition == (0.5 + OBSTACLE_OFFSET) * MOTOR_SIGN && position * MOTOR_SIGN > (0.35 + OBSTACLE_OFFSET))
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
  else if (robotState == States::Swing && position * MOTOR_SIGN > (0.62 + OBSTACLE_OFFSET))
  {
    robotState = States::Stabilize;
    targetPosition = TARGET;
  }
  else if (robotState == States::Stabilize && checkTol(angle, targetAngle, TOL) && checkTol(angularSpeed, 0, TOL) && checkTol(position, targetPosition, POS_TOL))
  {
    magnet.extend();
    robotState = States::Drop;
    targetPosition = TARGET;
    nbTree++;
  }
  else if (robotState == States::Drop && magnet.isExtended())
  {
    magnet.retract();
    robotState = States::MoveBack;
    targetPosition = -0.02;
  }
  else if (robotState == States::MoveBack && checkTol(position, 0.0, POS_TOL) && checkTol(speed, 0.0, POS_TOL))
  {
    robotState = States::Idle;
    motor.setAxisState(AxisState::IDLE);
  }

  // Calcul de la commande moteur
  float goal[4] = {position - targetPosition, speed, sin(angle - targetAngle), angularSpeed};
  switch (robotState)
  {
  case States::MoveBack:
  {
    float u = LQR_BACK[0] * goal[0] + LQR_BACK[1] * goal[1] + LQR_BACK[2] * goal[2] + LQR_BACK[3] * goal[3];

    if (isErrorState)
    {
      motor.setForce(0.0);
    }
    else
    {
      motor.setForce(-u);
    }

    break;
  }

  case States::Stabilize:
  case States::Drop:
  {
    float u = LQR_STAB[0] * goal[0] + LQR_STAB[1] * goal[1] + LQR_STAB[2] * goal[2] + LQR_STAB[3] * goal[3];

    if (isErrorState)
    {
      motor.setForce(0.0);
    }
    else
    {
      motor.setForce(-u);
    }

    break;
  }

  case States::Swing:
  {
    if (isErrorState)
    {
      motor.setForce(0.0);
    }
    else
    {
      motor.setForce(100 * MOTOR_SIGN);
    }

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
    state.angle = angle - targetAngle;
    state.angularSpeed = angularSpeed;
    state.accelerationX = accelerationX;
    state.slipDetected = false;
    state.state = robotState;
    state.targetPosition = targetPosition;
    state.power = 0;
    state.magnetExtended = magnet.isExtended();
    state.nbTree = nbTree;

    bool msgSent = communication.sendState(state);
  }

} // Loop end
