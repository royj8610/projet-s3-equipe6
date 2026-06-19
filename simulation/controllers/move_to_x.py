# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np

class MoveToPointController:
    @staticmethod
    def compute(t: float, state: np.ndarray, t0: float, target: float) -> float:
        """
        Déplace le véhicule vers la position cible.

        Retourne :
            Tm : couple moteur demandé.
        """
        x, dx, theta, dtheta = state

        # TODO : 
        Tm = 1.0

        return Tm