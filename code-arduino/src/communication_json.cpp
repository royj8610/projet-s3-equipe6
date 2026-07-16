#include <Arduino.h>
#include <ArduinoJson.h>
#include "communication_json.h"


// variables de commande reçues du Raspberry Pi
String commandeRecue = "AUCUNE";
double positionCible = 0.0;
bool demarrageDemande = false;
bool arretDemande = false;


// variables d'etat du robot
// pour l'instant c'est des valeurs de test
// elles seront remplacees par les vraies valeurs calculees : IMU, encodeurs, vitesse, position, glissement,
double positionRobot = 0.0;
double vitesseRobot = 0.0;
double anglePendule = 0.0;
double accelerationX = 0.0;
bool glissementDetecte = false;
String etatRobot = "IDLE";


void initialiserCommunicationJson() {

   Serial.begin(115200);

   // sans ce delai, le Raspberry Pi peut parfois manquer le premier message
   delay(1000);
   Serial.println("{\"status\":\"Arduino ready\"}");
}


void lireCommandeDuRaspberry() {

   if (Serial.available() > 0) {
       String messageRecu = Serial.readStringUntil('\n');

       StaticJsonDocument<256> documentJson;

       // on fait la conversion du texte reçu en objet JSON
       DeserializationError erreur = deserializeJson(documentJson, messageRecu);
       if (erreur) {
           Serial.println("{\"error\":\"JSON invalide\"}");
           return;
       }
       const char* commande = documentJson["cmd"];
       if (commande == nullptr) {
           Serial.println("{\"error\":\"Commande absente\"}");
           return;
       }
       commandeRecue = String(commande);

       // commande START : le Raspberry demande a l'Arduino de demarrer
       if (commandeRecue == "START") {
           demarrageDemande = true;
           arretDemande = false;
           etatRobot = "MOVE";
       }

       // commande STOP : le Raspberry demande a l'Arduino d'arreter
       else if (commandeRecue == "STOP") {
           arretDemande = true;
           demarrageDemande = false;
           etatRobot = "IDLE";
       }
       // commande SET_TARGET : le Raspberry envoie une position cible
       else if (commandeRecue == "SET_TARGET") {
           positionCible = documentJson["x_target"];
           etatRobot = "MOVE_TO_TARGET";
       }
   }
}

void envoyerEtatAuRaspberry() {
   StaticJsonDocument<256> documentJson;

   // L'Arduino fait les calculs et envoie seulement les resultats utiles au Pi
   documentJson["time"] = millis();
   documentJson["x"] = positionRobot;
   documentJson["v"] = vitesseRobot;
   documentJson["angle"] = anglePendule;
   documentJson["ax"] = accelerationX;
   documentJson["state"] = etatRobot;
   documentJson["slip"] = glissementDetecte;
   documentJson["target"] = positionCible;

   // serializeJson ecrit directement le JSON sur le port serie
   serializeJson(documentJson, Serial);

   // ee retour a la ligne est important :
   // il permet au Raspberry Pi de lire les messages ligne par ligne
   Serial.println();
}