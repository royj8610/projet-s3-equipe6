## Développer la résolution de la dynamique du cartpole et des fonctions mathématiques associées

import sympy as sp
import sympy.physics.mechanics as mec

import param as const

#####################################################################################

# Symboles Constantes
mA, mQ, mRod = sp.symbols('mA mQ mRod')
g = sp.Symbol('g')
L = sp.Symbol('L')

# Symboles forces
Fn, Ft = sp.symbols('Fn Ft')
Tm = sp.symbols('Tm')

# Symboles dynamiques
x, theta = mec.dynamicsymbols('x theta')
dx, dtheta = x.diff(), theta.diff()
ddx, ddtheta = dx.diff(), dtheta.diff()

#####################################################################################

# Constantes
cst = {
    mA: const.M_CHARIOT,
    mQ: const.M_SAPIN,
    mRod: const.M_ROD,
    L: const.L_ROD,
    g: const.G
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

Bo = No.locatenew('Bo', x*N.x)
Bcm = Bo.locatenew('Bcm', 0.5*L*-B.y)
Bc = No.locatenew('Bo', L*-B.y)

Scm = mec.Point('Scm')
Scm.set_pos(Bcm, mQ/(mQ+mRod)*Bc.pos_from(Bcm))

#####################################################################################

chariot = mec.Particle(
    name="Cart", 
    mass=mA, 
    point=Bo
)
sapin = mec.Particle(
    name="Payload", 
    mass=mQ, 
    point=Bc
)

# Corps et particules
Izz_B = 1/3*mRod*(L**2)
I_rod_matrix = mec.Inertia.from_inertia_scalars(Bcm, B, 0, 0, Izz_B)
rod = mec.RigidBody(
    name="Rod",
    masscenter=Bcm,
    frame=B,
    mass=mRod,
    inertia=I_rod_matrix
)

#####################################################################################
# Équation Forces A
FA_lhs = mA*g*-N.y + 2*Fn*N.y + Ft*-B.y
FA_rhs = chariot.linear_momentum(N).dt(N)

# Équations Forces B
FB_lhs = Ft*B.y + mRod*g*-N.y + mQ*g*-N.y
FB_rhs = mec.functions.linear_momentum(N, rod, sapin).dt(N)

# Équations Moments B
MB_lhs = Bcm.pos_from(Bo).cross(-mRod*g*N.y) + Bc.pos_from(Bo).cross(-mQ*g*N.y) - const.B_FROTT*dtheta*N.z
MB_rhs = (Izz_B + mQ*(L**2))*B.ang_acc_in(N) + (mRod + mQ)*Scm.pos_from(Bo).cross(Bo.acc(N))

eqF_chariot = FA_rhs - FA_lhs
eqF_sys = FB_rhs - FB_lhs
edM_sys = MB_rhs - MB_lhs

MM, ff = sp.linear_eq_to_matrix(
    [
        eqF_chariot.dot(N.x), 
        edM_sys.dot(N.z),
        eqF_sys.dot(B.y)
    ], 
    [
        ddx, ddtheta, Ft
    ]
)

MM_num = sp.lambdify(
    (theta),
    MM.subs(cst),
    'numpy'
)

ff_num = sp.lambdify(
    (x, theta, dx, dtheta),
    ff.subs(cst),
    'numpy'
)

eq = sp.Eq(MB_lhs.dot(N.z), MB_rhs.dot(N.z))

sol1 = sp.solve(eq.subs(cst), ddtheta)
sol2 = sp.solve(eq.subs(cst), ddx)

get_ang_acc = sp.lambdify((theta, dtheta, ddx), sol1[0], modules="numpy")
get_acc = sp.lambdify((theta, dtheta, ddtheta), sol2[0], modules="numpy")
print(sol2[0])

pos_bq = (Bc.pos_from(No).dot(N.x).subs(cst), Bc.pos_from(No).dot(N.y).subs(cst))
get_pos_bq = sp.lambdify((theta, x), pos_bq, modules="numpy")
