
import numpy as np
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
        theta_swing_target: float,
        x_target: float,
        tol:float
    ):
        self.mode = Modes.SWING

        self.theta_swing_target = theta_swing_target
        self.x_target = x_target

        self.tol = tol

        self.t_mode_start = 0.0

        # Historique optionnel, utile pour déboguer ou tracer les transitions.
        self.mode_history: list[tuple[float, Modes]] = [(0.0, self.mode)]


    def update_mode(self, t: float, state: np.ndarray) -> None:
        """
        Met à jour le mode selon l'état courant.
        """

        x, dx, theta, dtheta = state

        if self.mode == Modes.SWING:
            # Si theta_swing_target = -45 deg,
            # on passe au prochain mode quand theta <= -45 deg.
            if theta <= self.theta_swing_target and dtheta <= 0:
                self.set_mode(Modes.MOVE_TO_X, t)

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
                    self.theta_swing_target
                )

            case Modes.MOVE_TO_X:
                return MoveToPointController.compute(
                    t,
                    state,
                    self.t_mode_start,
                    self.x_target,
                )

            case Modes.STABILIZE:
                return StabilizeController.compute(
                    t,
                    state,
                    self.t_mode_start,
                    self.x_target,
                )

            case _:
                raise ValueError(f"Mode inconnu : {self.mode}")
