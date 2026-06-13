
import numpy as np
from scipy.integrate import solve_ivp
from simulation.cart_pole.symbolic import cartpole_symbolic

def Fm_input(t, y):
    """
    Fonction TEMPORAIRE pour simuler le moteur
    """
    return 0.0 # Force constante -> Tm constant


def cartpole_solve(t, y, A_fn:callable, b_fn:callable):
    """
    Fonction pour l'intégration numérique du systeme.

    Parameters
    ----------
    y : list[float]
        Liste des variables indépendantes [x, theta, dx, dtheta].

    Returns
    -------
    list[float]
        Dérivée des variables indépendantes [dx, dtheta, ddx, ddtheta].
    """
    x, theta, dx, dtheta = y

    # Force motrice en X
    Fm = Fm_input(t, y)

    A = np.array(A_fn(theta), dtype=float)
    b = np.array(b_fn(theta, dx, dtheta, Fm), dtype=float).reshape(2)

    ddx, ddtheta = np.linalg.solve(A, b)

    return [
        dx,
        dtheta,
        ddx,
        ddtheta,
    ]


def cartpole_simulate(
        init_val:np.ndarray,
        tf:float
    ):
    """
    Effectue l'intégration numérique pour le système du cartpole.

    Parameters
    ----------
    init_val :
        Les valeurs initiales de l'intégration [x, theta, dx, dtheta]
    tf : float 
        Le temps de fin de l'intégration en seconde.

    Returns
    -------
    sol : 
        La solution du solve_ivp.
    """
    A_fn, b_fn = cartpole_symbolic()

    sol = solve_ivp(
            fun=cartpole_solve,
            t_span=(0, tf),
            y0=init_val,
            method="RK45",
            t_eval=np.linspace(0, tf, 1000), # Valeurs arbitraires
            dense_output=True,  # Recommended for animation
            args=(A_fn, b_fn),
            # max_step=1e-3,  # Increases the number of elements (and compute time) by one order of magnitude.
            atol=1e-9,
            rtol=1e-6,
        )

    return sol
