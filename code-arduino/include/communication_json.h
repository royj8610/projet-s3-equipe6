#ifndef COMM_JSON_H
#define COMM_JSON_H
#include <Arduino.h>

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
        String state = "IDLE";
        double targetPosition = 0.0;
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

    // // variables de commande reçues du Raspberry Pi
    // String commandeRecue = "AUCUNE";
    // double positionCible = 0.0;
    // bool demarrageDemande = false;
    // bool arretDemande = false;

    // // variables d'etat du robot
    // // pour l'instant c'est des valeurs de test
    // // elles seront remplacees par les vraies valeurs calculees : IMU, encodeurs, vitesse, position, glissement,
    // double positionRobot = 0.0;
    // double vitesseRobot = 0.0;
    // double anglePendule = 0.0;
    // double accelerationX = 0.0;
    // bool glissementDetecte = false;
    // String etatRobot = "IDLE";
};

// // variables de commande reçues du Raspberry Pi
// // elles peuvent etre utilisees ailleurs dans le programme Arduino
// extern String commandeRecue;
// extern double positionCible;
// extern bool demarrageDemande;
// extern bool arretDemande;

// // initialise la communication serie entre l'Arduino et le Raspberry Pi
// // on utilise le port USB serie avec une vitesse de 115200 bauds
// void initialiserCommunicationJson();

// // lit une commande JSON reçue du Raspberry Pi
// void lireCommandeDuRaspberry();

// // Envoie l'etat actuel du robot au Raspberry Pi sous forme JSON
// void envoyerEtatAuRaspberry();

#endif // COMM_JSON_H