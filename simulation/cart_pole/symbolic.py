## Développer la résolution de la dynamique du cartpole et des fonctions mathématiques associées

import sympy as sp
import numpy as np
import sympy.physics.mechanics as mec
from sympy.physics.vector import vprint

from simulation.cart_pole.param import CartPoleParams

def cartpole_symbolic():
    """
    Résolution symbolique du cartpole.

    Returns
    -------
    Any, Any : 
        Les functions des équations matricielle A et b.
    """
    #####################################################################################

    # Symboles Constantes
    mA, mQ, mRod = sp.symbols('mA mQ mRod')
    g = sp.symbols('g')
    L = sp.symbols('L')
    rw = sp.symbols('rw')

    # Frottement
    b_x, b_theta = sp.symbols('b_x, b_theta')

    # Symboles forces
    # Fn, Ft = sp.symbols('Fn Ft')
    Fm = sp.symbols('Fm') # Force motrice vers l'avant (Moteur)

    # Symboles dynamiques
    x, theta = mec.dynamicsymbols('x theta')
    dx, dtheta = x.diff(), theta.diff()
    ddx, ddtheta = dx.diff(), dtheta.diff()

    #####################################################################################

    # Constantes
    cst = {
        mA: CartPoleParams.M_CHARIOT,
        mQ: CartPoleParams.M_SAPIN,
        mRod: CartPoleParams.M_ROD,

        L: CartPoleParams.L_ROD,
        rw: CartPoleParams.R_WHEEL,

        g: CartPoleParams.G,

        b_theta: CartPoleParams.B_FROTT,
        b_x: CartPoleParams.B_X,
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

    # Point attache pendule
    Ao = No.locatenew('Ao', x*N.x)
    Ao.set_vel(N, dx * N.x)

    # TODO : Est qu'on inclu un point pour le Acm?

    # Centre masse tige
    Bcm = Ao.locatenew('Bcm', 0.5*L*-B.y)
    Bcm.v2pt_theory(Ao, N, B)

    # Pour attache sapin
    Bc = Ao.locatenew('Bc', L*-B.y)
    Bc.v2pt_theory(Ao, N, B)


    # Scm = mec.Point('Scm')
    # Scm.set_pos(Bcm, mQ/(mQ+mRod)*Bc.pos_from(Bcm))

    #####################################################################################
    # Corps et particules

    # Corps A
    chariot = mec.Particle(
        name="Chariot", 
        mass=mA, 
        point=Ao
    )

    # Corps C
    sapin = mec.Particle(
        name="Sapin", 
        mass=mQ, 
        point=Bc
    )

    # Corps B
    Izz_B = 1/12*mRod*(L**2)
    I_rod_matrix = mec.Inertia.from_inertia_scalars(Bcm, B, 0, 0, Izz_B)
    rod = mec.RigidBody(
        name="Rod",
        masscenter=Bcm,
        frame=B,
        mass=mRod,
        inertia=I_rod_matrix
    )

    #####################################################################################
    # Équation 1 : Forces sur le système
    # Les forces au niveau du pivot ne nous intéresse pas allons on peut considérer tous
    #   le système (A + B + C) d'un seul coup

    F_lhs = mec.functions.linear_momentum(N, chariot, sapin, rod).dt(N)
    F_rhs = Fm*N.x - b_x*dx*N.x  # On pourrait mettre aussi les gravités en N.y mais ce nous intéresse pas tant

    eq_force_x = (F_lhs - F_rhs).dot(N.x)

    #####################################################################################
    # Équation 2 : Moment du pendule autour du point d'attache Ao

    # Moments externes sur le pendule
    M_gravity_rod = Bcm.pos_from(Ao).cross(mRod * g * -N.y)
    M_gravity_payload = Bc.pos_from(Ao).cross(mQ * g * -N.y)
    M_friction_pivot = -b_theta * dtheta * N.z

    M_lhs = M_gravity_rod + M_gravity_payload + M_friction_pivot

    # Moment dynamique autour de Ao pour la tige + la masse
    M_dyn_rod = (
        Bcm.pos_from(Ao).cross(mRod * Bcm.acc(N))
        + Izz_B * B.ang_acc_in(N)
    )
    M_dyn_payload = Bc.pos_from(Ao).cross(mQ * Bc.acc(N))

    M_rhs = M_dyn_rod + M_dyn_payload

    eq_moment_Ao = M_lhs.dot(N.z) - M_rhs.dot(N.z)

    #####################################################################################

    # Résolution symbolique
    sol = sp.solve((eq_force_x.subs(cst), eq_moment_Ao.subs(cst)), (ddx, ddtheta), simplify=True)
    ddx_expr = sp.simplify(sol[ddx])
    ddtheta_expr = sp.simplify(sol[ddtheta])
    
    state = sp.Matrix([x, dx, theta, dtheta])
    dstate = sp.Matrix([dx, ddx_expr, dtheta, ddtheta_expr])

    A_Jac = dstate.jacobian(state)
    B_Jac = dstate.jacobian([Fm])

    equilibrium = {
        x: 0,
        dx: 0,
        theta: 0,
        dtheta: 0,
        Fm: 0
    }

    A_eq = sp.simplify(A_Jac.subs(equilibrium))
    B_eq = sp.simplify(B_Jac.subs(equilibrium))

    #####################################################################################
    # Système matriciel
    #
    # A * [ddx, ddtheta]^T = b

    mat_A, mat_b = sp.linear_eq_to_matrix(
        [
            eq_force_x,
            eq_moment_Ao,
        ],
        [
            ddx,
            ddtheta,
        ],
    )

    #####################################################################################
    # Lambdify
    
    A_fn = sp.lambdify(
        (theta),
        mat_A.subs(cst),
        'numpy'
    )

    b_fn = sp.lambdify(
        (theta, dx, dtheta, Fm),
        mat_b.subs(cst),
        'numpy'
    )

    #####################################################################################
    # Return
    
    return A_fn, b_fn, np.asarray(A_eq, dtype=np.float64), np.asarray(B_eq, dtype=np.float64)