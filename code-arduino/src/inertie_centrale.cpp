#include <Arduino.h>
#include <Wire.h>
#include "inertie_centrale.h"

// Fonction d'initialisation de la centrale inertielle
void imu_init() {
    // Démarre la communication I2C entre l'Arduino et les périphériques I2C
    // Sur Arduino Mega, les broches I2C sont :
    // SDA = pin 20
    // SCL = pin 21
    Wire.begin();

    // Message affiché dans le moniteur série pour confirmer le début de l'initialisation
    Serial.println("Initialisation IMU...");
}

// Fonction qui scanne tous les appareils connectés au bus I2C
void imu_scanI2C() {
    byte error;              // Variable qui reçoit le résultat de la communication I2C
    byte address;            // Adresse I2C testée
    int devicesFound = 0;    // Compteur du nombre d'appareils détectés

    Serial.println("Scan I2C en cours...");

    // Les adresses I2C possibles vont de 1 à 126
    // On teste chaque adresse une par une
    for (address = 1; address < 127; address++) {

        // Début de la communication avec l'adresse testée
        Wire.beginTransmission(address);

        // Fin de la communication
        // Si error == 0, cela veut dire qu'un appareil a répondu
        error = Wire.endTransmission();

        // Si un appareil I2C est détecté à cette adresse
        if (error == 0) {
            Serial.print("Appareil I2C trouve a l'adresse 0x");

            // Ajoute un 0 devant les adresses plus petites que 0x10
            // Exemple : affiche 0x0C au lieu de 0xC
            if (address < 16) {
                Serial.print("0");
            }

            // Affiche l'adresse en format hexadecimal
            Serial.println(address, HEX);

            // Augmente le compteur d'appareils trouvés
            devicesFound++;
        }
    }

    // Si aucun appareil n'a été trouvé
    if (devicesFound == 0) {
        Serial.println("Aucun appareil I2C trouve.");
    } 
    // Si au moins un appareil a été trouvé
    else {
        Serial.println("Scan I2C termine.");
    }
}