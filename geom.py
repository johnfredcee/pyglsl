
import euclid
import mesh

from math import *

def octohedron():
    """Construct an eight-sided polyhedron"""
    f = sqrt(2.0) / 2.0
    verts = [ \
			  euclid.Vector3( 0, -1,  0),
			  euclid.Vector3(-f,  0,  f),
			  euclid.Vector3( f,  0,  f),
			  euclid.Vector3( f,  0, -f),
			  euclid.Vector3(-f,  0, -f),
			  euclid.Vector3( 0,  1,  0) ]
    faces = [ \
        (0, 2, 1),
        (0, 3, 2),
        (0, 4, 3),
        (0, 1, 4),
        (5, 1, 2),
        (5, 2, 3),
        (5, 3, 4),
        (5, 4, 1) ]
    return { "verts" : verts, "faces": faces }

def surface(slices, stacks, func):
    verts = []
    for i in xrange(slices + 1):
        theta = i * pi / slices
        for j in xrange(stacks):
            phi = j * 2.0 * pi / stacks
            p = func(theta, phi)
            verts.append(p)            
    faces = []
    v = 0
    for i in xrange(slices):
        for j in xrange(stacks):
            next = (j + 1) % stacks
            faces.append((v + j, v + next, v + j + stacks))
            faces.append((v + next, v + next + stacks, v + j + stacks))
        v = v + stacks

    return (verts, faces)

def perp(u):
    """Randomly pick a reasonable perpendicular vector"""
    u_prime = u.cross(Vector3(1, 0, 0))
    if u_prime.magnitude_squared() < 0.01:
       u_prime = u.cross(v = Vector3(0, 1, 0))
    return u_prime.normalized()

def tube(u, v, func, radius):
    
    # Compute three basis vectors:
    p1 = Vector3(*func(u))
    p2 = Vector3(*func(u + 0.01))
    A = (p2 - p1).normalized()
    B = perp(A)
    C = A.cross(B).normalized()

    # Rotate the Z-plane circle appropriately:
    m = Matrix4.new_rotate_triple_axis(B, C, A)
    spoke_vector = m * Vector3(cos(2*v), sin(2*v), 0)

    # Add the spoke vector to the center to obtain the rim position:
    center = p1 + radius * spoke_vector
    return center[:]

def track(u, v, func, radius):
    # Compute three basis vectors:
    p1 = Vector3(*func(u))
    p2 = Vector3(*func(u + 0.01))
    # A = forward
    A = (p2 - p1).normalized()
    # b = up
    B = perp(A)
    # c = side
    C = A.cross(B).normalized()

    # Rotate the Z-plane vector appropriately:
    m = Matrix4.new_rotate_triple_axis(B, C, A)
    spoke_vector = m * Vector3(radius, 0.0, 0.0)

    # Add the spoke vector to the center to obtain the rim position:
    center = p1 + radius * spoke_vector
    return center[:]
    
# shape generating functions
def sphere(u, v):
    x = sin(u) * cos(v)
    y = cos(u)
    z = -sin(u) * sin(v)
    return x, y, z

def klein(u, v):
    u = u * 2
    if u < pi:
        x = 3 * cos(u) * (1 + sin(u)) + (2 * (1 - cos(u) / 2)) * cos(u) * cos(v)
        z = -8 * sin(u) - 2 * (1 - cos(u) / 2) * sin(u) * cos(v)
    else:
        x = 3 * cos(u) * (1 + sin(u)) + (2 * (1 - cos(u) / 2)) * cos(v + pi)
        z = -8 * sin(u)
    y = -2 * (1 - cos(u) / 2) * sin(v)
    return x, y, z

def spline( t, p ):
    """ Catmull-Rom
        (Ps can be numpy vectors or arrays too: colors, curves ...)
    """
        # wikipedia Catmull-Rom -> Cubic_Hermite_spline
        # 0 -> p0,  1 -> p1,  1/2 -> (- p_1 + 9 p0 + 9 p1 - p2) / 16
    # assert 0 <= t <= 1
    return (
        t*((2.0-t)*t - 1.0) * p[0]
        + (t*t*(3.0*t - 5.0) + 2) * p[1]
        + t*((4.0 - 3.0*t)*t + 1) * p[2]
        + (t-1.0)*t*t * p[3] ) / 2.0

def spline_path(t, points):
    ti = floor(t * len(points))
    t = t * len(points) - ti    
    si0 = ti - 1 if ti - 1 >= 0 else len(points) - (ti - 1)
    si1 = ti % len(points)
    si2 = (ti + 1) % len(points)
    si3 = (ti + 2) % len(points)
    x = spline(t, [ points[si0].x, points[si1].x, points[si2].x, points[si3].x ])
    y = spline(t, [ points[si0].y, points[si1].y, points[si2].y, points[si3].y ])
    z = spline(t, [ points[si0].z, points[si1].z, points[si2].z, points[si3].z ])
    return x,y,z

def granny_path(t):
    t = 2 * t
    x = -0.22 * cos(t) - 1.28 * sin(t) - 0.44 * cos(3 * t) - 0.78 * sin(3 * t)
    y = -0.1 * cos(2 * t) - 0.27 * sin(2 * t) + 0.38 * cos(4 * t) + 0.46 * sin(4 * t)
    z = 0.7 * cos(3 * t) - 0.4 * sin(3 * t)
    return x, y, z

def granny(u, v):
    return tube(u, v, granny_path, radius = 0.1)


# top level functions
def make_sphere(slices = 32, stacks = 32):
    verts, faces = surface(slices, stacks, sphere)
    print verts
    print faces
    return { "verts" : verts, "faces" : faces }

def make_klein(slices = 32, stacks = 32):
    verts, faces = surface(slices, stacks, klein)
    return { "verts" : verts, "faces" : faces }

def make_knot(slices = 32, stacks = 32):
    slices, stacks = 32, 32
    verts, faces = parametric.surface(slices, stacks, granny)
    return { "verts" : verts, "faces" : faces }

