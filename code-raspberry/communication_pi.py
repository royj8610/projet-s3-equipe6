import serial
import json
import time

# sur Windows/ VScode: COM3, COM4, ...
# sur Raspberry Pi : "/dev/ttyACM0" or "/dev/ttyUSB0"
PORT_ARDUINO = "COM3"
VITESSE_SERIE = 115200

def ouvrir_communication_arduino():
   """
   Ouvre la communication serie avec l'Arduino.
   Le Raspberry Pi est le master :
   il peut envoyer des commandes : START, STOP, SET_TARGET
   L'Arduino repond en envoyant son etat en JSON
   """
   arduino = serial.Serial(PORT_ARDUINO, VITESSE_SERIE, timeout=1)

   # delai pour laisser l'Arduino redemarrer apres l'ouverture du port serie
   time.sleep(2)
   return arduino

def envoyer_commande_simple(arduino, commande):
   """
   Envoie une commande simple a l'Arduino
   """
   message = {
       "cmd": commande
   }
   arduino.write((json.dumps(message) + "\n").encode())

def envoyer_position_cible(arduino, position_cible):
   """
   Envoie une position cible a l'Arduino.
   L'Arduino reste responsable des calculs et du controle moteur
   """
   message = {
       "cmd": "SET_TARGET",
       "x_target": position_cible
   }
   arduino.write((json.dumps(message) + "\n").encode())

def lire_message_arduino(arduino):
   """
   Lit une ligne envoyee par l'Arduino et fait la conversion en JSON
   """
   ligne = arduino.readline().decode(errors="ignore").strip()
   if ligne == "":
       return None
   try:
       return json.loads(ligne)
   except json.JSONDecodeError:
       print("Message JSON invalide :", ligne)
       return None

def afficher_etat_robot(donnees):
   """
   Affiche les donnees reçues
   Si le message ne contient pas une donnee, .get() retourne None au lieu de planter
   """
   if donnees is None:
       return
   print("Temps :", donnees.get("time"))
   print("Position x :", donnees.get("x"))
   print("Vitesse :", donnees.get("v"))
   print("Angle pendule :", donnees.get("angle"))
   print("Accel X :", donnees.get("ax"))
   print("État :", donnees.get("state"))
   print("Glissement :", donnees.get("slip"))
   print("Cible :", donnees.get("target"))
   print("------------------------")

def main():
   arduino = ouvrir_communication_arduino()
   print("Communication ouverte avec Arduino")
   # Test : le Raspberry demande à l'Arduino de démarrer.
   envoyer_commande_simple(arduino, "START")
   while True:
       donnees = lire_message_arduino(arduino)
       afficher_etat_robot(donnees)

if __name__ == "__main__":
   main()