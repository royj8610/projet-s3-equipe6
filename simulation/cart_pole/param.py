## Insérer les différents paramètres du cartpole, environnement, etc. dans ce fichier

# -*- coding: utf-8 -*-
#!/usr/bin/env python3

"""
L'ensemble des constantes du système et de l'environnement
"""
import numpy as np
from simulation.lib.math_utils import UnitConversion

class SimulationParams:
    """
    Paramètre pour la simultation
    """
    START_X = 0
    GOAL_X = 1.2 #m
    SWING_ANGLE = -np.deg2rad(45)

class CartPoleParams:
    """
    Constantes physiques du système chariot-pendule.
    """

    # Masses
    M_CHARIOT = 0.8
    M_SAPIN = 0.140
    M_ROD = 0.05

    # Dimensions
    L_ROD = 30 * UnitConversion.CM_TO_M
    L_CCM_BQ = 0.048
    R_WHEEL = 2.5 * UnitConversion.CM_TO_M

    # Dimensions visuelles du véhicule
    CART_LENGTH = 0.16
    CART_HEIGHT = 0.08

    # Gravité
    G = 9.81

    # Pertes
    B_FROTT = 0.005
    B_X = 0.005

    # Inertie - Non utilisé pour le moment
    I_XX_SAPIN = 0.00020
    I_YY_SAPIN = 0.00010
    I_ZZ_SAPIN = 0.00029


class RailParams:
    """
    Constantes de l'environnement (rail, obstacle, zone de dépôt).
    Surtout utilisé pour l'animation
    """

    RAIL_LENGTH = 1.7

    # # Dimensions visuelles du véhicule
    # CART_LENGTH = 0.16
    # CART_HEIGHT = 0.08

    # Payload / sapin
    PAYLOAD_RADIUS = 4 * UnitConversion.CM_TO_M

    # Position du pivot par rapport au rail
    # Le pivot est sous le chariot.
    PIVOT_Y = -CartPoleParams.CART_HEIGHT / 2

    # Point le plus bas possible du payload lorsque theta = 0
    PAYLOAD_LOWEST_Y = PIVOT_Y - CartPoleParams.L_ROD - PAYLOAD_RADIUS

    # Sol placé légèrement sous le point le plus bas du payload
    GROUND_CLEARANCE = 0.20
    GROUND_Y = PAYLOAD_LOWEST_Y - GROUND_CLEARANCE    # TODO : Trouver la valeur réelle

    # Hauteur visuelle du rail par rapport au sol
    RAIL_HEIGHT = -GROUND_Y

    # Positions importantes
    DROPZONE_X = 120 * UnitConversion.CM_TO_M   # 1.20 m
    OBSTACLE_X = 60 * UnitConversion.CM_TO_M    # 0.60 m

    # Drop zone
    DROPZONE_WIDTH = 0.20
    DROPZONE_HEIGHT = 0.10

    # Obstacle
    # Le sommet de l'obstacle est 1 cm au-dessus du point le plus bas du pendule.
    OBSTACLE_CLEARANCE = 1 * UnitConversion.CM_TO_M
    OBSTACLE_TOP_Y = PAYLOAD_LOWEST_Y + OBSTACLE_CLEARANCE
    OBSTACLE_HEIGHT = OBSTACLE_TOP_Y - GROUND_Y
    OBSTACLE_WIDTH = 0.02