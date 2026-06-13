# -*- coding: utf-8 -*-
#!/usr/bin/env python3

class Motor:
    """
    Représentation numérique d'un moteur physique
    """
    def __init__(
            self,
            kg=1,
        ) -> None:
        """
        Constructeur de la classe Motor

        Parameters
        ----------
        kg : float
            Le facteur de démultiplication du moteur
        """
        self.kg = kg
        

    def voltage_to_torque(self, U: float) -> float:
        """
        Convertie une tension d'entré en torque moteur.

        Parameters
        ----------
        U : float
            La tension de commande du moteur.

        Returns
        -------
        float
            Le torque calculé au niveau du moteur.
        """
        return 0.0
    
    
    def torque_to_speed():
        pass

    
    def voltage_to_angular_speed(self, U: float) -> float:
        """
        Convertie une tension d'entré en la vitesse angulaire du moteur.

        Parameters
        ----------
        U : float
            La tension de commande du moteur.

        Returns
        -------
        float
            La vitesse angulaire calculé au niveau du moteur.
        """
        return 0.0
    

