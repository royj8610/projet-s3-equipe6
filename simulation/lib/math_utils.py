# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import numpy as np
from typing import TypeAlias

Array: TypeAlias = np.ndarray[tuple[int], np.dtype[np.float32 | np.float64]]

class UnitConversion:
    """
    Facteurs de conversion d'unités.
    """

    CM_TO_M = 1/100

def centered_differentiation(x: Array, f: Array):
    """N.B. La précision est proportionnelle au carré du pas."""
    df = np.zeros_like(f)
    df[0] = (f[1] - f[0]) / (x[1] - x[0])
    df[-1] = (f[-1] - f[-2]) / (x[-1] - x[-2])
    df[1:-1] = (f[2:] - f[:-2]) / (x[2:] - x[:-2])
    return df

def clamp(x: float, low: float, high: float):
    if x < low:
        return low
    if x > high:
        return high
    return x