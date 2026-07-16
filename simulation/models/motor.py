# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np

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
            ke = 0.0,
            kt=0.0,
            no_load_speed=200, # En RPM
            Tstall=210 # En kg.mm
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
        self.Tstall = Tstall * 0.00980665
        self.no_load_speed = no_load_speed * 0.104719755

        self.velocity = 0.0
        self.omega = self.velocity/CartPoleParams.R_WHEEL

    def voltage_to_torque(self, U: float) -> float:
        """
        Convertie une tension d'entrée en torque moteur.

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

    def force_to_torque(self, Fm: float) -> float:
        """
        Convertit la force linéaire Fm en couple moteur Tm
        à partir du rapport de réduction et du rayon de roue.

        Parameters
        ----------
        Fm : float
            La force linéaire (N).

        Returns
        -------
        float
            Le couple du moteur (N.m).
        """
        Tr = Fm * CartPoleParams.R_WHEEL
        Tm = Tr / self.kg

        return Tm
    
    def limit_force(self, Fm_target: float, dx: float):
        omega_r = dx / CartPoleParams.R_WHEEL
        omega_m = np.clip(omega_r * self.kg, -self.no_load_speed, self.no_load_speed)

        Tmax = self.Tstall * (self.no_load_speed - omega_m) / self.no_load_speed

        Fmax = self.torque_to_force(Tmax)

        return np.clip(Fm_target, -Fmax, Fmax)
