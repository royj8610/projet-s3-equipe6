# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np


class SwingController:
    @staticmethod
    def compute(t: float, state: np.ndarray, t0: float, target: float) -> float:
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
            Tm : couple moteur demandé.
        """
        x, dx, theta, dtheta = state

        # TODO : 

        Tm = 0.1

        return Tm