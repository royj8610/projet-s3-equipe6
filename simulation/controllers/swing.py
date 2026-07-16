# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np

from simulation.cart_pole.param import CartPoleParams, RailParams

class SwingController:
    @staticmethod
    def compute(t: float, state: np.ndarray, t0: float, target_y: float) -> float:
        """
        Amène le pendule vers l'angle cible de swing.

        Convention :
            theta = 0      : pendule vertical vers le bas
            theta < 0      : pendule vers la gauche
            theta > 0      : pendule vers la droite

        Objectif :
            Atteindre environ target = -45 deg avec une vitesse angulaire
            négative ou nulle, donc encore vers la gauche ou à l'apogée.

        Retourne :
            Fm : la force à appliquer sur le chariot.
        """
        x, dx, theta, dtheta = state

        # k = 10 # Factor

        # M = CartPoleParams.M_SAPIN
        # G = CartPoleParams.G
        # L = CartPoleParams.L_ROD

        # E = 0.5*M*(L**2)*(dtheta**2) + M*G*L*(1 - np.cos(theta))
        # E_TARGET = M*G*target_y

        # u = k*((E - E_TARGET)*dtheta*np.cos(theta) + 0.25 - x)

        # return u

        return 10.0