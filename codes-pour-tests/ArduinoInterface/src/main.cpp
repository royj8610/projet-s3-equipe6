/* 
 * GRO 302 - Conception d'un robot mobile
 * Code de démarrage
 * Auteurs: Jean-Samuel Lauzon     
 * date: 1 mai 2019
*/

/*------------------------------ Librairies ---------------------------------*/
#include <LibS3GRO.h>
#include <ArduinoJson.h>
#include <libExample.h> // Vos propres librairies
/*------------------------------ Constantes ---------------------------------*/

#define BAUD            115200      // Frequence de transmission serielle
#define UPDATE_PERIODE  100         // Periode (ms) d'envoie d'etat general

/*---------------------------- variables globales ---------------------------*/


volatile bool shouldSend_ = false;  // drapeau prêt à envoyer un message
volatile bool shouldRead_ = false;  // drapeau prêt à lire un message

SoftTimer timer_;            // chronometre d'envoie d'information

int workState_ = 0;
int positionA_ = 0;
int angleA_ = 0;
int positionB_ = 0;
int angleB_ = 0;
int treeNum_ = 0;


/*------------------------- Prototypes de fonctions -------------------------*/

void timerCallback();
void sendMsg(); 
void readMsg();
void serialEvent();
void turnOn();
void turnOff();


/*---------------------------- fonctions "Main" -----------------------------*/

void setup() {
  Serial.begin(BAUD);               // initialisation de la communication serielle
  
  // Chronometre 
  timer_.setDelay(UPDATE_PERIODE);
  timer_.setCallback(timerCallback);
  timer_.enable();

}

/* Boucle principale (infinie)*/
void loop() {

  if(shouldRead_){
    readMsg();
  }
  if(shouldSend_){
    sendMsg();
  }

  if (workState_ == 1){
    positionA_ = random(1000); 
    angleA_ = random(360);
    positionB_ = random(1000); 
    angleB_ = random(360);
    delay(1000);
    }
  else if (workState_ == 0)
  {
    positionA_ = 0; 
    angleA_ = 0;
    positionB_ = 0; 
    angleB_ = 0;
  }
  
  

  // mise a jour des chronometres
  timer_.update();

}

/*---------------------------Definition de fonctions ------------------------*/

void serialEvent(){shouldRead_ = true;}

void timerCallback(){shouldSend_ = true;}


void sendMsg(){
  /* Envoit du message Json sur le port seriel */
  StaticJsonDocument<500> doc;
  
  // Elements du message
  doc["time"] = millis()/1000;
  doc["positionA"] = positionA_;
  doc["angleA"] = angleA_;

  doc["positionB"] = positionB_;
  doc["angleB"] = angleB_;
  doc["treeNum"] = treeNum_;

  doc["state"] = workState_;



  // Serialisation
  serializeJson(doc, Serial);
  
  // Envoit
  Serial.println();
  shouldSend_ = false;
}

void readMsg(){
  // Lecture du message Json
  StaticJsonDocument<500> doc;
  JsonVariant parse_msg;

  // Lecture sur le port Seriel
  DeserializationError error = deserializeJson(doc, Serial);
  shouldRead_ = false;

  // Si erreur dans le message
  if (error) {
    Serial.print("deserialize() failed: ");
    Serial.println(error.c_str());
    return;
  }
  
  // Analyse des éléments du message message
  // Si on doit allumer la lumière
  parse_msg = doc["start"];
  if(!parse_msg.isNull()){
    turnOn();
  }

  // Si on doit faire un echo du message recu
  parse_msg = doc["stop"];
  if(!parse_msg.isNull()){
    turnOff();
  }

  parse_msg = doc["reset"];
  if(!parse_msg.isNull()){
    Serial.println("is reset");
  }

}

// Allumer la lumière
void turnOn(){

  workState_ = 1;
}

// Éteindre la lumière
void turnOff(){
  workState_ = 0;

}
