# -*- coding: utf-8 -*-
#!/usr/bin/env python3
from simulation.cart_pole.param import CartPoleParams

class Motor:
    """
    Représentation numérique d'un moteur physique
    
    Valeur par défaut du moteur 5010 360KV
    """
    def __init__(
            self,
            kg= (60/32),
            R = 0.270,
            ke = 0.0265,
            kt=0.0265,
        ):
        """
        Constructeur de la classe Motor

        Parameters
        ----------
        kg : float
            Le facteur de démultiplication du moteur
        R : float
            La résistance du moteur
        ke : float 
            La constante électrique du moteur
        kt : float
            LA constante de couple du moteur
        """
        self.kg = kg
        self.R = R
        self.ke = ke
        self.kt = kt

        self.velocity = 0.0
        self.omega = self.velocity/CartPoleParams.R_WHEEL

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
        i = (U - self.ke*(self.omega))/self.R
        Tm = self.kt * i
        
        return Tm
    
    def torque_to_force(self, Tm: float) -> float:
        """
        Convertit le couple moteur Tm en force linéaire Fm
        à partir du rapport de réduction et du rayon de roue.

        Parameters
        ----------
        Tm : float
            Le couple moteur (N·m).

        Returns
        -------
        float
            La force linéaire résultante (N).
        """
        Tr = Tm * self.kg
        Fm = Tr / CartPoleParams.R_WHEEL
        return Fm


    
    


