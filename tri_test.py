
import sys
import math
import euclid
import mesh
import geomgen
import track
import fbxutils

fbxManager = None
fbxScene = None
meshes = []

def make_vertex(point):
    return euclid.Point3(point["x"], point["y"], point["z"])


if __name__ == "__main__":
    sceneName = "Tri"
    try:
        import fbx
    except ImportError:
        print "Could not import fbx"

    fbxManager = fbx.FbxManager.Create()
    # Create an IOSettings object
    ios = fbx.FbxIOSettings.Create(fbxManager, fbx.IOSROOT)
    fbxManager.SetIOSettings(ios)

    # Create the entity that will hold the scene.
    fbxScene = fbx.FbxScene.Create(fbxManager, sceneName)

    # octdata = geomgen.octohedron()
    # octemesh = mesh.EditableMesh("Octohedron", octdata)
    # octfbxmesh = makeMesh(fbxScene, octemesh)
    # octnode = addNode(fbxScene, octemesh.name + "_Node")
    # octnode.SetNodeAttribute(octfbxmesh)
    # make_mesh(geomgen.octohedron, "Octohedron")
    # make_mesh(geomgen.make_klein, "Klien", diffuse = (1.0, 0.0, 0.0))
    tripoints = [ { "x" : 1.0, "y" : 0.0, "z" : 0.0 },
                  { "x" : 0.0, "y" : 1.0, "z" : 0.0 },
                  { "x" : 0.0, "y" : 0.0, "z" : 1.0 } ]
    triuvs = [ { "u" : 1.0, "v" : 0.0 },
               { "u" : 0.0, "v" : 1.0 },
               { "u" : 0.0, "v" : 0.0 } ]

    tridata = { "verts" : [], "faces" : [], "uvs" : [], "colors" : [] }
    for i in range(0, len(tripoints)):
        tridata["verts"].append(make_vertex(tripoints[i]))
        tridata["colors"].append((128, 128, 128));
        tridata["uvs"].append((triuvs[i]["u"], triuvs[i]["v"]));
    tridata["faces"].append((0,1,2))
    fbxutils.make_mesh(fbxManager, fbxScene, lambda:  tridata, "Tri", diffuse = (0.4, 0.4, 0.4), texture = "grid.png")
    fbxutils.writeScene(fbxManager, fbxScene, sceneName + ".fbx")
    fbxManager.Destroy()
    del fbxManager
