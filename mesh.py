from pyeuclid.euclid import Vector3

class Color:
    def __init__(self, r, g, b, a = 0.):
        self.r = r
        self.g = g
        self.b = b
        self.a = a

    def __len__():
        return 4

    def __getitem__(self, key):
        return (self.r, self.g, self.b, self.a)[key]

class UV:
    def __init__(self, u, v):
        self.v = v
        self.u = u

    def __len__(self):
        return 2
    def __getitem__(self, key):
        return (u,v)[key]

class Vertex:
    def __init__(self, x, y, z):
        self.x = round(x, 4)
        self.y = round(y, 4)
        self.z = round(z, 4)
        self.v = Vector3(self.x,self.y,self.z)

    def __hash__(self):
        return hash((self.x, self.y, self.z))

    def __eq__(self, other):
        return other and self.x == other.x and self.y == other.y and self.z == other.z

    def __ne__(self, other):
        return not self.__eq__(other)


class Edge:
    def __init__(self, mesh, vi0, vi1):
        self.v0 = vi0
        self.v1 = vi1
        self.mesh = mesh

    def __len__(self):
        return 2

    def __getitem__(self, key):
        return (self.v0,self.v1)[key]

    def verts(self):
        return (self.mesh.vertices[self.v0], self.mesh.vertices[self.v1])

class Tri:
    def __init__(self, mesh, ei0, ei1, ei2):
        self.e0 = ei0
        self.e1 = ei1
        self.e2 = ei2
        self.mesh = mesh

    def __len__(self):
        return 3

    def __getitem__(self, key):
        return (self.e0,self.e1, self.e2)[key]

    def verts(self):
        return (self.mesh.vertices[self.mesh.edges[self.e0][0]],
                self.mesh.vertices[self.mesh.edges[self.e1][0]],
                self.mesh.vertices[self.mesh.edges[self.e2][0]])


