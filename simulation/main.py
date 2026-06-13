
from cart_pole.cartpole import cartpole_simulate

def main():
    """
    Point d'entré du programme.
    """

    y0 = [0, 0, 0, 0]
    tf = 60
    sol = cartpole_simulate(init_val=y0, tf=tf)



if __name__ == "__main__":
    main()
