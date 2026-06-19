# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np

class StabilizeController:
    @staticmethod
    def compute(t: float, state: np.ndarray, t0: float, target: float) -> float:
        """
        Stabilise les 4 états du système.

        Objectif :
            x      -> target
            dx     -> 0
            theta  -> 0
            dtheta -> 0

        Convention :
            theta = 0 correspond au pendule vertical vers le bas.

        Retourne :
            Tm : couple moteur demandé.
        """
        x, dx, theta, dtheta = state

        # TODO :

        Tm = 1.0

        return float(Tm)