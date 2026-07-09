# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np
from scipy.linalg import solve_continuous_are


def compute_lqr_gain(A: np.ndarray, B: np.ndarray, Q: np.ndarray, R: np.ndarray) -> np.ndarray:
    """
    Calcule le gain LQR K pour le système linéaire :

        state_dot = A state + B u

    La commande finale sera :

        u = -K @ error_state
    """
    P = solve_continuous_are(A, B, Q, R)
    K = np.linalg.inv(R) @ B.T @ P
    return K