
import numpy as np
from simulation.cart_pole.cartpole import cartpole_simulate
from simulation.visual.animations import animate_cart_pole
from simulation.models.motor import Motor

def main():
    """
    Point d'entré du programme.
    """

    y0 = np.array([0, 0, 0, 0]) # [x, dx, theta, dtheta]
    tf = 20
    
    moteurA = Motor(kg=60/32)
    sol = cartpole_simulate(init_val=y0, tf=tf, moteur=moteurA)

    animate_cart_pole(sol, False)


if __name__ == "__main__":
    main()
