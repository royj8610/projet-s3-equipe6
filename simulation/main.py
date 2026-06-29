
import numpy as np
from simulation.cart_pole.cartpole import cartpole_simulate
from simulation.visual.animations import animate_cart_pole
from simulation.models.motor import Motor

from simulation.cart_pole.param import CartPoleParams, RailParams
import matplotlib.pyplot as plt

def main():
    """
    Point d'entré du programme.
    """

    y0 = np.array([0, 0, 0, 0]) # [x, dx, theta, dtheta]
    tf = 8
    
    moteurA = Motor(kg=60/32, no_load_speed=4345, Tstall=121)
    sol = cartpole_simulate(init_val=y0, tf=tf, moteur=moteurA)

    L = CartPoleParams.L_ROD
    WALL_X = RailParams.OBSTACLE_X

    t_eval = sol.t
    x = np.array(sol.y[0])
    dx = np.array(sol.y[1])
    theta = np.array(sol.y[2])
    dtheta = sol.y[3]

    p_x = []
    p_y = []

    distance = 10
    closest_index = 0

    fig = plt.figure(1)
    ax = fig.add_subplot(111)

    for i in range(len(x)):
        if x[i] < 1.2:
            p_x.append(x[i] + L*np.sin(theta[i]))
            p_y.append(L - L*np.cos(theta[i]))
        else:
            break

        if abs(WALL_X - p_x[i]) < distance:
            distance = abs(WALL_X - p_x[i])
            closest_index = i

    ax.plot([WALL_X, WALL_X], [0, 0.15], 'red', ls='--', label='Wall position')
    ax.plot([-0.3, 1.5], [0.01, 0.01], 'green', ls='--', label='Wall height')
    ax.plot([p_x[closest_index]], [p_y[closest_index]], 'red', marker='o')

    ax.annotate(
        text=f'({round(p_x[closest_index], 4)}, {round(p_y[closest_index], 4)})',      # The coordinate text label
        xy=(p_x[closest_index], p_y[closest_index]),               # Point to target
        xytext=(5, 5),             # Shift text 5 points right and 5 points up
        textcoords="offset points" # Relative pixel offsetting
    )

    ax.plot(p_x, p_y)
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_xlim(-0.3, 1.5)
    ax.set_ylim(0, 0.15)
    ax.set_title("Trajectoire du pendule")

    ax.legend()

    plt.show()

    animate_cart_pole(sol, False)


if __name__ == "__main__":
    main()
