import sympy as sp
import numpy as np
import sympy.physics.mechanics as mec
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

sol1 = sp.solve(eq.subs(cst), ddtheta)
sol2 = sp.solve(eq.subs(cst), ddx)

get_ang_acc = sp.lambdify((theta, dtheta, ddx), sol1[0], modules="numpy")
get_acc = sp.lambdify((theta, dtheta, ddtheta), sol2[0], modules="numpy")
print(sol2[0])

pos_bq = (Bq.pos_from(No).dot(N.x).subs(cst), Bq.pos_from(No).dot(N.y).subs(cst))
get_pos_bq = sp.lambdify((theta, x), pos_bq, modules="numpy")

#####################################################################################
                                ### SIMULATION ###
#####################################################################################

V_MAX = 0.4
MAX_ACCEL = 7

FACTOR = 0.5

ANGLE_THRESH = 0.6

pos_x = 0.25
v_x = 0
a_x = 0

angle = 0
dangle = 0
ddangle = 0

start_time = 0
last_time = 0
current_time = 0

def increase(target_x):
    global angle, dangle, pos_x, v_x, a_x

    if abs(angle) < 0.01 and abs(dangle) < 0.01:
        a_x = MAX_ACCEL
    elif dangle > 0:
        if (v_x > -V_MAX + (target_x - pos_x)*FACTOR) and angle > -ANGLE_THRESH:
            a_x = -MAX_ACCEL
        else:
            a_x = 0
    else:
        if (v_x < V_MAX + (target_x - pos_x)*FACTOR) and angle < ANGLE_THRESH:
            a_x = MAX_ACCEL
        else:
            a_x = 0

def secret(target_x):
    global angle, dangle, pos_x, v_x, a_x

    relative = 0

    if angle < 0:
        relative = (angle % (-2*np.pi)) + 2*np.pi
    else:
        relative = angle % (2*np.pi)

    if relative < 3*np.pi/4 or relative > 5*np.pi/4:
        increase(target_x)
    elif relative < np.pi:
        if dangle > 0:
            target_ddang = -(dangle**2)/(2*(np.pi - relative))
            target_acc = get_acc(angle, dangle, target_ddang)

            if target_acc < 0:
                if v_x > -V_MAX:
                    a_x = max(-MAX_ACCEL, target_acc)
                else:
                    a_x = 0
            else:
                if v_x < V_MAX:
                    a_x = min(MAX_ACCEL, target_acc)
                else:
                    a_x = 0
        else:
            a_x = -MAX_ACCEL
    else:
        if dangle < 0:
            target_ddang = -(dangle**2)/(2*(np.pi - relative))
            target_acc = get_acc(angle, dangle, target_ddang)

            if target_acc < 0:
                if v_x > -V_MAX:
                    a_x = max(-MAX_ACCEL, target_acc)
                else:
                    a_x = 0
            else:
                if v_x < V_MAX:
                    a_x = min(MAX_ACCEL, target_acc)
                else:
                    a_x = 0

def decrease(target_x):
    global angle, dangle, pos_x, v_x, a_x

    if abs(angle) < 0.1 and abs(dangle) < 1:
        if v_x > 0.05:
            a_x = -MAX_ACCEL
        elif v_x < -0.05:
            a_x = MAX_ACCEL
        else:
            a_x = 0
            v_x = 0
    elif dangle < 0:
        if (v_x > -V_MAX) and angle < ANGLE_THRESH:
            a_x = -MAX_ACCEL
        else:
            a_x = 0
    else:
        if (v_x < V_MAX) and angle > -ANGLE_THRESH:
            a_x = MAX_ACCEL
        else:
            a_x = 0

def decreaseV2(target_x):
    global angle, dangle, pos_x, v_x, a_x

    relative = 0

    if angle < 0:
        relative = angle % (-2*np.pi)
    else:
        relative = angle % (2*np.pi)

    angle_target = angle - relative

    if abs(relative) < 0.01 and abs(dangle) < 0.1:
        if v_x > 0.05:
            a_x = -MAX_ACCEL
        elif v_x < -0.05:
            a_x = MAX_ACCEL
        else:
            a_x = 0
            v_x = 0
    elif np.sign(angle) != np.sign(dangle):
        target_ddang = -(dangle**2)/(2*(angle_target - angle))
        target_acc = get_acc(angle, dangle, target_ddang)

        if target_acc < 0:
            if v_x > -V_MAX:
                a_x = max(-MAX_ACCEL, target_acc)
            else:
                a_x = 0
        else:
            if v_x < V_MAX:
                a_x = min(MAX_ACCEL, target_acc)
            else:
                a_x = 0
    else:
        a_x = 0


# 1. Setup the figure, axis, and empty plot elements
fig, ax = plt.subplots()
ax.set_xlim(0, 1.7)
ax.set_ylim(-0.5, 0.2)
ax.set_aspect('equal', adjustable='box')
ax.plot([0, 170], [0, 0], '--r', lw=1)
ax.plot([0.85, 0.85], [-0.5, -LENGTH + 0.01 + 0.093], 'black', lw=5)
line, = ax.plot([], [], lw=2, color='blue', marker='o')

# 2. Define the animation update function
def update(frame):
    global start_time, last_time, current_time, pos_x, v_x, a_x, angle, dangle, ddangle

    current_time = time.time()

    if last_time != 0:
        dt = current_time - last_time

        pos_x += v_x*dt + 0.5*a_x*dt**2
        v_x += a_x*dt

        angle += dangle*dt + 0.5*ddangle*dt**2

        dangle += ddangle*dt
        ddangle = get_ang_acc(angle, dangle, a_x)

        if current_time - start_time > 5:
            decreaseV2(0.8)
        elif current_time - start_time > 2:
            increase(0.25)
        
        x_q, y_q = get_pos_bq(angle, pos_x)

        line.set_data([pos_x, x_q], [0, y_q])
    else:
        start_time = time.time()

    last_time = current_time

    return line,

# 3. Create the animation object
# CRITICAL: You must save this to a variable so Python doesn't garbage collect it!
ani = FuncAnimation(fig, update, interval=10, blit=True)

plt.show()