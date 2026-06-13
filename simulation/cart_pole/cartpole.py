
import numpy as np
from scipy.integrate import solve_ivp

def cartpole_solve(t, y):
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
    pass # TODO


def cartpole_simulate(
        init_val:np.ndarray,
        tf:float
    ):
    """
    Effectue l'intégration numérique pour le système du cartpole.

    Parameters
    ----------
    init_val :
        Les valeurs initiales de l'intégration
    tf : float 
        Le temps de fin de l'intégration.

    Returns
    -------
    sol : 
        La solution du solve_ivp.
    """

    sol = solve_ivp(
            fun=cartpole_solve,
            t_span=(0, tf),
            y0=init_val,
            method="RK45",
            #t_eval=...,
            dense_output=True,  # Recommended for animation
            #args=...,
            # max_step=1e-3,  # Increases the number of elements (and compute time) by one order of magnitude.
            atol=1e-9,
            rtol=1e-6,
        )

    return sol





# Je pense pas que ce soit utile - Liam

# def main():
#     ## Logique de résolution du cartpole, appeler les fonctions des autres fichiers
#     pass



# if __name__ == "__main__":
#     main()