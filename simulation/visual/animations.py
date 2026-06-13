# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np

from matplotlib.animation import FFMpegWriter, FuncAnimation
from matplotlib.patches import Circle, Rectangle
from matplotlib.widgets import Button

import simulation.cart_pole.param as const


def animate_cart_pole(
    sol,
    save_to_file: bool = False,
    filename: str = "simulation/assets/cart_pole.mp4",
):
    """
    Anime le véhicule sur rail avec pendule suspendu.

    Hypothèses :
    - sol vient de solve_ivp
    - sol.y = [x, dx, theta, dtheta]
    - theta = 0 lorsque le pendule pointe verticalement vers le bas
    - x est la position horizontale du point d'attache du pendule / véhicule
    """

    #####################################################################################
    # Paramètres temporels

    target_fps: int = 60

    num_frames = int(np.ceil((sol.t[-1] - sol.t[0]) * target_fps)) + 1
    ts = np.linspace(sol.t[0], sol.t[-1], num_frames)

    if sol.sol is not None:
        xs, thetas, dxs, dthetas = sol.sol(ts)
    else:
        xs = np.interp(ts, sol.t, sol.y[0])
        thetas = np.interp(ts, sol.t, sol.y[2])

    #####################################################################################
    # Géométrie

    rail_length = const.RAIL_LENGTH
    rail_y = 0.0

    ground_y = const.GROUND_Y

    cart_width = const.CART_LENGTH
    cart_height = const.CART_HEIGHT

    wheel_radius = const.R_WHEEL

    rod_length = const.L_ROD
    payload_radius = const.PAYLOAD_RADIUS

    obstacle_x = const.OBSTACLE_X
    obstacle_height = const.OBSTACLE_HEIGHT

    dropzone_x = const.DROPZONE_X
    dropzone_width = const.DROPZONE_WIDTH
    dropzone_height = const.DROPZONE_HEIGHT

    # Le point d'attache est sous le chariot, proche du rail.
    pivot_y = const.PIVOT_Y

    #####################################################################################
    # Position du payload

    # theta = 0 vers le bas
    payload_xs = xs + rod_length * np.sin(thetas)
    payload_ys = pivot_y - rod_length * np.cos(thetas)

    #####################################################################################
    # Limites d'affichage

    pad_x = 0.15

    x_min = min(
        np.min(xs) - cart_width,
        np.min(payload_xs) - payload_radius,
        0.0,
    ) - pad_x

    x_max = max(
        np.max(xs) + cart_width,
        np.max(payload_xs) + payload_radius,
        rail_length,
    ) + pad_x

    y_min = ground_y - 0.05

    y_max = rail_y + 0.25

    #####################################################################################
    # Figure

    fig, ax = plt.subplots(num="rail-pendulum-animation", figsize=(9, 5))

    ax.set_title("Véhicule sur rail avec pendule suspendu")
    ax.set_xlabel("Position x (m)")
    ax.set_ylabel("Hauteur y (m)")

    ax.set_xlim(x_min, x_max)
    ax.set_ylim(y_min, y_max)

    ax.set_aspect("equal", adjustable="box")
    ax.grid(True, alpha=0.3)

    #####################################################################################
    # Éléments fixes : rail, sol, obstacle, drop zone

    # Limites gauche/droite du rail
    ax.axvline(
        x=0.0,
        color="red",
        linestyle="--",
        linewidth=1,
    )

    ax.axvline(
        x=rail_length,
        color="red",
        linestyle="--",
        linewidth=1,
    )

    # Rail
    ax.axhline(
        y=rail_y,
        color="gray",
        linestyle="-",
        linewidth=4,
    )

    # Sol
    ax.axhline(
        y=ground_y,
        color="black",
        linestyle="-",
        linewidth=3,
    )

    # Obstacle
    #
    # Il part du sol et monte jusqu'à :
    # ground_y + OBSTACLE_HEIGHT
    #
    obstacle = Rectangle(
        (
            obstacle_x - const.OBSTACLE_WIDTH / 2,
            ground_y,
        ),
        width=const.OBSTACLE_WIDTH,
        height=const.OBSTACLE_HEIGHT,
        fill=True,
        facecolor="red",
        edgecolor="darkred",
        linewidth=1.5,
        zorder=50,
    )
    ax.add_patch(obstacle)

    # Drop zone
    dropzone_floor_y = ground_y + 0.01

    ax.hlines(
        y=dropzone_floor_y,
        xmin=dropzone_x - dropzone_width / 2,
        xmax=dropzone_x + dropzone_width / 2,
        color="green",
        linestyle="-",
        linewidth=5,
        zorder=100,
    )

    ax.vlines(
        x=[
            dropzone_x - dropzone_width / 2,
            dropzone_x + dropzone_width / 2,
        ],
        ymin=dropzone_floor_y,
        ymax=dropzone_floor_y + dropzone_height,
        color="green",
        linestyle="-",
        linewidth=5,
        zorder=100,
    )

    #####################################################################################
    # Éléments animés

    origin = (0.0, 0.0)

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
        linewidth=1.5,
        animated=True,
    )
    ax.add_patch(left_wheel)

    right_wheel = Circle(
        origin,
        wheel_radius,
        fill=True,
        facecolor="gray",
        edgecolor="black",
        linewidth=1.5,
        animated=True,
    )
    ax.add_patch(right_wheel)

    payload = Circle(
        origin,
        payload_radius,
        fill=True,
        facecolor="green",
        edgecolor="black",
        linewidth=2,
        animated=True,
    )
    ax.add_patch(payload)

    pivot_marker, = ax.plot(
        [],
        [],
        "ko",
        markersize=5,
        animated=True,
    )

    rod_line, = ax.plot(
        [],
        [],
        "b-",
        linewidth=2,
        animated=True,
    )

    state_text = ax.text(
        0.02,
        0.75,
        "",
        transform=ax.transAxes,
        animated=True,
    )

    #####################################################################################
    # Fonctions internes

    def set_frame(frame: int):
        x = xs[frame]
        theta = thetas[frame]

        pivot_x = x

        payload_x = payload_xs[frame]
        payload_y = payload_ys[frame]

        # Chariot centré sur x
        cart.set_xy(
            (
                x - cart_width / 2,
                rail_y - cart_height / 2,
            )
        )

        # Roues sur le rail
        left_wheel.center = (
            x - cart_width / 3,
            rail_y,
        )

        right_wheel.center = (
            x + cart_width / 3,
            rail_y,
        )

        # Pendule
        pivot_marker.set_data([pivot_x], [pivot_y])
        rod_line.set_data([pivot_x, payload_x], [pivot_y, payload_y])
        payload.center = (payload_x, payload_y)

        # Texte
        state_text.set_text(
            f"t = {ts[frame]:.2f} s\n"
            f"x = {x:.3f} m\n"
            f"theta = {np.rad2deg(theta):.1f} deg"
        )

        return (
            cart,
            left_wheel,
            right_wheel,
            pivot_marker,
            rod_line,
            payload,
            state_text,
        )

    def init():
        return set_frame(0)

    def animate(frame: int):
        return set_frame(frame)

    #####################################################################################
    # Animation

    frametime_ms = int(1000 / target_fps)

    anim = FuncAnimation(
        fig,
        animate,
        frames=num_frames,
        init_func=init,
        interval=frametime_ms,
        blit=True,
    )

    is_paused = False

    plt.subplots_adjust(bottom=0.18)

    button_ax = fig.add_axes([0.42, 0.04, 0.16, 0.06])
    play_pause_button = Button(button_ax, "Pause")


    def toggle_play_pause(event):
        nonlocal is_paused

        if is_paused:
            anim.event_source.start()
            play_pause_button.label.set_text("Pause")
            is_paused = False
        else:
            anim.event_source.stop()
            play_pause_button.label.set_text("Play")
            is_paused = True


    play_pause_button.on_clicked(toggle_play_pause)

    if save_to_file:
        print("Saving the animation to file...")

        metadata = dict(
            title="Rail pendulum animation",
            artist="Matplotlib",
            comment="Vehicle on rail with suspended pendulum",
        )

        writer = FFMpegWriter(
            fps=target_fps,
            metadata=metadata,
        )

        anim.save(filename, writer=writer)

    plt.show(block=True)
