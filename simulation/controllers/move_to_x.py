# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np
from simulation.cart_pole.cartpole import cartpole_symbolic
from simulation.lib.lqr import compute_lqr_gain

class MoveToPointController:
    @staticmethod
    def compute(t: float, state: np.ndarray, k, t0: float, target: float) -> float:
        """
        Déplace le véhicule vers la position cible.

        Retourne :
            Tm : couple moteur demandé.
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
    
