# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np
from simulation.cart_pole.cartpole import cartpole_symbolic
from simulation.lib.lqr import compute_lqr_gain

class MoveToPointController:
    @staticmethod
    def compute(t: float, state: np.ndarray, t0: float, target: float) -> float:
        """
        Déplace le véhicule vers la position cible.

        Retourne :
            Tm : couple moteur demandé.
        """
        x, dx, theta, dtheta = state

        A, b = cartpole_symbolic()
        Q = np.array() #pas trop sûr du forme, à complèter
        R = np.array()

        K = compute_lqr_gain(A, b, Q, R)
        
        current_state = np.array[x, dx, theta, dtheta]
        target_state = np.array[target, 0.0, 0.0, 0.0]
        error_state = target_state - current_state


        u = -MoveToPointController.K @ error_state
        Tm = float(u.item())

        return Tm #pas encore testé
    
