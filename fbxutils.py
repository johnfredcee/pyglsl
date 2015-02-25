
import sys
import math
import euclid
import mesh
import geomgen
import track
import fbx

###############################################################
# Helper Function(s).                                         #
###############################################################

def makeMaterial(pManager, pScene, materialName, **kwargs ):
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
    lMaterial = fbx.FbxSurfacePhong.Create(pManager, lMaterialName)
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

def makeMesh( pScene, pMaterial, mesh, texture = None ):
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

    # generate a normal layer
    result.GenerateNormals()

    # add a layer with uvs and colors
    layer = result.GetLayer(0)
    if (not layer):
        result.CreaterLayer()
        layer = mesh.GetLayer(0)

    # color layer
    colorLayerElement = fbx.FbxLayerElementVertexColor.Create(result, "")
    colorLayerElement.SetMappingMode( fbx.FbxLayerElement.eByControlPoint )
    colorLayerElement.SetReferenceMode( fbx.FbxLayerElement.eDirect )
    for v in mesh.vertices:
        c4 = fbx.FbxColor(v.r, v.g, v.b, v.a)
        colorLayerElement.GetDirectArray().Add(c4)
    layer.SetVertexColors(colorLayerElement)
    # diffuse uv layer
    uvDiffuseLayerElement = fbx.FbxLayerElementUV.Create( result, 'diffuseUV' )
    uvDiffuseLayerElement.SetMappingMode( fbx.FbxLayerElement.eByControlPoint )
    uvDiffuseLayerElement.SetReferenceMode( fbx.FbxLayerElement.eDirect )
    for v in mesh.vertices:
        v2 = fbx.FbxVector2(v.u, v.v)
        uvDiffuseLayerElement.GetDirectArray().Add(v2)
    layer.SetUVs(uvDiffuseLayerElement, fbx.FbxLayerElement.eTextureDiffuse)
    if (texture):
        fbxTexture  = fbx.FbxFileTexture.Create(pScene, mesh.name + "_Texture")
        fbxTexture.SetFileName(texture)
        fbxTexture.SetTextureUse( fbx.FbxTexture.eStandard )
        fbxTexture.SetMappingType( fbx.FbxTexture.eUV )
        fbxTexture.SetMaterialUse( fbx.FbxFileTexture.eModelMaterial )
        fbxTexture.SetSwapUV( False )
        fbxTexture.SetTranslation( 0.0, 0.0 )
        fbxTexture.SetScale( 1.0, 1.0 )
        fbxTexture.SetRotation( 0.0, 0.0 )
        fbxTexture.Alpha.Set( 0.0 )
        pMaterial.Diffuse.ConnectSrcObject(fbxTexture)
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
    return newNode

def addMaterial( pManager, pScene, pNode, nodeName, **kwargs):
    fbxMaterial = makeMaterial( pManager, pScene,
                                nodeName + "_Material",
                                **kwargs)
    pNode.AddMaterial(fbxMaterial)
    # Create a new node in the scene.
    return fbxMaterial
        
def writeScene(pManager, pScene, pFilename, pFileFormat = -1, pEmbedMedia = False):
    lExporter = fbx.FbxExporter.Create(pManager, "")
    #print "Readers"
    numFormats = pManager.GetIOPluginRegistry().GetReaderFormatCount()
    #for i in range( 0, numFormats ):
    #    print "Format %d " % i
    #    print pManager.GetIOPluginRegistry().GetReaderFormatDescription( i )
    numFormats = pManager.GetIOPluginRegistry().GetWriterFormatCount()
    #print "Writers"
    #for i in range( 0, numFormats ):
    #   print "Format %d " % i
    #   print pManager.GetIOPluginRegistry().GetWriterFormatDescription( i )    
    if pFileFormat < 0 or pFileFormat >= pManager.GetIOPluginRegistry().GetWriterFormatCount():
        pFileFormat = pManager.GetIOPluginRegistry().GetNativeWriterFormat()
        if not pEmbedMedia:
            lFormatCount = pManager.GetIOPluginRegistry().GetWriterFormatCount()
            for lFormatIndex in range(lFormatCount):
                if pManager.GetIOPluginRegistry().WriterIsFBX(lFormatIndex):
                    lDesc = pManager.GetIOPluginRegistry().GetWriterFormatDescription(lFormatIndex)
                    if "ascii" in lDesc:
                        pFileFormat = lFormatIndex
                        break
    
    if not pManager.GetIOSettings():
        ios = fbx.FbxIOSettings.Create(pManager, IOSROOT)
        pManager.SetIOSettings(ios)
    
    pManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_MATERIAL, True)
    pManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_TEXTURE, True)
    pManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_EMBEDDED, pEmbedMedia)
    pManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_SHAPE, True)
    pManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_GOBO, True)
    pManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_ANIMATION, True)
    pManager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_GLOBAL_SETTINGS, True)
    unrealSystem = fbx.FbxAxisSystem(fbx.FbxAxisSystem.eMayaZUp)
    sourceSystem = pScene.GetGlobalSettings().GetAxisSystem()
    if sourceSystem != unrealSystem:
        unrealSystem.ConvertScene(pScene)
    if lExporter.Initialize(pFilename, pFileFormat, pManager.GetIOSettings()):
        lExporter.Export(pScene)

    lExporter.Destroy()

def make_mesh(pManager, pScene, geomgenfn, name, **kwargs):
    data = geomgenfn()
    emesh = mesh.EditableMesh(name, data)
    fbxnode = addNode(pScene, emesh.name +  "_Node", **kwargs)
    fbxmaterial = addMaterial(pManager, pScene, fbxnode, emesh.name + "_Node", **kwargs)
    fbxmesh = makeMesh(pScene, fbxmaterial, emesh, kwargs["texture"] if "texture" in kwargs else None)
    fbxnode.SetNodeAttribute(fbxmesh)
    return

