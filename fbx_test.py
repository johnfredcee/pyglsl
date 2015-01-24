
import sys
import math
import euclid
import mesh
import geom
import track

fbxManager = None
fbxScene = None
meshes = []

###############################################################
# Helper Function(s).                                         #
###############################################################

def makeMaterial( pScene, materialName, **kwargs ):
    lMaterialName = materialName
    lShadingName  = "Phong"
    lBlack = fbx.FbxDouble3(0.0, 0.0, 0.0)
    lRed = fbx.FbxDouble3(1.0, 0.0, 0.0)
    if kwargs.has_key("diffuse"):
        diffuse = kwargs["diffuse"]
        diffuse = fbx.FbxDouble3(diffuse[0], diffuse[1], diffuse[2])
    else:
        diffuse = lRed
    if kwargs.has_key("ambient"):
        ambient = kwargs["ambient"]
        ambient = fbx.FbxDouble3(ambient[0], ambient[1], ambient[2])
    else:
        ambient = lRed
    if kwargs.has_key("specular"):
        specular = kwargs["specular"]
        specular = fbx.FbxDouble3(specular[0], specular[1], specular[2])
    else:
        specular = lBlack
    if kwargs.has_key("emissive"):
        emissive = kwargs["emissive"]
        emissive = fbx.FbxDouble3(emissive[0], emissive[1], emissive[2])
    else:
        emissive = lBlack
    if kwargs.has_key("transparency"):
        transpaency = kwargs["transparency"]
    else:
        transparency = 0.5
    if kwargs.has_key("shininess"):
        shininess = kwargs["shininess"]
    else:
        shininess = 0.5
    if kwargs.has_key("specularity"):
        specularity = kwargs["specularity"]
    else:
        specularity = 0.3
    lMaterial = fbx.FbxSurfacePhong.Create(fbxManager, lMaterialName)
    # Generate primary and secondary colors.
    lMaterial.Emissive.Set(emissive)
    lMaterial.Ambient.Set(ambient)
    lMaterial.AmbientFactor.Set(1.)
    # Add texture for diffuse channel
    lMaterial.Diffuse.Set(diffuse)
    lMaterial.DiffuseFactor.Set(1.)
    lMaterial.TransparencyFactor.Set(transparency)
    lMaterial.ShadingModel.Set(lShadingName)
    lMaterial.Shininess.Set(shininess)
    lMaterial.Specular.Set(specular)
    lMaterial.SpecularFactor.Set(specularity)
    return lMaterial

def makeMesh( pScene, mesh ):
    result = fbx.FbxMesh.Create( pScene, mesh.name + "_Mesh")
    result.InitControlPoints(len(mesh.vertices))
    index = 0
    for v in mesh.vertices:
        v4  = fbx.FbxVector4(v.x, v.y, v.z, 0.0)
        result.SetControlPointAt(v4, index)
        index = index + 1
    index = 0
    for f in mesh.faces:
        result.BeginPolygon(index)
        vi0 = mesh.edges[mesh.faces[index].e0].v0
        vi1 = mesh.edges[mesh.faces[index].e1].v0
        vi2 = mesh.edges[mesh.faces[index].e2].v0
        result.AddPolygon(vi0)
        result.AddPolygon(vi1)
        result.AddPolygon(vi2)
        result.EndPolygon()
        index = index + 1

    result.GenerateNormals()
    return result

def addNode( pScene, nodeName, **kwargs ):
    # Obtain a reference to the scene's root node.
    scaling = kwargs["scaling"] if "scaling" in kwargs else (1.0, 1.0, 1.0)
    location = kwargs["location"] if "location" in kwargs else (0.0, 0.0, 0.0)
    parent = kwargs["parent"] if "parent" in kwargs else pScene.GetRootNode()
    newNode = fbx.FbxNode.Create( pScene, nodeName )
    newNode.LclScaling.Set(fbx.FbxDouble3(scaling[0], scaling[1], scaling[2]))
    newNode.LclTranslation.Set(fbx.FbxDouble3(location[0], location[1], location[2]))
    parent.AddChild( newNode )
    fbxMaterial = makeMaterial( pScene,
                                nodeName + "_Material",
                                **kwargs)
    newNode.AddMaterial(fbxMaterial)
    # Create a new node in the scene.
    return newNode
        

