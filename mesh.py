import euclid

class Vertex:
    def __init__(self, x, y, z):
        self.x = round(x, 4)
        self.y = round(y, 4)
        self.z = round(z, 4)
        self.v = euclid.Vector3(self.x,self.y,self.z)
        self.r = 0.0
        self.g = 0.0
        self.b = 0.0
        self.a = 0.0
        self.u = 0.0
        self.v = 0.0

    def setUV(self, u, v):
        self.u = u
        self.v = v

    def setColor(self, r, g, b, a = 0.0):
        self.r = r
        self.g = g
        self.b = b
        self.a = a

    def __init__(self, **kwargs):
        self.x = round(kwargs["vert"][0], 4)
        self.y = round(kwargs["vert"][1], 4)
        self.z = round(kwargs["vert"][2], 4)
        self.v = euclid.Vector3(self.x,self.y,self.z)
        self.r = round(kwargs["color"][0], 4)
        self.g = round(kwargs["color"][1], 4)
        self.b = round(kwargs["color"][2], 4)
        if (len(kwargs["color"]) == 3):
            self.a = 0.0
        else:
            self.a = round(kwargs["color"][3], 4)
        self.u = round(kwargs["uv"][0], 4)
        self.v = round(kwargs["uv"][1], 4)

    def __hash__(self):
        return hash((self.x, self.y, self.z, self.u, self.v))

    def __eq__(self, other):
        return other and self.x == other.x and self.y == other.y and self.z == other.z and self.u ==  other.u and self.v == other.v

    def __ne__(self, other):
        return not self.__eq__(other)


class Edge:
    def __init__(self, mesh, vi0, vi1):
        self.v0 = vi0
        self.v1 = vi1
        self.mesh = mesh

    def verts(self):
        return (self.mesh.vertices[self.v0], self.mesh.vertices[self.v1])

class Tri:
    def __init__(self, mesh, ei0, ei1, ei2):
        self.e0 = ei0
        self.e1 = ei1
        self.e2 = ei2
        self.mesh = mesh

    def verts(self):
        return (self.mesh.vertices[self.mesh.edges[self.e0][0]],
                self.mesh.vertices[self.mesh.edges[self.e1][0]],
                self.mesh.vertices[self.mesh.edges[self.e2][0]])


class EditableMesh:
    def __init__(self, name, data = None):
        self.vertices = []
        self.vertmap = {}
        self.edges = []
        self.edgemap = {}
        self.faces = []
        self.name = name
        if (data != None):
            self.addPolyFaces(data)
        
        
    def addVertex(self, v):
        if (v in self.vertmap):
            return self.vertmap[v]
        self.vertices = self.vertices + [ v ]
        self.vertmap[v] = len(self.vertices) - 1
        return len(self.vertices) - 1

    def addEdge(self, e):
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
        self.faces = self.faces + [ Tri(self, ei0, ei1, ei2) ]
        return len(self.faces) - 1

    def addTriVerts(self, vi0, vi1, vi2):
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
                assert(vi < len(p["verts"]))                
        vis = []
        vi = 0
        for v in p["verts"]:
            vuv = p["uvs"][vi] if p["uvs"] else (0.0, 0.0)
            vcolor = p["colors"][vi] if p["colors"] else (1.0, 1.0, 1.0, 0.0)
            vertex = Vertex(vert = v, color = vcolor, uv = vuv)            
            vis = vis + [ self.addVertex(vertex) ]
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
