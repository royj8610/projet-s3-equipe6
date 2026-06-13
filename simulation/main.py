
from simulation.cart_pole.cartpole import cartpole_simulate
from simulation.visual.animations import animate_cart_pole

def main():
    """
    Point d'entré du programme.
    """

    y0 = [0, 45*3.14/180, 0, 0] # [x, theta, dx, dtheta]
    tf = 10
    
    sol = cartpole_simulate(init_val=y0, tf=tf)

    animate_cart_pole(sol)


if __name__ == "__main__":
    main()
