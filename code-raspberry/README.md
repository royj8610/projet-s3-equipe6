# Communication JSON côté Raspberry Pi
Ce dossier contient le code Python utilisé par le Raspberry Pi pour communiquer avec l’Arduino Mega.
## Rôle du Raspberry Pi
Le Raspberry Pi agit comme maître de communication. Il sert à :
- envoyer des commandes simples à l’Arduino ;
- lire les messages JSON envoyés par l’Arduino ;
- afficher les données reçues ;
- enregistrer les données si nécessaire.
L’Arduino reste responsable des calculs principaux et du contrôle moteur.
## Fichier principal
| Fichier | Rôle |
|---|---|
| `communication_pi.py` | Script Python pour communiquer avec l’Arduino |
## Dépendance nécessaire
Le script utilise la librairie Python `pyserial`.
Installation :
```bash
pip install pyserial