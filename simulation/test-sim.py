import sympy as sp
import numpy as np
import sympy.physics.mechanics as mec
from sympy.physics.vector import vprint
from scipy.linalg import solve_continuous_are
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import time

MQ = 0.1
M_ROD = 0.05
LENGTH = 0.25
G = 9.81
B_FROTT = 0.005

#####################################################################################

# Symboles
mQ, mRod = sp.symbols('mQ mRod')
g = sp.Symbol('g')
L = sp.Symbol('L')

theta = mec.dynamicsymbols('theta')
dtheta = theta.diff()
ddtheta = dtheta.diff()
x = mec.dynamicsymbols('x')
dx = x.diff()
ddx = dx.diff()

#####################################################################################

# Constantes
cst = {
    mQ: MQ,
    mRod: M_ROD,
    L: LENGTH,
    g: G
}

#####################################################################################

# Calcul facteurs K
g = G
L = LENGTH

A = np.array([
    [0, 1, 0, 0],
    [0, 0, 0, 0],
    [0, 0, 0, 1],
    [0, 0, -g/L, 0]
])

B = np.array([
    [0],
    [1],
    [0],
    [-1/L]
])

# x = [x, dx, theta, dtheta]
Q = np.diag([600, 1, 100, 10])
R = np.array([[0.03]])

P = solve_continuous_are(A, B, Q, R)

K_DOWN = np.linalg.inv(R) @ B.T @ P

#print(K_DOWN)

#####################################################################################

# Calcul facteurs K

A = np.array([
    [0, 1, 0, 0],
    [0, 0, 0, 0],
    [0, 0, 0, 1],
    [0, 0, g/L, 0]
])

B = np.array([
    [0],
    [1],
    [0],
    [1/L]
])

# x = [x, dx, theta, dtheta]
Q = np.diag([70, 1, 100, 5])
R = np.array([[1]])

P = solve_continuous_are(A, B, Q, R)

K_UP = np.linalg.inv(R) @ B.T @ P

#print(K_UP)

#####################################################################################

# Repères
N = mec.ReferenceFrame('N')

B = mec.ReferenceFrame('B')
B.orient_axis(N, N.z, theta)

#####################################################################################

# Points
No = mec.Point('No')
No.set_vel(N, 0)

Bo = mec.Point('Bo')
Bo.set_pos(No, x*N.x)

Bcm = mec.Point('Bcm')
Bcm.set_pos(Bo, -0.5*L*B.y)

Bq = mec.Point('Bq')
Bq.set_pos(Bo, L*-B.y)

Scm = mec.Point('Scm')
Scm.set_pos(Bcm, mQ/(mQ+mRod)*Bq.pos_from(Bcm))

#####################################################################################

# Corps et particules
Izz = 1/3*mRod*(L**2)

#####################################################################################

# Équations
M_lhs = Bcm.pos_from(Bo).cross(-mRod*g*N.y) + Bq.pos_from(Bo).cross(-mQ*g*N.y) - B_FROTT*dtheta*N.z
M_rhs = (Izz + mQ*(L**2))*B.ang_acc_in(N) + (mRod + mQ)*Scm.pos_from(Bo).cross(Bo.acc(N))

eq = sp.Eq(M_lhs.dot(N.z), M_rhs.dot(N.z))

sol1 = sp.solve(eq, ddtheta)
sol2 = sp.solve(eq, ddx)
vprint(sol1[0].simplify())

get_ang_acc = sp.lambdify((theta, dtheta, ddx), sol1[0].subs(cst), modules="numpy")
get_acc = sp.lambdify((theta, dtheta, ddtheta), sol2[0].subs(cst), modules="numpy")
#print(sol2[0].simplify())

pos_bq = (Bq.pos_from(No).dot(N.x).subs(cst), Bq.pos_from(No).dot(N.y).subs(cst))
get_pos_bq = sp.lambdify((theta, x), pos_bq, modules="numpy")

#####################################################################################
                                ### SIMULATION ###
#####################################################################################

V_MAX = 0.4
MAX_ACCEL = 7

FACTOR = 0.5

ANGLE_THRESH = 0.7

target_pos = 0.25

pos_x = 0.25
v_x = 0
a_x = 0

angle = 0.1
dangle = 0
ddangle = 0

start_time = 0
last_time = 0
current_time = 0

increasing = False

def swing_up(target_height):
    global angle, dangle, pos_x, a_x

    k = 2.5 # Factor

    E = 0.5*MQ*(LENGTH**2)*(dangle**2) + MQ*G*LENGTH*(1 - np.cos(angle))
    E_TARGET = MQ*G*target_height

    u = k*(E - E_TARGET)*dangle*np.cos(angle) + (0.3 - pos_x)

    a_x = np.clip(u, -MAX_ACCEL, MAX_ACCEL)

def go_upright(x_target):
    global pos_x, v_x, a_x, angle, dangle

    c = 0.15 # Friction correction

    if abs(np.sin(angle)) < 0.3 and np.cos(angle) < 0:
        error_state = np.array([
            pos_x - x_target,
            v_x,
            -np.sin(angle),
            dangle
        ])

        u = -(K_UP @ error_state)[0]

        a_x = np.clip(u, -MAX_ACCEL, MAX_ACCEL)
    else:
        swing_up(2*LENGTH + c)

def go_upside_down(x_target):
    global pos_x, v_x, a_x, angle, dangle

    if np.cos(angle) < 0:
        a_x = 2*(-np.cos(angle) - 0.8 - v_x)
    else:
        error_state = np.array([
            pos_x - x_target,
            v_x,
            np.sin(angle),
            dangle
        ])

        u = -(K_DOWN @ error_state)[0]

        a_x = np.clip(u, -MAX_ACCEL, MAX_ACCEL)

        print(a_x)

# 1. Setup the figure, axis, and empty plot elements
fig, ax = plt.subplots()
ax.set_xlim(0, 1.7)
ax.set_ylim(-0.5, 0.5)
ax.set_aspect('equal', adjustable='box')
ax.plot([0, 170], [0, 0], '--r', lw=1)
ax.plot([0.85, 0.85], [-0.5, -LENGTH + 0.01], 'black', lw=5)
line, = ax.plot([], [], lw=2, color='blue', marker='o')

TIME_SPEED = 1.0

# 2. Define the animation update function
def update(frame):
    global start_time, last_time, current_time, pos_x, v_x, a_x, angle, dangle, ddangle, target_pos

    current_time = time.time()*TIME_SPEED

    if last_time != 0:
        dt = current_time - last_time

        pos_x += v_x*dt + 0.5*a_x*dt**2
        v_x += a_x*dt

        angle += dangle*dt + 0.5*ddangle*dt**2

        dangle += ddangle*dt
        ddangle = get_ang_acc(angle, dangle, a_x)

        #print(round(current_time - start_time, 2), "s")

        if current_time - start_time < 4:
            go_upright(0.4)
        elif current_time - start_time < 8:
            go_upright(1.25)
        elif current_time - start_time < 20:
            go_upside_down(1.25)
        
        x_q, y_q = get_pos_bq(angle, pos_x)

        line.set_data([pos_x, x_q], [0, y_q])
    else:
        start_time = time.time() * TIME_SPEED

    last_time = current_time

    return line,

# 3. Create the animation object
# CRITICAL: You must save this to a variable so Python doesn't garbage collect it!
ani = FuncAnimation(fig, update, interval=10, blit=True)

plt.show()