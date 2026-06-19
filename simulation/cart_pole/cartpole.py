
import numpy as np
from scipy.integrate import solve_ivp
from simulation.cart_pole.symbolic import cartpole_symbolic
from simulation.models.motor import Motor

def Fm_motor(t, y):
    """
    Fonction pour représenter le  force du moteur 
    Pour tester
    Reste à implémenter le state machine
    """
    motor = Motor()
    x, dx, theta, dtheta = y

    motor.velocity = dx
    if (t < 1.0):  # Pour tester seulement
        U = 0.2
    else :
        U = -0.2
    Tm = motor.voltage_to_torque(U)
    Fm = motor.torque_to_force(Tm)
    return Fm # Force constante -> Tm constant


def cartpole_solve(t, y, A_fn:callable, b_fn:callable):
    """
    Fonction pour l'intégration numérique du systeme.

    Parameters
    ----------
    y : list[float]
        Liste des variables indépendantes [x, theta, dx, dtheta].
    A_fn : callable
        Fonction numérique de la matrice A
    b_fn : callable
        Fonction numérique du vecteur solution b
    moteur : Motor
        Objet représentant le moteur utilisé
    
    Returns
    -------
    list[float]
        Dérivée des variables indépendantes [dx, dtheta, ddx, ddtheta].
    """
    x, theta, dx, dtheta = y

    # Force motrice en X
    Fm = Fm_motor(t, y) # TODO : Remplacer par actual fonction de moteur

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
    init_val : np.ndarray
        Les valeurs initiales de l'intégration [x, theta, dx, dtheta]
    tf : float 
        Le temps de fin de l'intégration en seconde.

    Returns
    -------
    sol : 
        La solution du solve_ivp.
    """
    # Création de fonction avec la dynamique
    A_fn, b_fn = cartpole_symbolic()


    sol = solve_ivp(
            fun=cartpole_solve,
            t_span=(0, tf),
            y0=init_val,
            method="RK45",
            t_eval=np.linspace(0, tf, 1000), # Valeurs arbitraires
            dense_output=True,
            args=(A_fn, b_fn), # arguments supplémentaires
            # max_step=1e-3,  # Increases the number of elements (and compute time) by one order of magnitude. - Vient du template
            atol=1e-9,
            rtol=1e-6,
        )

    return sol
