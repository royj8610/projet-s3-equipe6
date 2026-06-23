
import numpy as np
from collections.abc import Callable
from scipy.integrate import solve_ivp
from simulation.cart_pole.symbolic import cartpole_symbolic
from simulation.models.motor import Motor
from simulation.state_machine.machine_controller import StateMachineController
from simulation.cart_pole.param import SimulationParams

# def Fm_input(t, y):
#     """
#     Fonction TEMPORAIRE pour simuler le moteur
#     """
#     return 0.0 # Force constante -> Tm constant

# def Fm_motor(t, y):
#     """
#     Fonction pour représenter le  force du moteur 
#     Pour tester
#     Reste à implémenter le state machine
#     """
#     motor = Motor()
#     x, dx, theta, dtheta = y

#     motor.velocity = dx
#     if (t < 1.0):  # Pour tester seulement
#         U = 0.2
#     else :
#         U = -0.2
#     Tm = motor.voltage_to_torque(U)
#     Fm = motor.torque_to_force(Tm)
#     return Fm # Force constante -> Tm constant


def cartpole_solve(t, state, A_fn:Callable, b_fn:Callable, controller:StateMachineController, moteur : Motor):
    """
    Fonction pour l'intégration numérique du systeme.

    Parameters
    ----------
    y : list[float]
        Liste des variables indépendantes [x, dx, theta, dtheta].
    A_fn : callable
        Fonction numérique de la matrice A
    b_fn : callable
        Fonction numérique du vecteur solution b
    controller : Callable
        Fonction de control du système
    moteur : Motor
        L'objet représentant le moteur physique
    
    Returns
    -------
    list[float]
        Dérivée des variables indépendantes [dx, ddx, dtheta, ddtheta].
    """
    x, dx, theta, dtheta = state

    # Force motrice en X
    Tm = controller.compute(t, state)
    Fm = moteur.torque_to_force(Tm) # TODO : La dynamique est par rapport à Fm présentement, à changer pour Tm

    A = np.array(A_fn(theta), dtype=float)
    b = np.array(b_fn(theta, dx, dtheta, Fm), dtype=float).reshape(2)

    ddx, ddtheta = np.linalg.solve(A, b)

    return [
        dx,
        ddx,
        dtheta,
        ddtheta,
    ]


def cartpole_simulate(
        init_val:np.ndarray,
        tf:float,
        moteur: Motor
    ):
    """
    Effectue l'intégration numérique pour le système du cartpole.

    Parameters
    ----------
    init_val : np.ndarray
        Les valeurs initiales de l'intégration [x, dx, theta, dtheta]
    tf : float 
        Le temps de fin de l'intégration en seconde.

    Returns
    -------
    sol : 
        La solution du solve_ivp.
    """
    # Création de fonction avec la dynamique
    A_fn, b_fn, A_Jac, B_Jac = cartpole_symbolic()

    # État initiale de la machine à états
    t0 = 0.0
    state0 = init_val
    controller = StateMachineController(
        SimulationParams.SWING_ANGLE, 
        SimulationParams.GOAL_X,
        0.05,
        A_Jac,
        B_Jac
    )

    # Résolution
    sol = solve_ivp(
            fun=cartpole_solve,
            t_span=(t0, tf),
            y0=state0,
            method="RK45",
            t_eval=np.linspace(t0, tf, 1000), # Valeurs arbitraires
            dense_output=True,
            args=(A_fn, b_fn, controller, moteur), # arguments supplémentaires
            # max_step=1e-3,
            atol=1e-7,
            rtol=1e-5,
        )

    if not sol.success:
        print("solve_ivp a échoué.")
        print("Message :", sol.message)

    return sol
