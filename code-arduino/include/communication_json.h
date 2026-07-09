#ifndef COMMUNICATION_JSON_H
#define COMMUNICATION_JSON_H
#include <Arduino.h>


// variables de commande reçues du Raspberry Pi
// elles peuvent etre utilisees ailleurs dans le programme Arduino
extern String commandeRecue;
extern double positionCible;
extern bool demarrageDemande;
extern bool arretDemande;

// initialise la communication serie entre l'Arduino et le Raspberry Pi
// on utilise le port USB serie avec une vitesse de 115200 bauds
void initialiserCommunicationJson();


// lit une commande JSON reçue du Raspberry Pi
void lireCommandeDuRaspberry();


// Envoie l'etat actuel du robot au Raspberry Pi sous forme JSON
void envoyerEtatAuRaspberry();




#endif