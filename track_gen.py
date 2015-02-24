
import sys
import math
import euclid
import mesh
import geom
import track
import fbxutils

fbxManager = None
fbxScene = None
meshes = []

if __name__ == "__main__":
    sceneName = "Track"
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

    # octdata = geom.octohedron()
    # octemesh = mesh.EditableMesh("Octohedron", octdata)
    # octfbxmesh = makeMesh(fbxScene, octemesh)
    # octnode = addNode(fbxScene, octemesh.name + "_Node")
    # octnode.SetNodeAttribute(octfbxmesh)
    # make_mesh(geom.octohedron, "Octohedron")
    # make_mesh(geom.make_klein, "Klien", diffuse = (1.0, 0.0, 0.0))
    track_data = track.make_track()
    fbxutils.make_mesh(fbxManager, fbxScene, lambda:  track_data, "Track", diffuse = (0.4, 0.4, 0.4), texture = "grid.png")
    fbxutils.writeScene(fbxManager, fbxScene, sceneName + ".fbx")
    fbxManager.Destroy()
    del fbxManager

