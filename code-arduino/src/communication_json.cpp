#include <Arduino.h>
#include <ArduinoJson.h>
#include "communication_json.h"
#include "states.h"

//-----------------------------------------
//             Constructeurs
//-----------------------------------------
/**
 * @brief Constructeur
 */
CommJSON::CommJSON(Stream &serial)
    : _serial(serial)
{
}

//-----------------------------------------
//                Publics
//-----------------------------------------
/**
 * @brief Établit la communication avec l'autre appareil
 * @return bool : init réussi
 */
bool CommJSON::init()
{
    StaticJsonDocument<128> documentJson;

    documentJson["type"] = "status";
    documentJson["ready"] = true;

    serializeJson(documentJson, _serial);
    _serial.println();

    return true;
}

/**
 * @brief Lecture du port de communication
 * @return true: une nouvelle communication a été recue, false: no new comm
 */
bool CommJSON::read()
{
    if (_serial.available() <= 0)
    {
        // Rien recu
        return false;
    }

    // Récupère le message au complet
    const String message = _serial.readStringUntil('\n');
    if (message.length() == 0)
    {
        return false;
    }

    return parseMessage(message);
}

/**
 * @brief Récupère la dernière commande
 * @return Command : la commande recue
 */
CommJSON::Command CommJSON::getCommand() const
{
    return _command;
}

/**
 * @brief Récupère et consomme la dernière commande
 * @return Command : la commande recue
 */
CommJSON::Command CommJSON::consumeCommand()
{
    const Command command = _command;
    _command = Command::None;

    return command;
}

/**
 * @brief Récupère le target de position
 * @return double : valeur du target
 */
double CommJSON::getTargetPosition() const
{
    return _targetPosition;
}

/**
 * @brief Envoie l'état du robot à l'autre appareil
 * @return bool : byte envoyés?
 */
bool CommJSON::sendState(const RobotState &state)
{
    StaticJsonDocument<384> documentJson;

    documentJson["type"] = "robot_state";
    documentJson["time"] = millis();
    documentJson["position"] = state.position;
    documentJson["speed"] = state.speed;
    documentJson["angle"] = state.angle;
    documentJson["angular_speed"] = state.angularSpeed;
    // documentJson["accel_x"] = state.accelerationX;
    documentJson["state"] = stateToString(state.state);
    // documentJson["slip"] = state.slipDetected;
    documentJson["target"] = state.targetPosition;
    documentJson["power"] = state.power;
    documentJson["extended"] = state.magnetExtended;
    documentJson["nb_tree"] = state.nbTree;

    const size_t bytesWritten = serializeJson(documentJson, _serial);

    _serial.println();

    return bytesWritten > 0;
}

//-----------------------------------------
//                Privates
//-----------------------------------------
/**
 * @brief Décompose un message JSON recu en donnée utilisable
 */
bool CommJSON::parseMessage(const String &message)
{
    StaticJsonDocument<256> documentJson;

    const DeserializationError error = deserializeJson(documentJson, message);

    // Json a une erreur?
    if (error)
    {
        _command = Command::Invalid;
        sendError("JSON invalide");
        return false;
    }

    // Est ce que le champ "cmd" exist?
    const char *commandText = documentJson["cmd"];
    if (commandText == nullptr)
    {
        _command = Command::Invalid;
        sendError("Commande absente");
        return false;
    }

    // On trouve la bonne commande
    if (strcmp(commandText, "SWING") == 0)
    {
        _command = Command::Swing;
        return true;
    }
    if (strcmp(commandText, "STOP") == 0 || strcmp(commandText, "IDLE") == 0)
    {
        _command = Command::Stop;
        return true;
    }

    if (strcmp(commandText, "DROP") == 0)
    {
        _command = Command::Drop;
        return true;
    }

    if (strcmp(commandText, "MOVE_BACK") == 0)
    {
        if (!documentJson["x_target"].is<double>())
        {
            _command = Command::Invalid;
            sendError("Position cible absente ou invalide");
            return false;
        }

        _targetPosition = documentJson["x_target"].as<double>();
        _command = Command::MoveBack;
        return true;
    }

    if (strcmp(commandText, "STABILIZE") == 0)
    {
        if (!documentJson["x_target"].is<double>())
        {
            _command = Command::Invalid;
            sendError("Position cible absente ou invalide");
            return false;
        }

        _targetPosition = documentJson["x_target"].as<double>();
        _command = Command::Stabilize;
        return true;
    }

    _command = Command::Invalid;
    sendError("Commande inconnue");

    return false;
}

/**
 * @brief Envoie une erreur à l'autre appareil
 */
void CommJSON::sendError(const char *message)
{
    StaticJsonDocument<128> documentJson;

    documentJson["type"] = "error";
    documentJson["message"] = message;

    serializeJson(documentJson, _serial);
    _serial.println();
}
