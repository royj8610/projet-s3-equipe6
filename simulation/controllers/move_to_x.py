# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np

class MoveToPointController:
    @staticmethod
    def compute(t: float, state: np.ndarray, t0:float, target: float) -> float:
        """
        Déplace le véhicule vers X_TARGET avec un contrôleur PD.
        """
        x, dx, theta, dtheta = state

        kp_x = 25.0
        kd_x = 10.0

        u = kp_x * (target - x) - kd_x * dx

        return u