class EditableMesh:
    def __init__(self, name, data = None):
        self.vertices = [] # array of vertices
        self.colors = [] # array of vertex colors
        self.uvs = [] # array of texture coordinates
        self.vertmap = {} # maps a vertex value to a vertex index
        self.edges = []
        self.edgemap = {}
        self.faces = []
        self.name = name
        if (data != None):
            self.addPolyFaces(data)

    def addVertex(self, v, uv = None, color = None):
        """ Add a vertex to the vertex list """
        if (v in self.vertmap):
            return self.vertmap[v]
        self.vertices = self.vertices + [ v ]
        self.uvs = self.uvs + [ uv ]
        self.colors = self.colors + [ color ]
        self.vertmap[v] = len(self.vertices) - 1
        return len(self.vertices) - 1

    def addEdge(self, e):
        """ Add an edge to the edge list """
        assert(e.v0 < len(self.vertices))
        assert(e.v1 < len(self.vertices))
        assert(e.v0 >= 0)
        assert(e.v1 >= 0)
        if e in self.edgemap:
            return self.edgemap[e]
        self.edges = self.edges + [ e ]
        self.edgemap[e] = len(self.edges) - 1
        return len(self.edges) - 1

    def addTri(self, ei0, ei1, ei2):
        """ Given three edges, form a triangle """
        self.faces = self.faces + [ Tri(self, ei0, ei1, ei2) ]
        return len(self.faces) - 1

    def addTriVerts(self, vi0, vi1, vi2):
        """ Given three vertex indices, form a triangle """
        assert(vi0 < len(self.vertices))
        assert(vi1 < len(self.vertices))
        assert(vi2 < len(self.vertices))
        assert(vi1 >= 0)
        assert(vi2 >= 0)
        assert(vi0 >= 0)
        ei0 = self.addEdge(Edge(self, vi0, vi1))
        ei1 = self.addEdge(Edge(self, vi1, vi2))
        ei2 = self.addEdge(Edge(self, vi2, vi0))
        self.addTri(ei0, ei1, ei2)

    def addTriFace(self, t):
        """ Add a fully formed face """
        assert(t.v0 < len(self.vertices))
        assert(t.v1 < len(self.vertices))
        assert(t.v2 < len(self.vertices))
        vi0 = self.addVertex(t.v0)
        vi1 = self.addVertex(t.v1)
        vi2 = self.addVertex(t.v2)
        ei0 = self.addEdge(Edge(self, vi0, vi1))
        ei1 = self.addEdge(Edge(self, vi1, vi2))
        ei2 = self.addEdge(Edge(self, vi2, vi0))
        self.faces = self.faces + [ Tri(self, ei0, ei1, ei2) ]
        return len(self.faces) - 1

    def addQuad(self, ei0, ei1, ei2, ei3):
        self.faces = self.faces + [ Tri(self, ei0, ei1, ei2) ]
        self.faces = self.faces + [ Tri(self, ei1, ei3, ei2) ]
        return len(self.faces) - 2

    def addQuadVerts(self, vi0, vi1, vi2, vi3):
        ei0 = self.addEdge(Edge(self, vi0, vi1))
        ei1 = self.addEdge(Edge(self, vi1, vi2))
        ei2 = self.addEdge(Edge(self, vi2, vi3))
        ei3 = self.addEdge(Edge(self, vi3, vi0))
        self.addQuad(ei0, ei1, ei2, ei3)

    def addQuadFace(self, t):
        assert(t.v0 < len(self.vertices()))
        assert(t.v1 < len(self.vertices()))
        assert(t.v2 < len(self.vertices()))
        assert(t.v3 < len(self.vertices()))
        vi0 = self.addVertex(t.v0);
        vi1 = self.addVertex(t.v1);
        vi2 = self.addVertex(t.v2);
        vi3 = self.addVertex(t.v3);
        ei0 = self.addEdge(Edge(self, vi0, vi1))
        ei1 = self.addEdge(Edge(self, vi1, vi2))
        ei2 = self.addEdge(Edge(self, vi2, vi3))
        ei3 = self.addEdge(Edge(self, vi3, vi0))
        self.faces = self.faces + [ Tri(self, ei0, ei1, ei2) ]
        self.faces = self.faces + [ Tri(self, ei1, ei3, ei2) ]
        return len(self.faces) - 2

    def addPolyVerts(self, faces):
        # first calculate centroid
        verts = map(lambda x: self.vertices[x], faces)
        centroid = Vertex(0.0, 0.0, 0.0)
        for v in verts:
            centroid = centroid + v
        centroid = centroid / len(verts)
        ci = self.addVertex(centroid)
        i = iter(range(0, len(faces)))
        j = iter(map(lambda x: x+1 % len(faces), range(0,len(faces))))
        while True:
            try:
                self.addTriVerts(i,j,ci)
            except StopIteration as e:
                break

    # expects to p to have two members
    # verts - either a list of 3 tuples
    # faces - tuples that index into 3 tuples
    def addPolyFaces(self, p):
        for face in p["faces"]:
            for vi in face:
                assert(vi < len(p["vertices"]))
        vis = []
        vi = 0
        for v in p["vertices"]:
            vuv = p["uvs"][vi] if p["uvs"] else None
            vcolor = p["colors"][vi] if p["colors"] else None
            vertex = Vertex(v[0], v[1], v[2])
            if (len(vcolor) == 4):
                vcolor = Color(vcolor[0], vcolor[1], vcolor[2], vcolor[3])
            else:
                vcolor = Color(vcolor[0], vcolor[1], vcolor[2])
            vuv = UV(vuv[0], vuv[1])
            vis = vis + [ self.addVertex(vertex, vuv, vcolor) ]
            vi = vi + 1
        for f in p["faces"]:
            assert(len(f) > 2)
            if (len(f) == 3):
                self.addTriVerts(vis[f[0]], vis[f[1]], vis[f[2]])
            if (len(f) == 4):
                self.addQuadVerts(vis[f[1]], vis[f[2]], vis[f[3]], vis[f[4]])
            if (len(f) > 4):
                polyface = map(lambda x: vis[x], p["faces"])
                self.addPolyVerts(polyface)
    def has_colors(self):
        return len([x for x in self.colors if x != None]) == len(self.vertices)
    def has_uvs(self):
        return len([x for x in self.uvs if x != None]) == len(self.vertices)

def vertex_walk(mesh):
    index = 0
    while index <  len(mesh.vertcies):
        yield mesh.vertices[index].v
        index = index + 1

def color_walk(mesh):
    index = 0
    while index <  len(mesh.vertcies):
        vertex = mesh.vertices[index];
        yield (vertex.r, vertex.g, vertex.b, vertex.a)
        index = index + 1

def uv_walk(mesh):
    index = 0
    while index <  len(mesh.vertcies):
        vertex = mesh.vertices[index];
        yield (vertex.u, vertex.v)
        index = index + 1
