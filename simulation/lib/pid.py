# -*- coding: utf-8 -*-
#!/usr/bin/env python3


class PIDController:
    def __init__(
        self,
        kp: float = 1.0,
        ki: float = 0.0,
        kd: float = 0.0,
        saturate_command: bool = False,
        min_command: float | None = None,
        max_command: float | None = None,
        anti_windup: bool = False,
        i_term_sat_low: float | None = None,
        i_term_sat_high: float | None = None,
        derivative_smoothing_factor: float = 1.0,
    ) -> None:
        self.kp = kp
        self.ki = ki
        self.kd = kd

        # Output saturation
        self.saturate_command = saturate_command
        self.min_command = min_command
        self.max_command = max_command

        # Integral term saturation (anti-windup)
        self.anti_windup = anti_windup
        self.i_term_sat_low = i_term_sat_low
        self.i_term_sat_high = i_term_sat_high

        # Derivative filtering (2 terms moving average)
        if not (0.0 <= derivative_smoothing_factor <= 1.0):
            raise ValueError("Smoothing factor is out of range. Valid range: [0, 1].")
        self.alpha = derivative_smoothing_factor  # 1.0 means only current term

        self.error_integral = 0.0
        self.prev_error = 0.0
        self.prev_error_derivative = 0.0
        self.prev_time = 0.0

    def saturate_output(self, on: bool = True):
        self.saturate_command = on

    def saturate_i_term(self, on: bool = True):
        self.anti_windup = on

    def reset(self):
        """Resets error attributes."""
        self.error_integral = 0.0
        self.prev_error = 0.0
        self.prev_error_derivative = 0.0

    def compute_command(self, t: float, reference: float, measured: float):
        dt = t - self.prev_time

        error = reference - measured
        p_term = self.kp * error

        self.error_integral += error * dt
        if self.anti_windup and (
            self.i_term_sat_low is not None or self.i_term_sat_high is not None
        ):
            self.error_integral = max(
                self.i_term_sat_low or self.error_integral,
                min(self.error_integral, self.i_term_sat_high or self.error_integral),
            )
        i_term = self.ki * self.error_integral

        if dt > 1e-9:
            # NOTE: Reduce error by 1000 because is caused instability and
            #       VERY long numerical resolution times.
            error_derivative = (error - self.prev_error) / (dt * 1000)
        else:
            error_derivative = 0.0
        # Filter with moving average
        error_derivative = (
            self.alpha * error_derivative + (1 - self.alpha) * self.prev_error_derivative
        )
        d_term = self.kd * error_derivative

        # Update memory
        self.prev_error = error
        self.prev_error_derivative = error_derivative
        self.prev_time = t

        command = p_term + i_term + d_term
        if self.saturate_command and (self.min_command is not None or self.max_command is not None):
            command = max(self.min_command or command, min(command, self.max_command or command))

        return command
