# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FFMpegWriter, FuncAnimation
from matplotlib.patches import Circle, Rectangle

from simulation.cart_pole.param import CartPoleParams, Rail


def animate_pendulum(
    sol,
    constants: dict[str, float],
):
    """
    Animates a simple (gravity) pendulum.

    Parameters:
        sol (Sequence[float]): Solution of the numerical integration.
        constants (dict[str, float]): Constants of the pendulum (g, L, b, m, etc.).
    """
    # 1:1 time for animation
    target_fps: int = 60
    num_frames = int(np.ceil((sol.t[-1] - sol.t[0]) * target_fps)) + 1
    ts = np.linspace(sol.t[0], sol.t[-1], num_frames)

    # Evaluate solution at animation timesteps
    # NOTE: We assume the solution is valid.
    thetas, _ = sol.sol(ts)

    # Geometry
    L = constants.get("L", (CartPoleParams.rod_len + CartPoleParams.bob_height / 2))
    pivot_x, pivot_y = (0, 0)
    gnd_y = 0.0
    bob_radius = 0.06 * L
    bob_xs = L * np.sin(thetas)
    bob_ys = gnd_y - L * np.cos(thetas)

    fig, ax = plt.subplots(num="pendulum-animation", figsize=(6, 6))
    ax.set_title("Pendule Simple")
    ax.set_xlabel("cart_x (m)")
    ax.set_ylabel("y (m)")
    ax.set_aspect("equal")
    ax.grid(True, alpha=0.3)

    # Track limits
    pad = 1.2 * Rail.length / 2
    ax.set_xlim(-pad, pad)
    ax.set_ylim(-pad, pad)

    # Static line
    ax.axhline(y=gnd_y, color="gray", linestyle="-", linewidth=3)

    # Elements of the pendulum
    (pivot,) = ax.plot([], [], "k^", markersize=8)
    (line,) = ax.plot([], [], "b-", linewidth=2)
    bob = Circle(
        (pivot_x, pivot_y),
        bob_radius,
        fill=True,
        facecolor="green",
        edgecolor="black",
        linewidth=2,
        animated=True,
    )
    ax.add_patch(bob)

    # Text elements
    time_text = ax.text(0.02, 0.95, "", transform=ax.transAxes)

    def init():
        pivot.set_data([pivot_x], [pivot_y])
        line.set_data([pivot_x, bob_xs[0]], [pivot_y, bob_ys[0]])
        bob.center = (bob_xs[0], bob_ys[0])
        time_text.set_text("")
        return pivot, line, bob, time_text

    def animate(frame: int):
        bob_x, bob_y = bob_xs[frame], bob_ys[frame]
        line.set_data([pivot_x, bob_x], [pivot_y, bob_y])
        bob.center = (bob_x, bob_y)
        time_text.set_text(f"Time: {ts[frame]:.2f}s")

        return pivot, line, bob, time_text

    frametime = int(1 / target_fps)
    __anim = FuncAnimation(
        fig, animate, num_frames, init, interval=frametime, blit=True
    )

    plt.show(block=True)


