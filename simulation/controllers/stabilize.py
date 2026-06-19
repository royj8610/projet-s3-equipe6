# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np

class StabilizeController:
    @staticmethod
    def compute(t: float, state: np.ndarray, t0:float, target: float) -> float:
        """
        Stabilise le pendule vers theta = 0 tout en gardant x près de target.
        """
        x, dx, theta, dtheta = state

        kp_x = 15.0
        kd_x = 8.0

        kp_theta = 60.0
        kd_theta = 15.0

        # Attention : le signe de theta dépend de ta convention.
        # Si ça déstabilise au lieu de stabiliser, inverse le signe des termes angulaires.
        u = (
            kp_x * (target - x)
            - kd_x * dx
            - kp_theta * theta
            - kd_theta * dtheta
        )

        return u
