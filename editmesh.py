import math
import pyglet
from pyglet.gl import *
from euclid import *

class EditMesh(object):
    VERTICES = 1
    EDGES = 2
    QUADS = 3

    def __init__(self):
        self.vertices = []
        self.edges = []
        self.quads = []
        self.collections = {}
        self.current_collection = None
        self.collecting = None

    def draw(self, shader):
        return

    def add_vertex(self, point):
        assert(len(point) == 3)
        point = ( round(point[0],4), round(point[1],4), round(point[2],4) )
        if point in self.vertices:
            result = self.vertices.index(point)
        else:
            self.vertices.append(point)
            result = len(self.vertices) - 1
        return result

    def add_edge(self, v0, v1):
        assert(v0 < len(self.vertices))
        assert(v1 < len(self.vertices))
        edge = (v0, v1)
        if edge in self.edges:
            result = self.edges.index(edge)
        else:
            self.edges.append(edge)
            result = len(self.edges)-1
        return result

    def add_quad(self, e0, e1, e2, e3):
        assert(self.vertices[e0[1]] == self.vertices[e1[0]])
        assert(self.vertices[e1[1]] == self.vertices[e2[0]])
        assert(self.vertices[e2[1]] == self.vertices[e3[0]])
        assert(self.vertices[e3[1]] == self.vertices[e0[0]])
        quad = (e0, e1, e2, e3)
        if quad in self.quads:
            result = self.quads.index(quad)
        else:
            self.quads.append(quad)
            result = len(self.quads)-1
        return result


    def update(self):
        return
