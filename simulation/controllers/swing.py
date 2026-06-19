# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np   

class SwingController:
    @staticmethod
    def compute(t: float, state: np.ndarray, t0:float, target: float) -> float:
        """
        Retourne la commande u pendant la phase SWING.

        Ici, c'est volontairement simple :
        on applique une commande constante pour créer une oscillation.
        """
        x, dx, theta, dtheta = state

        u_max = 10.0

        # Exemple très simple :
        # selon ton modèle, il faudra peut-être inverser le signe.
        return u_max