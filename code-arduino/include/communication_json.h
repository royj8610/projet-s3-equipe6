#ifndef COMM_JSON_H
#define COMM_JSON_H
#include <Arduino.h>
#include "states.h"

class CommJSON
{
public:
    enum class Command
    {
        None,
        Start,
        Stop,
        SetTarget,
        Invalid
    };

    struct RobotState
    {
        double position = 0.0;
        double speed = 0.0;
        double angle = 0.0;
        double angularSpeed = 0.0;
        double accelerationX = 0.0;
        bool slipDetected = false;
        States state = States::Idle;
        double targetPosition = 0.0;
        double power = 0.0;
    };

    // Constructeur
    CommJSON(Stream &serial);
    bool init();

    // Retourne true lorsqu'un message complet a été reçu et traité.
    bool read();

    // Dernière commande reçue.
    Command getCommand() const;

    // Remet la commande à None après traitement.
    Command consumeCommand();

    double getTargetPosition() const;

    bool sendState(const RobotState &state);

private:
    Stream &_serial;

    Command _command = Command::None;
    double _targetPosition = 0.0;

    bool parseMessage(const String &message);
    void sendError(const char *message);
};

#endif // COMM_JSON_H