def animate_cart_pole(
    sol,
    save_to_file: bool = False,
    inverse: bool = False,
    new: bool = False,
):
    """
    Animates a cart-pole system.

    Parameters:
        sol: Solution of the numerical integration.
        constants (dict[str, float]): Constants of the cart-pole (g, L, b, m, etc.).
    """
    # 1:1 time for animation
    target_fps: int = 60
    if new:
        sol_ts, xs, thetas = sol
        num_frames = int(np.ceil((sol_ts[-1] - sol_ts[0]) * target_fps)) + 1
        ts = np.linspace(sol_ts[0], sol_ts[-1], num_frames)
        xs = np.interp(ts, sol_ts, xs)
        thetas = np.interp(ts, sol_ts, thetas)
    else:
        num_frames = int(np.ceil((sol.t[-1] - sol.t[0]) * target_fps)) + 1
        ts = np.linspace(sol.t[0], sol.t[-1], num_frames)

        # Evaluation solution at animation timesteps
        if sol.sol is not None:
            xs, x_dots, thetas, theta_dots = sol.sol(ts)
        else:
            # NOTE: Should not happen if `dense_output=True`.
            xs = np.interp(ts, sol.t, sol.y[0])
            thetas = np.interp(ts, sol.t, sol.y[2])

    # Geometry
    L = CartPoleParams.rod_len + CartPoleParams.bob_height / 2
    origin = (0.0, 0.0)
    track_y = 0.0
    ground_y = track_y - Rail.rail_height
    cart_width = CartPoleParams.cart_length
    cart_height = CartPoleParams.cart_height
    wheel_radius = CartPoleParams.wheel_radius
    bob_radius = CartPoleParams.bob_height / 2

    bob_xs = xs + L * np.sin(thetas)
    pivot_y = track_y + cart_height / 2 + wheel_radius
    if inverse:
        bob_ys = pivot_y + L * np.cos(thetas)
    else:
        bob_ys = pivot_y - L * np.cos(thetas)

    # Track limits
    pad_x = 1.2 * L
    x_min = min(np.min(xs) - pad_x, 0.0)
    x_max = max(np.max(xs) + pad_x, Rail.length)
    y_min = ground_y
    y_max = pivot_y + L + bob_radius + 0.5

    fig, ax = plt.subplots(num="cart-pole-animation", figsize=(8, 6))
    ax.set_title("Cart-Pole System")
    ax.set_xlabel("Position (m)")
    ax.set_ylabel("Height (m)")
    ax.set_xlim(x_min, x_max)
    ax.set_ylim(y_min, y_max)
    ax.set_aspect("equal", adjustable="box")
    ax.grid(True, alpha=0.3)

    # World elements
    ax.axvline(x=0.0 - 0.01, color="red", linestyle="-", linewidth=2)  # Left rail limit
    ax.axvline(
        x=Rail.length + 0.01, color="red", linestyle="-", linewidth=2
    )  # Right rail limit
    ax.axhline(y=track_y, color="gray", linestyle="-", linewidth=3)
    ax.axhline(y=ground_y, color="black", linestyle="-", linewidth=3)
    ax.vlines(
        x=Rail.obstacle_x,
        ymin=ground_y,
        ymax=ground_y + Rail.obstacle_height,
        color="red",
        linestyle="-",
        linewidth=5,
    )
    # Dropzone
    ax.hlines(
        y=ground_y + 0.01,
        xmin=Rail.dropzone_x - 0.1,
        xmax=Rail.dropzone_x + 0.1,
        color="green",
        linestyle="-",
        linewidth=5,
        zorder=100,
    )
    ax.vlines(
        x=[Rail.dropzone_x - 0.1, Rail.dropzone_x + 0.1],
        ymin=[ground_y + 0.01, ground_y + 0.01],
        ymax=[ground_y + 0.1, ground_y + 0.1],
        color="green",
        linestyle="-",
        linewidth=5,
        zorder=100,
    )

    # Patches
    cart = Rectangle(
        origin,
        cart_width,
        cart_height,
        fill=True,
        facecolor="lightblue",
        edgecolor="black",
        linewidth=2,
        animated=True,
    )
    ax.add_patch(cart)
    left_wheel = Circle(
        origin,
        wheel_radius,
        fill=True,
        facecolor="gray",
        edgecolor="black",
        linewidth=2,
        animated=True,
    )
    ax.add_patch(left_wheel)
    right_wheel = Circle(
        origin,
        wheel_radius,
        fill=True,
        facecolor="gray",
        edgecolor="black",
        linewidth=2,
        animated=True,
    )
    ax.add_patch(right_wheel)
    bob = Circle(
        origin,
        bob_radius,
        fill=True,
        facecolor="green",
        edgecolor="black",
        linewidth=2,
        animated=True,
    )
    ax.add_patch(bob)

    # Other elements
    (pivot,) = ax.plot([], [], "k^", markersize=8)
    (pole_line,) = ax.plot([], [], "b-", linewidth=2)
    time_text = ax.text(0.02, 0.95, "", transform=ax.transAxes)

    def init():
        cart.set_xy((xs[0] - cart_width / 2, track_y + wheel_radius))
        left_wheel.center = (xs[0] - cart_width / 4, track_y + wheel_radius)
        right_wheel.center = (xs[0] + cart_width / 4, track_y + wheel_radius)
        bob.center = (bob_xs[0], bob_ys[0])
        pivot.set_data([xs[0]], [pivot_y])
        pole_line.set_data([xs[0], bob_xs[0]], [pivot_y, bob_ys[0]])
        time_text.set_text("")
        return cart, left_wheel, right_wheel, pivot, pole_line, bob, time_text

    def animate(frame: int):
        cart_x = xs[frame]
        bob_x, bob_y = bob_xs[frame], bob_ys[frame]
        cart.set_x(cart_x - cart_width / 2)
        left_wheel.center = (cart_x - cart_width / 3, track_y + wheel_radius)
        right_wheel.center = (cart_x + cart_width / 3, track_y + wheel_radius)
        bob.center = (bob_x, bob_y)
        pivot.set_xdata([cart_x])
        pole_line.set_data([cart_x, bob_x], [pivot_y, bob_y])
        time_text.set_text(f"Time: {ts[frame]:.2f}s")
        return cart, left_wheel, right_wheel, pivot, pole_line, bob, time_text

    frametime = int(1000 / target_fps)
    __anim = FuncAnimation(
        fig, animate, num_frames, init, interval=frametime, blit=True
    )

    if save_to_file:
        print("Saving the animation to file...")
        metadata = dict(
            title="Cart-pole animation",
            artist="Matplotlib",
            comment="Simple cart-pole system",
        )
        writer = FFMpegWriter(target_fps, metadata=metadata)
        __anim.save("simulation/assets/cart_pole.mp4", writer=writer)

    plt.show(block=True)
