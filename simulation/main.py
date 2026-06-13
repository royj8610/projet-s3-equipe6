
from simulation.cart_pole.cartpole import cartpole_simulate
from simulation.visual.animations import animate_cart_pole

def main():
    """
    Point d'entré du programme.
    """

    y0 = [0, 0, 0, 0]
    tf = 2
    sol = cartpole_simulate(init_val=y0, tf=tf)

    animate_cart_pole(sol)


if __name__ == "__main__":
    main()
