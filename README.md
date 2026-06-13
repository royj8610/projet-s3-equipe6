# Projet S3 robot mobile

## Configuration du projet

Pour travailler sur le projet, vous devrez créer votre propre branche. Cette étape est importante, car vous ne pourrez pas pousser vos changements sur la branche *main*.

### 1. Cloner le projet sur votre ordinateur

Sur votre ordinateur, ouvrir une fenêtre de commande. En utilisant la commande ```cd dossier```, naviguer jusqu'à l'endroit où vous voulez télécharger le projet. Exécuter la commande :

```git clone https://github.com/royj8610/projet-s3-equipe6.git [nom du dossier]```

**Vous devez changer [nom du dossier] par le nom que vous voulez donner au dossier du projet**

Alternativement, utiliser une application comme GitHub Desktop.

### 2. Créer une branche

**VS Code :** Cliquer sur "main" en bas à gauche de la fenêtre. Dans le pop-up, vous pouvez cliquer "Create new branch" et lui donner le nom que vous voulez.

### 3. Créer une pull request

Après avoir fait un *push* et que vos changements sont à jour sur GitHub, vous pouvez créer une pull request pour joindre vos changements à la branche *main*.

Allez sur le site de GitHub, sous l'onglet *pull request*. Cliquer sur "New pull request". Vous devrier voir une branche "base" et une branche "compare". Changez la branche compare pour votre branche.

Après avoir révisé vos changements, cliquez sur "Create pull request". Vous pouvez donner un nom à votre *pull request* et laisser un commentaire pour expliquer les changements apportés.

Une fois la *pull request* créée, elle devra être approuvée par un autre membre avant de pouvoir être fusionnée sur *main*.

**NOTE : Faites uniquement des pull-request pour les features complètes et du code fonctionnel !!**

## Structure du simulateur Python

Le point d'entré du programme est le fichier `main.py`.

### Sous-module
- **cart_pole** : Ensemble des scripts liés au modèle physique
    - `cartpole.py` Devrait être le point d'entré depuis un script externe. Effectue la résolution numérique (Intégration) du système
    - `symbolic.py` Contient la représentation Sympy du système est la conversion numérique des équations.
    - `param.py` Contient 2 classes pour les constantes du système, *CartPoleParams* (Constante du chariot) et *RailParams* (Constante de l'environnement).

- **models** : Ensemble des scripts représentant des models physiques ne dépendant pas d'une situation précise (Comme des moteurs)
    - `motor.py` Classe de représentation d'un moteur

- **lib** : Scripts utilitaires ou n'ayant pas de model physique
    - `math_utils.py` Fonction et classes mathématique (Conversion, dérivée, etc)
    - `print_utils.py` Classe de constant pour l'affichage en terminal
    - `pid.py` Classes de PID 

- **visual** : Scripts pour l'affichage graphique et l'animation
    - `animations.py` Fonction pour les animations
    

### Exemple de Docstring
Cette structure de docstring permet un formattage automatique lors du over de la souris sur la fonction

```Python
    """
    Convertie une tension d'entré en torque moteur.

    Parameters
    ----------
    U : float
        The total distance traveled in meters.
    time : float
        The total time taken in seconds.

    Returns
    -------
    float
        The calculated velocity in meters per second.

    Raises
    ------
    ZeroDivisionError
        If the time provided is zero.
    """
```