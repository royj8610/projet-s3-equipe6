/*
 * Projet S3 - Robot transporteur de sapin - Moitier de robot
 *
 * Programme de test de communication avec le RaspberryPi
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

//-----------------------------------------
//                Objects
//-----------------------------------------
CommJSON communication(Serial);

//-----------------------------------------
//               Variables
//-----------------------------------------
unsigned long timerEnvoi = 0;
double targetPosition = 0.0;
States robotState = States::Idle;
float angleSimule = 0.0;
float positionSimule = 0.0;

float angleCible = 0.4;
float positionCible = 0.5;

//-----------------------------------------
//              Fonctions
//-----------------------------------------
float simulateAngle()
{
    // Le système se rapproche progressivement de la cible
    angleSimule += (angleCible - angleSimule) * 0.05;

    // Petit bruit de capteur
    float bruit = random(-10, 11) / 100.0;

    return angleSimule + bruit;
}

float simulatePosition()
{
    // Le système se rapproche progressivement de la cible
    positionSimule += (positionCible - positionSimule) * 0.05;

    // Petit bruit de capteur
    float bruit = random(-10, 11) / 100.0;

    return positionSimule + bruit;
}

//-----------------------------------------
//                  Setup
//-----------------------------------------
void setup()
{
    // Init Serial monitor
    Serial.begin(115200);
    delay(1000); // Attent un peu pour le start du sérial port

    DEBUG_PRINTLN("> Init Communication Raspberry ");
    communication.init();

    randomSeed(1); // Pour que le random soit toujours le même random
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

    // Envoie de l'état robot au raspberry
    if (millis() - timerEnvoi >= DELAI_ENVOI)
    {
        DEBUG_PRINT("Send state : ");
        timerEnvoi = millis();

        CommJSON::RobotState state;

        float position = simulatePosition();
        float angle = simulateAngle();

        state.position = position;
        state.speed = 0;
        state.angle = angle;
        state.angularSpeed = 0;
        state.accelerationX = 0;
        state.slipDetected = false;
        state.state = robotState;
        state.targetPosition = targetPosition;
        state.power = 0;

        bool msgSent = communication.sendState(state);
    }
}