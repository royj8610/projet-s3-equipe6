
import numpy as np
from scipy.linalg import solve_continuous_are

from simulation.cart_pole.param import RailParams, CartPoleParams

from simulation.controllers.swing import SwingController
from simulation.controllers.move_to_x import MoveToPointController
from simulation.controllers.stabilize import StabilizeController
from simulation.state_machine.modes import Modes


class StateMachineController:
    """
    Contrôleur global avec mémoire.

    Il choisit automatiquement le mode selon l'état courant :

        SWING -> MOVE_TO_X -> STABILIZE
    """

    def __init__(
        self,
        height_target: float,
        x_target: float,
        tol:float,
        A_Jac,
        B_Jac
    ):
        self.mode = Modes.SWING

        self.height_target = height_target
        self.x_target = x_target

        self.tol = tol

        self.t_mode_start = 0.0

        # Historique optionnel, utile pour déboguer ou tracer les transitions.
        self.mode_history: list[tuple[float, Modes]] = [(0.0, self.mode)]

        # Calcul K stabilisation
        R = np.array([[1]])

        Q_STAB = np.diag([200, 1, 50, 10])
        P_STAB = solve_continuous_are(A_Jac, B_Jac, Q_STAB, R)
        self.k_stab = np.linalg.inv(R) @ B_Jac.T @ P_STAB

        Q_GOTO = np.diag([100, 1, 0, 0])
        P_GOTO = solve_continuous_are(A_Jac, B_Jac, Q_GOTO, R)
        self.k_goto = np.linalg.inv(R) @ B_Jac.T @ P_GOTO


    def update_mode(self, t: float, state: np.ndarray) -> None:
        """
        Met à jour le mode selon l'état courant.
        """

        x, dx, theta, dtheta = state

        L = CartPoleParams.L_ROD

        if self.mode == Modes.SWING:
            # Si theta_swing_target = -45 deg,
            # on passe au prochain mode quand theta <= -45 deg.
            if L - L*np.cos(theta) > self.height_target and theta < 0 and dtheta <= 0:
                self.set_mode(Modes.PASS_WALL, t)
                self.set_x_target(RailParams.DROPZONE_X)
            elif x + CartPoleParams.L_ROD*np.sin(theta) > RailParams.OBSTACLE_X:
                self.set_mode(Modes.STABILIZE, t)
                self.set_x_target(RailParams.DROPZONE_X)

        elif self.mode == Modes.PASS_WALL:
            # Si le pendule a passé le mur, on passe au prochain mode
            if x + CartPoleParams.L_ROD*np.sin(theta) > RailParams.OBSTACLE_X:
                self.set_mode(Modes.STABILIZE, t)

        elif self.mode == Modes.MOVE_TO_X:
            # Si x_target est plus grand que x initial, on vérifie x >= x_target.
            # Si x_target est plus petit, on vérifie x <= x_target.
            if self.x_target >= 0.0:
                if x >= self.x_target:
                    self.set_mode(Modes.STABILIZE, t)
            else:
                if x <= self.x_target:
                    self.set_mode(Modes.STABILIZE, t)

        elif self.mode == Modes.STABILIZE:
            if self.x_target > 0 and self.check_tol(theta, 0) and self.check_tol(dtheta, 0):
                self.set_mode(Modes.MOVE_TO_X, t)
                self.set_x_target(0)


    def set_mode(self, new_mode: Modes, t: float) -> None:
        """
        Change de mode une seule fois et garde l'historique.
        """

        if new_mode == self.mode:
            return

        self.mode = new_mode
        self.t_mode_start = t
        self.mode_history.append((t, new_mode))

        print(f"Transition vers {new_mode.name} à t = {t:.3f}")

    def set_x_target(self, new_x_target:float) -> None:
        """
        Change le target de position
        """
        self.x_target = new_x_target


    def check_tol(self, val:float, target:float) -> bool:
        """
        Vérifie qu'une valeur est dans la plage de tolérance
        True : In
        False : Out
        """
        return val <= target + self.tol and val >= target - self.tol


    def compute(self, t: float, state: np.ndarray) -> float:
        """
        Retourne la commande selon le mode courant.
        """

        self.update_mode(t, state)

        match self.mode:
            case Modes.SWING:
                return SwingController.compute(
                    t, 
                    state,
                    self.t_mode_start, 
                    self.height_target
                )
            
            case Modes.PASS_WALL:
                return MoveToPointController.compute(
                    t,
                    state,
                    self.k_goto,
                    self.t_mode_start,
                    self.x_target,
                )

            case Modes.MOVE_TO_X:
                return MoveToPointController.compute(
                    t,
                    state,
                    self.k_goto,
                    self.t_mode_start,
                    self.x_target,
                )

            case Modes.STABILIZE:
                return StabilizeController.compute(
                    t,
                    state,
                    self.k_stab,
                    self.t_mode_start,
                    self.x_target,
                )

            case _:
                raise ValueError(f"Mode inconnu : {self.mode}")
