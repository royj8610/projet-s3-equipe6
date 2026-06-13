# -*- coding: utf-8 -*-
#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np

from matplotlib.animation import FFMpegWriter, FuncAnimation
from matplotlib.patches import Circle, Rectangle

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

    ground_y = rail_y - const.RAIL_HEIGHT

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
    pivot_y = rail_y - cart_height / 2

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

    y_min = min(
        ground_y,
        np.min(payload_ys) - payload_radius,
    ) - 0.05

    y_max = max(
        rail_y + 0.15,
        ground_y + obstacle_height + 0.05,
    )

    print("x min/max:", np.min(xs), np.max(xs))
    print("payload y min/max:", np.min(payload_ys), np.max(payload_ys))
    print("xlim:", x_min, x_max)
    print("ylim:", y_min, y_max)

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
        linestyle="-",
        linewidth=2,
    )

    ax.axvline(
        x=rail_length,
        color="red",
        linestyle="-",
        linewidth=2,
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
            obstacle_x - 0.01,
            ground_y,
        ),
        width=0.02,
        height=obstacle_height,
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

    time_text = ax.text(
        0.02,
        0.94,
        "",
        transform=ax.transAxes,
        animated=True,
    )

    state_text = ax.text(
        0.02,
        0.84,
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

        # Textes
        time_text.set_text(f"t = {ts[frame]:.2f} s")
        state_text.set_text(
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
            time_text,
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




# # -*- coding: utf-8 -*-
# #!/usr/bin/env python3

# import matplotlib.pyplot as plt
# import numpy as np
# from matplotlib.animation import FFMpegWriter, FuncAnimation
# from matplotlib.patches import Circle, Rectangle

# from simulation.cart_pole.param import CartPoleParams, Rail

# def animate_cart_pole(
#     sol,
#     save_to_file: bool = False,
#     inverse: bool = False,
#     new: bool = False,
# ):
#     """
#     Animates a cart-pole system.

#     Parameters:
#         sol: Solution of the numerical integration.
#         constants (dict[str, float]): Constants of the cart-pole (g, L, b, m, etc.).
#     """
#     # 1:1 time for animation
#     target_fps: int = 60
#     if new:
#         sol_ts, xs, thetas = sol
#         num_frames = int(np.ceil((sol_ts[-1] - sol_ts[0]) * target_fps)) + 1
#         ts = np.linspace(sol_ts[0], sol_ts[-1], num_frames)
#         xs = np.interp(ts, sol_ts, xs)
#         thetas = np.interp(ts, sol_ts, thetas)
#     else:
#         num_frames = int(np.ceil((sol.t[-1] - sol.t[0]) * target_fps)) + 1
#         ts = np.linspace(sol.t[0], sol.t[-1], num_frames)

#         # Evaluation solution at animation timesteps
#         if sol.sol is not None:
#             xs, x_dots, thetas, theta_dots = sol.sol(ts)
#         else:
#             # NOTE: Should not happen if `dense_output=True`.
#             xs = np.interp(ts, sol.t, sol.y[0])
#             thetas = np.interp(ts, sol.t, sol.y[2])

#     # Geometry
#     L = CartPoleParams.rod_len + CartPoleParams.bob_height / 2
#     origin = (0.0, 0.0)
#     track_y = 0.0
#     ground_y = track_y - Rail.rail_height
#     cart_width = CartPoleParams.cart_length
#     cart_height = CartPoleParams.cart_height
#     wheel_radius = CartPoleParams.wheel_radius
#     bob_radius = CartPoleParams.bob_height / 2

#     bob_xs = xs + L * np.sin(thetas)
#     pivot_y = track_y + cart_height / 2 + wheel_radius
#     if inverse:
#         bob_ys = pivot_y + L * np.cos(thetas)
#     else:
#         bob_ys = pivot_y - L * np.cos(thetas)

#     # Track limits
#     pad_x = 1.2 * L
#     x_min = min(np.min(xs) - pad_x, 0.0)
#     x_max = max(np.max(xs) + pad_x, Rail.length)
#     y_min = ground_y
#     y_max = pivot_y + L + bob_radius + 0.5

#     fig, ax = plt.subplots(num="cart-pole-animation", figsize=(8, 6))
#     ax.set_title("Cart-Pole System")
#     ax.set_xlabel("Position (m)")
#     ax.set_ylabel("Height (m)")
#     ax.set_xlim(x_min, x_max)
#     ax.set_ylim(y_min, y_max)
#     ax.set_aspect("equal", adjustable="box")
#     ax.grid(True, alpha=0.3)

#     # World elements
#     ax.axvline(x=0.0 - 0.01, color="red", linestyle="-", linewidth=2)  # Left rail limit
#     ax.axvline(
#         x=Rail.length + 0.01, color="red", linestyle="-", linewidth=2
#     )  # Right rail limit
#     ax.axhline(y=track_y, color="gray", linestyle="-", linewidth=3)
#     ax.axhline(y=ground_y, color="black", linestyle="-", linewidth=3)
#     ax.vlines(
#         x=Rail.obstacle_x,
#         ymin=ground_y,
#         ymax=ground_y + Rail.obstacle_height,
#         color="red",
#         linestyle="-",
#         linewidth=5,
#     )
#     # Dropzone
#     ax.hlines(
#         y=ground_y + 0.01,
#         xmin=Rail.dropzone_x - 0.1,
#         xmax=Rail.dropzone_x + 0.1,
#         color="green",
#         linestyle="-",
#         linewidth=5,
#         zorder=100,
#     )
#     ax.vlines(
#         x=[Rail.dropzone_x - 0.1, Rail.dropzone_x + 0.1],
#         ymin=[ground_y + 0.01, ground_y + 0.01],
#         ymax=[ground_y + 0.1, ground_y + 0.1],
#         color="green",
#         linestyle="-",
#         linewidth=5,
#         zorder=100,
#     )

#     # Patches
#     cart = Rectangle(
#         origin,
#         cart_width,
#         cart_height,
#         fill=True,
#         facecolor="lightblue",
#         edgecolor="black",
#         linewidth=2,
#         animated=True,
#     )
#     ax.add_patch(cart)
#     left_wheel = Circle(
#         origin,
#         wheel_radius,
#         fill=True,
#         facecolor="gray",
#         edgecolor="black",
#         linewidth=2,
#         animated=True,
#     )
#     ax.add_patch(left_wheel)
#     right_wheel = Circle(
#         origin,
#         wheel_radius,
#         fill=True,
#         facecolor="gray",
#         edgecolor="black",
#         linewidth=2,
#         animated=True,
#     )
#     ax.add_patch(right_wheel)
#     bob = Circle(
#         origin,
#         bob_radius,
#         fill=True,
#         facecolor="green",
#         edgecolor="black",
#         linewidth=2,
#         animated=True,
#     )
#     ax.add_patch(bob)

#     # Other elements
#     (pivot,) = ax.plot([], [], "k^", markersize=8)
#     (pole_line,) = ax.plot([], [], "b-", linewidth=2)
#     time_text = ax.text(0.02, 0.95, "", transform=ax.transAxes)

#     def init():
#         cart.set_xy((xs[0] - cart_width / 2, track_y + wheel_radius))
#         left_wheel.center = (xs[0] - cart_width / 4, track_y + wheel_radius)
#         right_wheel.center = (xs[0] + cart_width / 4, track_y + wheel_radius)
#         bob.center = (bob_xs[0], bob_ys[0])
#         pivot.set_data([xs[0]], [pivot_y])
#         pole_line.set_data([xs[0], bob_xs[0]], [pivot_y, bob_ys[0]])
#         time_text.set_text("")
#         return cart, left_wheel, right_wheel, pivot, pole_line, bob, time_text

#     def animate(frame: int):
#         cart_x = xs[frame]
#         bob_x, bob_y = bob_xs[frame], bob_ys[frame]
#         cart.set_x(cart_x - cart_width / 2)
#         left_wheel.center = (cart_x - cart_width / 3, track_y + wheel_radius)
#         right_wheel.center = (cart_x + cart_width / 3, track_y + wheel_radius)
#         bob.center = (bob_x, bob_y)
#         pivot.set_xdata([cart_x])
#         pole_line.set_data([cart_x, bob_x], [pivot_y, bob_y])
#         time_text.set_text(f"Time: {ts[frame]:.2f}s")
#         return cart, left_wheel, right_wheel, pivot, pole_line, bob, time_text

#     frametime = int(1000 / target_fps)
#     __anim = FuncAnimation(
#         fig, animate, num_frames, init, interval=frametime, blit=True
#     )

#     if save_to_file:
#         print("Saving the animation to file...")
#         metadata = dict(
#             title="Cart-pole animation",
#             artist="Matplotlib",
#             comment="Simple cart-pole system",
#         )
#         writer = FFMpegWriter(target_fps, metadata=metadata)
#         __anim.save("simulation/assets/cart_pole.mp4", writer=writer)

#     plt.show(block=True)
