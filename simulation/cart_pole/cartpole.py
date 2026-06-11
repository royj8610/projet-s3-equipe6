
import numpy as np
from scipy.integrate import solve_ivp

def cartpole_solve(t, y):
    pass

def cartpole_simulate(
        init_val:np.ndarray,
        tf:float
):
    """
    DESC: 
        Effectue l'intégration numérique pour le système du cartpole
    PARAM:
        init_val - Les valeurs initiales de l'intégration
        tf - Le temps de fin de l'intégration
    RET:
        sol - La solution du solve_ivp
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

    pass

def main():
    ## Logique de résolution du cartpole, appeler les fonctions des autres fichiers
    pass



if __name__ == "__main__":
    main()