def getASCIIFormatIndex( pManager ):
    ''' Obtain the index of the ASCII export format. '''
    # Count the number of formats we can write to.
    numFormats = pManager.GetIOPluginRegistry().GetWriterFormatCount()

    # Set the default format to the native binary format.
    formatIndex = pManager.GetIOPluginRegistry().GetNativeWriterFormat()

    # Get the FBX format index whose corresponding description contains "ascii".
    for i in range( 0, numFormats ):

        # First check if the writer is an FBX writer.
        if pManager.GetIOPluginRegistry().WriterIsFBX( i ):

            # Obtain the description of the FBX writer.
            description = pManager.GetIOPluginRegistry().GetWriterFormatDescription( i )

            # Check if the description contains 'ascii'.
            if 'ascii' in description:
                formatIndex = i
                break

    # Return the file format.
    return formatIndex


def writeScene(pFilename, pFileFormat = -1, pEmbedMedia = False):
    lExporter = fbx.FbxExporter.Create(fbxManager, "")
    if pFileFormat < 0 or pFileFormat >= fbxManager.GetIOPluginRegistry().GetWriterFormatCount():
        pFileFormat = fbxManager.GetIOPluginRegistry().GetNativeWriterFormat()
        if not pEmbedMedia:
            lFormatCount = fbxManager.GetIOPluginRegistry().GetWriterFormatCount()
            for lFormatIndex in range(lFormatCount):
                if fbxManager.GetIOPluginRegistry().WriterIsFBX(lFormatIndex):
                    lDesc = fbxManager.GetIOPluginRegistry().GetWriterFormatDescription(lFormatIndex)
                    if "ascii" in lDesc:
                        pFileFormat = lFormatIndex
                        break
    
    if not fbxManager.GetIOSettings():
        ios = fbx.FbxIOSettings.Create(fbxManager, IOSROOT)
        fbxManager.SetIOSettings(ios)
    
    fbxManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_MATERIAL, True)
    fbxManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_TEXTURE, True)
    fbxManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_EMBEDDED, pEmbedMedia)
    fbxManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_SHAPE, True)
    fbxManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_GOBO, True)
    fbxManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_ANIMATION, True)
    fbxManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_GLOBAL_SETTINGS, True)

    if lExporter.Initialize(pFilename, pFileFormat, fbxManager.GetIOSettings()):
        lExporter.Export(fbxScene)

    lExporter.Destroy()

def saveScene( pFilename, pFbxManager, pFbxScene, pAsASCII=False ):
    ''' Save the scene using the Python FBX API '''
    exporter = fbx.FbxExporter.Create( pFbxManager, '' )

    if pAsASCII:
        #DEBUG: Initialize the FbxExporter object to export in ASCII.
        asciiFormatIndex = getASCIIFormatIndex( pFbxManager )
        isInitialized = exporter.Initialize( pFilename, asciiFormatIndex )
    else:
        isInitialized = exporter.Initialize( pFilename )

    if( isInitialized == False ):
        raise Exception( 'Exporter failed to initialize. Error returned: ' + str( exporter.GetLastErrorString() ) )

    exporter.Export( pFbxScene )

    exporter.Destroy()

def make_mesh(geomfn, name, **kwargs):
    data = geomfn()
    emesh = mesh.EditableMesh(name, data)
    fbxmesh = makeMesh(fbxScene, emesh)
    fbxnode = addNode(fbxScene, emesh.name +  "+Node", **kwargs)
    fbxnode.SetNodeAttribute(fbxmesh)
    return

if __name__ == "__main__":
    sceneName = "cubeScene"
    if (len(sys.argv) >= 2):
        sceneName = sys.argv[1]
    else:
        cylinderName = "Cylinder"
    if (len(sys.argv) >= 3):
        cubeName = sys.argv[2]
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
    i = 0
    for segment_data in track_data:
        make_mesh(lambda:  segment_data, "Segment%d" % i, diffuse = (0.4, 0.4, 0.4))
        i = i + 1
    writeScene(sceneName + ".fbx")
    fbxManager.Destroy()
    del fbxManager
