
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
