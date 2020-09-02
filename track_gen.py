#
# Racetrack generation to fbx file. (C) John Connors 2016,2020
#
import sys
import os
sys.path = sys.path + [os.curdir + os.sep + "fbx2020_1"]    
sys.path = sys.path + [os.curdir + os.sep + "pyeuclid"]

import math
import euclid
import mesh
import geomgen
import track
import fbxutils
import fbx

fbxManager = None
fbxScene = None
meshes = []

if __name__ == "__main__":
    sceneName = "Track"
    fbxManager = fbx.FbxManager.Create()
    # Create an IOSettings object
    ios = fbx.FbxIOSettings.Create(fbxManager, fbx.IOSROOT)
    fbxManager.SetIOSettings(ios)

    # Create the entity that will hold the scene.
    fbxScene = fbx.FbxScene.Create(fbxManager, sceneName)

	# other things to generate..
    # octdata = geomgen.octohedron()
    # octemesh = mesh.EditableMesh("Octohedron", octdata)
    # octfbxmesh = makeMesh(fbxScene, octemesh)
    # octnode = addNode(fbxScene, octemesh.name + "_Node")
    # octnode.SetNodeAttribute(octfbxmesh)
    # make_mesh(geomgen.octohedron, "Octohedron")
    # make_mesh(geomgen.make_klein, "Klien", diffuse = (1.0, 0.0, 0.0))

    track_data = track.make_track()
    fbxutils.make_mesh(fbxManager, fbxScene, lambda: track_data, "Track", diffuse = (0.4, 0.4, 0.4), texture = "grid.png")
    fbxutils.writeScene(fbxManager, fbxScene, sceneName + ".fbx")
    fbxManager.Destroy()
    del fbxManager
