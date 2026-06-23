# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np

class MoveToPointController:
    @staticmethod
    def compute(t: float, state: np.ndarray, k, t0: float, target: float) -> float:
        """
        Déplace le véhicule vers la position cible.

        Retourne :
            Fm : la force à appliquer sur le chariot.
        """
        x, dx, theta, dtheta = state
        
        error_state = np.array([
            x - target,
            dx,
            np.sin(theta),
            dtheta
        ])

        u = -(k @ error_state)[0]

        return u
    
