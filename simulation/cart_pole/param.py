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
L_ROD = 0.25
L_CCM_BQ = 0.048
R_WHEEL = 2.5 * CM_TO_M

# Gravité
G = 9.81

# Pertes
B_FROTT = 0.005
B_X = 0

# Inertie
I_XX_SAPIN = 0.00020
I_YY_SAPIN = 0.00010
I_ZZ_SAPIN = 0.00029