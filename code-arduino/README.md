# Communication JSON côté Arduino
Ce dossier contient le code exécuté par l’Arduino Mega 2560.
## Rôle de l’Arduino
Dans le projet, l’Arduino est responsable des tâches temps réel :
- lire les capteurs ;
- lire l’IMU ;
- lire les encodeurs ;
- calculer la position, la vitesse et l’accélération ;
- détecter un éventuel glissement ;
- contrôler le moteur ;
- envoyer les données calculées au Raspberry Pi.
Le Raspberry Pi ne fait pas les calculs principaux. Il envoie seulement des commandes simples et reçoit les données calculées par l’Arduino.
## Fichiers importants
| Fichier | Rôle |
|---|---|
| `src/main.cpp` | Programme principal de l’Arduino |
| `include/communication_json.h` | Déclaration des fonctions de communication JSON |
| `src/communication_json.cpp` | Lecture des commandes JSON et envoi des données JSON |
| `include/inertie_centrale.h` | Déclaration des fonctions liées à l’IMU |
| `src/inertie_centrale.cpp` | Lecture des données de la centrale inertielle |
## Communication JSON
La communication se fait par USB série à 115200 bauds.
### Raspberry Pi vers Arduino
Exemple de commande pour démarrer :
```json
{"cmd":"START"}