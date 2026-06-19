# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np

from matplotlib.animation import FFMpegWriter, FuncAnimation
from matplotlib.patches import Circle, Rectangle
from matplotlib.widgets import Button, Slider

from simulation.cart_pole.param import CartPoleParams, RailParams


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
        thetas = np.interp(ts, sol.t, sol.y[1])
        dxs = np.interp(ts, sol.t, sol.y[2])
        dthetas = np.interp(ts, sol.t, sol.y[3])

    #####################################################################################
    # Géométrie

    rail_length = RailParams.RAIL_LENGTH
    rail_y = 0.0

    ground_y = RailParams.GROUND_Y

    cart_width = CartPoleParams.CART_LENGTH
    cart_height = CartPoleParams.CART_HEIGHT

    wheel_radius = CartPoleParams.R_WHEEL

    rod_length = CartPoleParams.L_ROD
    payload_radius = RailParams.PAYLOAD_RADIUS

    obstacle_x = RailParams.OBSTACLE_X
    obstacle_height = RailParams.OBSTACLE_HEIGHT

    dropzone_x = RailParams.DROPZONE_X
    dropzone_width = RailParams.DROPZONE_WIDTH
    dropzone_height = RailParams.DROPZONE_HEIGHT

    # Le point d'attache est sous le chariot, proche du rail.
    pivot_y = RailParams.PIVOT_Y

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

    #ax.set_xlim(x_min, x_max)
    ax.set_xlim(x_min, 1.7)
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
        zorder=1,
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
            obstacle_x - RailParams.OBSTACLE_WIDTH / 2,
            ground_y,
        ),
        width=RailParams.OBSTACLE_WIDTH,
        height=RailParams.OBSTACLE_HEIGHT,
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
        animated=False,
    )
    ax.add_patch(cart)

    left_wheel = Circle(
        origin,
        wheel_radius,
        fill=True,
        facecolor="gray",
        edgecolor="black",
        linewidth=1.5,
        animated=False,
    )
    ax.add_patch(left_wheel)

    right_wheel = Circle(
        origin,
        wheel_radius,
        fill=True,
        facecolor="gray",
        edgecolor="black",
        linewidth=1.5,
        animated=False,
    )
    ax.add_patch(right_wheel)

    payload = Circle(
        origin,
        payload_radius,
        fill=True,
        facecolor="green",
        edgecolor="black",
        linewidth=2,
        animated=False,
    )
    ax.add_patch(payload)

    pivot_marker, = ax.plot(
        [],
        [],
        "ko",
        markersize=5,
        animated=False,
    )

    rod_line, = ax.plot(
        [],
        [],
        "b-",
        linewidth=2,
        animated=False,
    )

    state_text = ax.text(
        0.02,
        0.80,
        "",
        transform=ax.transAxes,
        animated=False,
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


    #####################################################################################
    # Contrôles : slider + play/pause

    plt.subplots_adjust(bottom=0.22)

    # État partagé
    animation_state = {
        "frame": 0,
        "is_paused": True,
        "updating_slider": False,
    }

    # Slider
    slider_ax = fig.add_axes([0.15, 0.08, 0.70, 0.04])

    frame_slider = Slider(
        ax=slider_ax,
        label="Temps",
        valmin=0,
        valmax=num_frames - 1,
        valinit=0,
        valstep=1,
    )

    # Bouton Play/Pause
    button_ax = fig.add_axes([0.42, 0.02, 0.16, 0.045])
    play_pause_button = Button(button_ax, "Play")


    def show_frame(frame: int):
        """
        Affiche une frame donnée et synchronise le slider.
        """
        frame = int(np.clip(frame, 0, num_frames - 1))
        animation_state["frame"] = frame

        set_frame(frame)

        # Évite une boucle infinie :
        # set_val -> update_from_slider -> show_frame -> set_val -> ...
        animation_state["updating_slider"] = True
        frame_slider.set_val(frame)
        animation_state["updating_slider"] = False

        fig.canvas.draw_idle()


    def update_from_slider(val):
        """
        Appelé quand l'utilisateur déplace le slider.
        """
        if animation_state["updating_slider"]:
            return

        animation_state["is_paused"] = True
        play_pause_button.label.set_text("Play")

        show_frame(int(val))


    def toggle_play_pause(event):
        """
        Appelé quand l'utilisateur clique sur Play/Pause.
        """
        animation_state["is_paused"] = not animation_state["is_paused"]

        if animation_state["is_paused"]:
            play_pause_button.label.set_text("Play")
        else:
            play_pause_button.label.set_text("Pause")


    def animate_auto(_):
        """
        Fonction appelée automatiquement par FuncAnimation.
        Elle avance la frame seulement si l'animation n'est pas en pause.
        """
        if animation_state["is_paused"]:
            return set_frame(animation_state["frame"])

        next_frame = animation_state["frame"] + 1

        if next_frame >= num_frames:
            next_frame = num_frames - 1
            animation_state["is_paused"] = True
            play_pause_button.label.set_text("Play")

        show_frame(next_frame)

        return set_frame(animation_state["frame"])


    frame_slider.on_changed(update_from_slider)
    play_pause_button.on_clicked(toggle_play_pause)

    # Afficher la première frame au départ
    show_frame(0)

    frametime_ms = int(1000 / target_fps)

    anim = FuncAnimation(
        fig,
        animate_auto,
        interval=frametime_ms,
        blit=False,
    )


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
