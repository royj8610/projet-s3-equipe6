#include <Arduino.h>
#include <ArduinoJson.h>
#include "communication_json.h"

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

    // StaticJsonDocument<256> documentJson;

    // // on fait la conversion du texte reçu en objet JSON
    // DeserializationError erreur = deserializeJson(documentJson, messageRecu);
    // if (erreur)
    // {
    //     _serial.println("{\"error\":\"JSON invalide\"}");
    //     return false;
    // }
    // const char *commande = documentJson["cmd"];
    // if (commande == nullptr)
    // {
    //     _serial.println("{\"error\":\"Commande absente\"}");
    //     return false;
    // }
    // commandeRecue = String(commande);

    // // commande START : le Raspberry demande a l'Arduino de demarrer
    // if (commandeRecue == "START")
    // {
    //     demarrageDemande = true;
    //     arretDemande = false;
    //     etatRobot = "MOVE";
    // }

    // // commande STOP : le Raspberry demande a l'Arduino d'arreter
    // else if (commandeRecue == "STOP")
    // {
    //     arretDemande = true;
    //     demarrageDemande = false;
    //     etatRobot = "IDLE";
    // }
    // // commande SET_TARGET : le Raspberry envoie une position cible
    // else if (commandeRecue == "SET_TARGET")
    // {
    //     positionCible = documentJson["x_target"];
    //     etatRobot = "MOVE_TO_TARGET";
    // }
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
    documentJson["x"] = state.position;
    documentJson["v"] = state.speed;
    documentJson["angle"] = state.angle;
    documentJson["angular_speed"] = state.angularSpeed;
    documentJson["ax"] = state.accelerationX;
    documentJson["state"] = state.state;
    documentJson["slip"] = state.slipDetected;
    documentJson["target"] = state.targetPosition;

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
    if (strcmp(commandText, "START") == 0)
    {
        _command = Command::Start;
        return true;
    }

    if (strcmp(commandText, "STOP") == 0)
    {
        _command = Command::Stop;
        return true;
    }

    if (strcmp(commandText, "SET_TARGET") == 0)
    {
        if (!documentJson["x_target"].is<double>())
        {
            _command = Command::Invalid;
            sendError("Position cible absente ou invalide");
            return false;
        }

        _targetPosition = documentJson["x_target"].as<double>();
        _command = Command::SetTarget;

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

/**
 * @brief Envoie l'état du robot à l'autre appareil
 */
// bool CommJSON::setState(
//     double position,
//     double speed,
//     double angle,
//     double angSpeed,
//     String robotState)
// {
//     StaticJsonDocument<256> documentJson;

//     // L'Arduino fait les calculs et envoie seulement les resultats utiles au Pi
//     documentJson["time"] = millis();
//     documentJson["x"] = position;
//     documentJson["v"] = speed;
//     documentJson["angle"] = angle;
//     documentJson["ax"] = accelerationX;
//     documentJson["state"] = robotState;
//     documentJson["slip"] = glissementDetecte;
//     documentJson["target"] = positionCible;

//     // serializeJson ecrit directement le JSON sur le port serie
//     serializeJson(documentJson, _serial);

//     // ee retour a la ligne est important :
//     // il permet au Raspberry Pi de lire les messages ligne par ligne
//     _serial.println();

//     return true; // Is comm success?
// }

// void initialiserCommunicationJson() {

//    Serial.begin(115200);

//    // sans ce delai, le Raspberry Pi peut parfois manquer le premier message
//    delay(1000);
//    Serial.println("{\"status\":\"Arduino ready\"}");
// }

// void lireCommandeDuRaspberry() {

//    if (Serial.available() > 0) {
//        String messageRecu = Serial.readStringUntil('\n');

//        StaticJsonDocument<256> documentJson;

//        // on fait la conversion du texte reçu en objet JSON
//        DeserializationError erreur = deserializeJson(documentJson, messageRecu);
//        if (erreur) {
//            Serial.println("{\"error\":\"JSON invalide\"}");
//            return;
//        }
//        const char* commande = documentJson["cmd"];
//        if (commande == nullptr) {
//            Serial.println("{\"error\":\"Commande absente\"}");
//            return;
//        }
//        commandeRecue = String(commande);

//        // commande START : le Raspberry demande a l'Arduino de demarrer
//        if (commandeRecue == "START") {
//            demarrageDemande = true;
//            arretDemande = false;
//            etatRobot = "MOVE";
//        }

//        // commande STOP : le Raspberry demande a l'Arduino d'arreter
//        else if (commandeRecue == "STOP") {
//            arretDemande = true;
//            demarrageDemande = false;
//            etatRobot = "IDLE";
//        }
//        // commande SET_TARGET : le Raspberry envoie une position cible
//        else if (commandeRecue == "SET_TARGET") {
//            positionCible = documentJson["x_target"];
//            etatRobot = "MOVE_TO_TARGET";
//        }
//    }
// }

// void envoyerEtatAuRaspberry() {
//    StaticJsonDocument<256> documentJson;

//    // L'Arduino fait les calculs et envoie seulement les resultats utiles au Pi
//    documentJson["time"] = millis();
//    documentJson["x"] = positionRobot;
//    documentJson["v"] = vitesseRobot;
//    documentJson["angle"] = anglePendule;
//    documentJson["ax"] = accelerationX;
//    documentJson["state"] = etatRobot;
//    documentJson["slip"] = glissementDetecte;
//    documentJson["target"] = positionCible;

//    // serializeJson ecrit directement le JSON sur le port serie
//    serializeJson(documentJson, Serial);

//    // ee retour a la ligne est important :
//    // il permet au Raspberry Pi de lire les messages ligne par ligne
//    Serial.println();
// }