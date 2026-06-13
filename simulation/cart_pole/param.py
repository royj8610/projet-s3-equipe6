## Insérer les différents paramètres du cartpole, environnement, etc. dans ce fichier

# -*- coding: utf-8 -*-
#!/usr/bin/env python3

"""
L'ensemble des constantes du système et de l'environnement
"""
# Facteurs de Convertion
CM_TO_M = 1/100

# Masses
M_CHARIOT = 0.8
M_SAPIN = 0.140
M_ROD = 0.05

# Dimensions
L_ROD = 25 * CM_TO_M
L_CCM_BQ = 0.048
R_WHEEL = 2.5 * CM_TO_M

# Dimensions visuelles du véhicule
CART_LENGTH = 0.16
CART_HEIGHT = 0.08

# Gravité
G = 9.81

# Pertes
B_FROTT = 0.005
B_X = 0

# Inertie
I_XX_SAPIN = 0.00020
I_YY_SAPIN = 0.00010
I_ZZ_SAPIN = 0.00029

# Environnement
RAIL_LENGTH = 1.7
RAIL_HEIGHT = 0.05

# Positions importantes
DROPZONE_X = 120 * CM_TO_M
OBSTACLE_X = 60 * CM_TO_M

# Drop zone
DROPZONE_WIDTH = 0.20
DROPZONE_HEIGHT = 0.10

# Payload / sapin
PAYLOAD_RADIUS = 4 * CM_TO_M

# Obstacle
# L'obstacle monte à 1 cm au-dessus du point le plus bas du pendule, payload inclus.
OBSTACLE_CLEARANCE = 1 * CM_TO_M
OBSTACLE_HEIGHT = L_ROD + PAYLOAD_RADIUS + OBSTACLE_CLEARANCE
