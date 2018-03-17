
import sys
import os
import math
import euclid
import mesh
import geomgen
import fbx
import argparse
parser = argparse.ArgumentParser(description="Read FBX file and dump to flatbuffer.")
parser.add_argument('--fbx', metavar='F', nargs=1)
args = parser.parse_args()

sys.path = sys.path + [os.curdir + os.sep + "fbx2018_1"]    
try:
    from FbxCommon import *
except ImportError:
    import platform
    msg = 'You need to copy the content in compatible subfolder under /lib/python<version> into your python install folder such as '
    if platform.system() == 'Windows' or platform.system() == 'Microsoft':
        msg += '"Python26/Lib/site-packages"'
    elif platform.system() == 'Linux':
        msg += '"/usr/local/lib/python2.6/site-packages"'
    elif platform.system() == 'Darwin':
        msg += '"/Library/Frameworks/Python.framework/Versions/2.6/lib/python2.6/site-packages"'        
    msg += ' folder.'
    print(msg) 
    sys.exit(1)

def DisplayMetaData(pScene):
    sceneInfo = pScene.GetSceneInfo()
    if sceneInfo:
        print("\n\n--------------------\nMeta-Data\n--------------------\n")
        print("    Title: %s" % sceneInfo.mTitle.Buffer())
        print("    Subject: %s" % sceneInfo.mSubject.Buffer())
        print("    Author: %s" % sceneInfo.mAuthor.Buffer())
        print("    Keywords: %s" % sceneInfo.mKeywords.Buffer())
        print("    Revision: %s" % sceneInfo.mRevision.Buffer())
        print("    Comment: %s" % sceneInfo.mComment.Buffer())

        thumbnail = sceneInfo.GetSceneThumbnail()
        if thumbnail:
            print("    Thumbnail:")

            if thumbnail.GetDataFormat() == FbxThumbnail.eRGB_24 :
                print("        Format: RGB")
            elif thumbnail.GetDataFormat() == FbxThumbnail.eRGBA_32:
                print("        Format: RGBA")

            if thumbnail.GetSize() == FbxThumbnail.eNOT_SET:
                print("        Size: no dimensions specified (%ld bytes)", thumbnail.GetSizeInBytes())
            elif thumbnail.GetSize() == FbxThumbnail.e64x64:
                print("        Size: 64 x 64 pixels (%ld bytes)", thumbnail.GetSizeInBytes())
            elif thumbnail.GetSize() == FbxThumbnail.e128x128:
                print("        Size: 128 x 128 pixels (%ld bytes)", thumbnail.GetSizeInBytes())



def DisplayString(pHeader, pValue="" , pSuffix=""):
    lString = pHeader
    lString += str(pValue)
    lString += pSuffix
    print(lString)

def DisplayBool(pHeader, pValue, pSuffix=""):
    lString = pHeader
    if pValue:
        lString += "true"
    else:
        lString += "false"
    lString += pSuffix
    print(lString)

def DisplayInt(pHeader, pValue, pSuffix=""):
    lString = pHeader
    lString += str(pValue)
    lString += pSuffix
    print(lString)

def DisplayDouble(pHeader, pValue, pSuffix=""):
    print("%s%f%s" % (pHeader, pValue, pSuffix))

def Display2DVector(pHeader, pValue, pSuffix=""):
    print("%s%f, %f%s" % (pHeader, pValue[0], pValue[1], pSuffix))

def Display3DVector(pHeader, pValue, pSuffix=""):
    print("%s%f, %f, %f%s" % (pHeader, pValue[0], pValue[1], pValue[2], pSuffix))

def Display4DVector(pHeader, pValue, pSuffix=""):
    print("%s%f, %f, %f, %f%s" % (pHeader, pValue[0], pValue[1], pValue[2], pValue[3], pSuffix))

def DisplayColor(pHeader, pValue, pSuffix=""):
    print("%s%f (red), %f (green), %f (blue)%s" % (pHeader, pValue.mRed, pValue.mGreen, pValue.mBlue, pSuffix))


def DisplayGlobalLightSettings(pScene):
    lGlobalLightSettings = pScene.GlobalLightSettings()
    DisplayColor("Ambient Color: ", lGlobalLightSettings.GetAmbientColor())
    DisplayBool("Fog Enabled: ", lGlobalLightSettings.GetFogEnable())
    DisplayColor("Fog Color: ", lGlobalLightSettings.GetFogColor())
    
    lFogMode = lGlobalLightSettings.GetFogMode()
    
    if(lFogMode == fbx.FbxGlobalLightSettings.eLinear):
        DisplayString("Fog Mode: Linear")
    elif(lFogMode == fbx.FbxGlobalLightSettings.eExponential):
        DisplayString("Fog Mode: Exponential")
    elif(lFogMode == fbx.FbxGlobalLightSettings.eExponentialSquareRoot):
        DisplayString("Fog Mode: Exponential Square Root")
    else:
        DisplayString("Fog Mode: UNKNOWN")
        
    DisplayDouble("Fog Density: ", lGlobalLightSettings.GetFogDensity())
    DisplayDouble("Fog Start: ", lGlobalLightSettings.GetFogStart())
    DisplayDouble("Fog End: ", lGlobalLightSettings.GetFogEnd())
        
    DisplayBool("Shadow Enabled: ", lGlobalLightSettings.GetShadowEnable())
    DisplayDouble("Fog Density: ", lGlobalLightSettings.GetShadowIntensity())
    DisplayInt("Shadow Planes Count: ", lGlobalLightSettings.GetShadowPlaneCount())
    
    DisplayString("")


def DisplayGlobalCameraSettings(pScene):
    lGlobalSettings = pScene.GetGlobalSettings()
    DisplayString("Default Camera: ", lGlobalSettings.GetDefaultCamera().Buffer())
    DisplayString("")


def DisplayGlobalTimeSettings(pGlobalSettings):
    lTimeModes = [ "Default Mode", "Cinema", "PAL", "Frames 30", 
        "NTSC Drop Frame", "Frames 50", "Frames 60",
        "Frames 100", "Frames 120", "NTSC Full Frame", 
        "Frames 30 Drop", "Frames 1000" ] 

    DisplayString("Time Mode: ", lTimeModes[pGlobalSettings.GetTimeMode()])

    lTs = pGlobalSettings.GetTimelineDefaultTimeSpan()
    lStart = lTs.GetStart()
    lEnd   = lTs.GetStop()
    DisplayString("Timeline default timespan: ")
    lTmpStr=""
    DisplayString("     Start: ", lStart.GetTimeString(lTmpStr, 10))
    DisplayString("     Stop : ", lEnd.GetTimeString(lTmpStr, 10))

    DisplayString("")

# node related 
def DisplayHierarchy(pScene):
    lRootNode = pScene.GetRootNode()

    for i in range(lRootNode.GetChildCount()):
        DisplayNodeHierarchy(lRootNode.GetChild(i), 0)

def DisplayNodeHierarchy(pNode, pDepth):
    lString = ""
    for i in range(pDepth):
        lString += "     "

    lString += pNode.GetName()

    print(lString)

    for i in range(pNode.GetChildCount()):
        DisplayNodeHierarchy(pNode.GetChild(i), pDepth + 1)


def DisplayTarget(pNode):
    if pNode.GetTarget():
        DisplayString("    Target Name: ", pNode.GetTarget().GetName())

def DisplayPivotsAndLimits(pNode):
    # Pivots
    print("    Pivot Information")

    lPivotState = pNode.GetPivotState(fbx.FbxNode.eSourcePivot)
    if lPivotState == fbx.FbxNode.ePivotActive:
        print("        Pivot State: Active")
    else:
        print("        Pivot State: Reference")

    lTmpVector = pNode.GetPreRotation(FbxNode.eSourcePivot)
    print("        Pre-Rotation: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]))

    lTmpVector = pNode.GetPostRotation(FbxNode.eSourcePivot)
    print("        Post-Rotation: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]))

    lTmpVector = pNode.GetRotationPivot(FbxNode.eSourcePivot)
    print("        Rotation Pivot: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]))

    lTmpVector = pNode.GetRotationOffset(FbxNode.eSourcePivot)
    print("        Rotation Offset: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]))

    lTmpVector = pNode.GetScalingPivot(FbxNode.eSourcePivot)
    print("        Scaling Pivot: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]))

    lTmpVector = pNode.GetScalingOffset(FbxNode.eSourcePivot)
    print("        Scaling Offset: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]))

    print("    Limits Information")

    lIsActive = pNode.TranslationActive
    lMinXActive = pNode.TranslationMinX
    lMinYActive = pNode.TranslationMinY
    lMinZActive = pNode.TranslationMinZ
    lMaxXActive = pNode.TranslationMaxX
    lMaxYActive = pNode.TranslationMaxY
    lMaxZActive = pNode.TranslationMaxZ
    lMinValues = pNode.TranslationMin
    lMaxValues = pNode.TranslationMax

    if lIsActive:
        print("        Translation limits: Active")
    else:
        print("        Translation limits: Inactive")
    print("            X")
    if lMinXActive:
        print("                Min Limit: Active")
    else:
        print("                Min Limit: Inactive")
    print("                Min Limit Value: %f" % lMinValues.Get()[0])
    if lMaxXActive:
        print("                Max Limit: Active")
    else:
        print("                Max Limit: Inactive")
    print("                Max Limit Value: %f" % lMaxValues.Get()[0])
    
    print("            Y")
    if lMinYActive:
        print("                Min Limit: Active")
    else:
        print("                Min Limit: Inactive")
    print("                Min Limit Value: %f" % lMinValues.Get()[1])
    if lMaxYActive:
        print("                Max Limit: Active")
    else:
        print("                Max Limit: Inactive")
    print("                Max Limit Value: %f" % lMaxValues.Get()[1])
    
    print("            Z")
    if lMinZActive:
        print("                Min Limit: Active")
    else:
        print("                Min Limit: Inactive")
    print("                Min Limit Value: %f"% lMinValues.Get()[2])
    if lMaxZActive:
        print("                Max Limit: Active")
    else:
        print("                Max Limit: Inactive")
    print("                Max Limit Value: %f" % lMaxValues.Get()[2])

    lIsActive = pNode.RotationActive
    lMinXActive = pNode.RotationMinX
    lMinYActive = pNode.RotationMinY
    lMinZActive = pNode.RotationMinZ
    lMaxXActive = pNode.RotationMaxX
    lMaxYActive = pNode.RotationMaxY
    lMaxZActive = pNode.RotationMaxZ
    lMinValues = pNode.RotationMin
    lMaxValues = pNode.RotationMax

    if lIsActive:
        print("        Rotation limits: Active")
    else:
        print("        Rotation limits: Inactive")    
    print("            X")
    if lMinXActive:
        print("                Min Limit: Active")
    else:
        print("                Min Limit: Inactive")
    print("                Min Limit Value: %f" % lMinValues.Get()[0])
    if lMaxXActive:
        print("                Max Limit: Active")
    else:
        print("                Max Limit: Inactive")
    print("                Max Limit Value: %f" % lMaxValues.Get()[0])
    
    print("            Y")
    if lMinYActive:
        print("                Min Limit: Active")
    else:
        print("                Min Limit: Inactive")
    print("                Min Limit Value: %f" % lMinValues.Get()[1])
    if lMaxYActive:
        print("                Max Limit: Active")
    else:
        print("                Max Limit: Inactive")
    print("                Max Limit Value: %f" % lMaxValues.Get()[1])
    
    print("            Z")
    if lMinZActive:
        print("                Min Limit: Active")
    else:
        print("                Min Limit: Inactive")
    print("                Min Limit Value: %f"% lMinValues.Get()[2])
    if lMaxZActive:
        print("                Max Limit: Active")
    else:
        print("                Max Limit: Inactive")
    print("                Max Limit Value: %f" % lMaxValues.Get()[2])

    lIsActive = pNode.ScalingActive
    lMinXActive = pNode.ScalingMinX
    lMinYActive = pNode.ScalingMinY
    lMinZActive = pNode.ScalingMinZ
    lMaxXActive = pNode.ScalingMaxX
    lMaxYActive = pNode.ScalingMaxY
    lMaxZActive = pNode.ScalingMaxZ
    lMinValues = pNode.ScalingMin
    lMaxValues = pNode.ScalingMax

    if lIsActive:
        print("        Scaling limits: Active")
    else:
        print("        Scaling limits: Inactive")    
    print("            X")
    if lMinXActive:
        print("                Min Limit: Active")
    else:
        print("                Min Limit: Inactive")
    print("                Min Limit Value: %f" % lMinValues.Get()[0])
    if lMaxXActive:
        print("                Max Limit: Active")
    else:
        print("                Max Limit: Inactive")
    print("                Max Limit Value: %f" % lMaxValues.Get()[0])
    
    print("            Y")
    if lMinYActive:
        print("                Min Limit: Active")
    else:
        print("                Min Limit: Inactive")
    print("                Min Limit Value: %f" % lMinValues.Get()[1])
    if lMaxYActive:
        print("                Max Limit: Active")
    else:
        print("                Max Limit: Inactive")
    print("                Max Limit Value: %f" % lMaxValues.Get()[1])
    
    print("            Z")
    if lMinZActive:
        print("                Min Limit: Active")
    else:
        print("                Min Limit: Inactive")
    print("                Min Limit Value: %f"% lMinValues.Get()[2])
    if lMaxZActive:
        print("                Max Limit: Active")
    else:
        print("                Max Limit: Inactive")
    print("                Max Limit Value: %f" % lMaxValues.Get()[2])

def DisplayTransformPropagation(pNode):
    print("    Transformation Propagation")
    
    # Rotation Space
    lRotationOrder = pNode.GetRotationOrder(FbxNode.eSourcePivot)

    print("        Rotation Space:",)

    if lRotationOrder == eEulerXYZ:
        print("Euler XYZ")
    elif lRotationOrder == eEulerXZY:
        print("Euler XZY")
    elif lRotationOrder == eEulerYZX:
        print("Euler YZX")
    elif lRotationOrder == eEulerYXZ:
        print("Euler YXZ")
    elif lRotationOrder == eEulerZXY:
        print("Euler ZXY")
    elif lRotationOrder == eEulerZYX:
        print("Euler ZYX")
    elif lRotationOrder == eSphericXYZ:
        print("Spheric XYZ")
    
    # Use the Rotation space only for the limits
    # (keep using eEULER_XYZ for the rest)
    if pNode.GetUseRotationSpaceForLimitOnly(FbxNode.eSourcePivot):
        print("        Use the Rotation Space for Limit specification only: Yes")
    else:
        print("        Use the Rotation Space for Limit specification only: No")


    # Inherit Type
    lInheritType = pNode.GetTransformationInheritType()

    print("        Transformation Inheritance:",)

    if lInheritType == FbxTransform.eInheritRrSs:
        print("RrSs")
    elif lInheritType == FbxTransform.eInheritRSrs:
        print("RSrs")
    elif lInheritType == FbxTransform.eInheritRrs:
        print("Rrs")


def DisplayGeometricTransform(pNode):
    print("    Geometric Transformations")

    # Translation
    lTmpVector = pNode.GetGeometricTranslation(FbxNode.eSourcePivot)
    print("        Translation: %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]))

    # Rotation
    lTmpVector = pNode.GetGeometricRotation(FbxNode.eSourcePivot)
    print("        Rotation:    %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]))

    # Scaling
    lTmpVector = pNode.GetGeometricScaling(FbxNode.eSourcePivot)
    print("        Scaling:     %f %f %f" % (lTmpVector[0], lTmpVector[1], lTmpVector[2]))


#-- Marker
def DisplayMarker(pNode):
    lMarker = pNode.GetNodeAttribute()

    DisplayString("Marker Name: ", pNode.GetName())

    # Type
    lString = "    Marker Type: "
    if lMarker.GetType() == fbx.FbxMarker.eStandard:
        lString += "Standard"
    elif lMarker.GetType() == fbx.FbxMarker.eOptical:
         lString += "Optical"
    elif lMarker.GetType() == fbx.FbxMarker.eEffectorIK:
         lString += "IK Effector"
    elif lMarker.GetType() == fbx.FbxMarker.eEffectorFK:
         lString += "FK Effector"
    DisplayString(lString)

    # Look
    lString = "    Marker Look: "
    if lMarker.Look.Get() == fbx.FbxMarker.eCube:
        lString += "Cube"
    elif lMarker.Look.Get() == fbx.FbxMarker.eHardCross:
        lString += "Hard Cross"
    elif lMarker.Look.Get() == fbx.FbxMarker.eLightCross:
        lString += "Light Cross"
    elif lMarker.Look.Get() == fbx.FbxMarker.eSphere:
        lString += "Sphere"
    DisplayString(lString)

    # Size
    #lString = "    Size: "
    #lString += str(lMarker.Size.Get())
    DisplayDouble("    Size: ", lMarker.Size.Get())

    # Color
    c = lMarker.Color.Get()
    color = fbx.FbxColor(c[0], c[1], c[2])
    DisplayColor("    Color: ", color)

    # IKPivot
    Display3DVector("    IKPivot: ", lMarker.IKPivot.Get())

## -- mesh --
def DisplayLink(pGeometry):
    #Display cluster now
    lSkinCount=pGeometry.GetDeformerCount(fbx.FbxDeformer.eSkin)

    #lLinkCount = pGeometry.GetLinkCount()
    for i in range(lSkinCount):
        lClusterCount = pGeometry.GetDeformer(i, fbx.FbxDeformer.eSkin).GetClusterCount()
        for j in range(lClusterCount):
            DisplayInt("    Cluster ", i)

            lCluster = pGeometry.GetDeformer(i, fbx.FbxDeformer.eSkin).GetCluster(j)
            #lLink = pGeometry.GetLink(i)

            lClusterModes = [ "Normalize", "Additive", "Total1" ]

            DisplayString("    Mode: ", lClusterModes[lCluster.GetLinkMode()])

            if lCluster.GetLink():
                DisplayString("        Name: ", lCluster.GetLink().GetName())

            lString1 = "        Link Indices: "
            lString2 = "        Weight Values: "

            lIndexCount = lCluster.GetControlPointIndicesCount()
            lIndices = lCluster.GetControlPointIndices()
            lWeights = lCluster.GetControlPointWeights()

            for k in range(lIndexCount):
                lString1 += str(lIndices[k])
                lString2 += str(lWeights[k])

                if k < lIndexCount - 1:
                    lString1 += ", "
                    lString2 += ", "

            print(lString1)
            print(lString2)

            DisplayString("")

            lMatrix = fbx.FbxAMatrix()

            lMatrix = lCluster.GetTransformMatrix(lMatrix)
            Display3DVector("        Transform Translation: ", lMatrix.GetT())
            Display3DVector("        Transform Rotation: ", lMatrix.GetR())
            Display3DVector("        Transform Scaling: ", lMatrix.GetS())

            lMatrix = lCluster.GetTransformLinkMatrix(lMatrix)
            Display3DVector("        Transform Link Translation: ", lMatrix.GetT())
            Display3DVector("        Transform Link Rotation: ", lMatrix.GetR())
            Display3DVector("        Transform Link Scaling: ", lMatrix.GetS())

            if lCluster.GetAssociateModel():
                lMatrix = lCluster.GetTransformAssociateModelMatrix(lMatrix)
                DisplayString("        Associate Model: ", lCluster.GetAssociateModel().GetName())
                Display3DVector("        Associate Model Translation: ", lMatrix.GetT())
                Display3DVector("        Associate Model Rotation: ", lMatrix.GetR())
                Display3DVector("        Associate Model Scaling: ", lMatrix.GetS())

            DisplayString("")

def DisplayShape(pGeometry):
    lBlendShapeCount = pGeometry.GetDeformerCount(FbxDeformer.eBlendShape)

    for lBlendShapeIndex in range(lBlendShapeCount):
        lBlendShape = pGeometry.GetDeformer(lBlendShapeIndex, FbxDeformer.eBlendShape)   
        DisplayString("    BlendShape ", lBlendShape.GetName())
        
        lBlendShapeChannelCount = lBlendShape.GetBlendShapeChannelCount()
        for lBlendShapeChannelIndex in range(lBlendShapeChannelCount):
            lBlendShapeChannel = lBlendShape.GetBlendShapeChannel(lBlendShapeChannelIndex)
            DisplayString("    BlendShapeChannel ", lBlendShapeChannel.GetName())
            DisplayDouble("    Default Deform Value: ", lBlendShapeChannel.DeformPercent.Get())
            
            lTargetShapeCount = lBlendShapeChannel.GetTargetShapeCount()
            for lTargetShapeIndex in range(lTargetShapeCount):
                lShape = lBlendShapeChannel.GetTargetShape(lTargetShapeIndex)
                DisplayString("    TargetShape ", lShape.GetName())
                
                lControlPointsCount = lShape.GetControlPointsCount()
                lControlPoints = lShape.GetControlPoints()
                lNormals = lShape.GetLayer(0).GetNormals().GetDirectArray()
                
                for j in range(lControlPointsCount):
                    DisplayInt("        Control Point ", j)
                    Display3DVector("            Coordinates: ", lControlPoints[j])
                    
                    if lNormals.GetCount() == lControlPointsCount:
                        Display3DVector("            Normal Vector: ", lNormals.GetAt(j))
                
                DisplayString("")

def DisplayTextureInfo(pTexture, pBlendMode):
    DisplayString("            Name: \"", pTexture.GetName(), "\"")
    DisplayString("            File Name: \"", pTexture.GetFileName(), "\"")
    DisplayDouble("            Scale U: ", pTexture.GetScaleU())
    DisplayDouble("            Scale V: ", pTexture.GetScaleV())
    DisplayDouble("            Translation U: ", pTexture.GetTranslationU())
    DisplayDouble("            Translation V: ", pTexture.GetTranslationV())
    DisplayBool("            Swap UV: ", pTexture.GetSwapUV())
    DisplayDouble("            Rotation U: ", pTexture.GetRotationU())
    DisplayDouble("            Rotation V: ", pTexture.GetRotationV())
    DisplayDouble("            Rotation W: ", pTexture.GetRotationW())

    lAlphaSources = [ "None", "RGB Intensity", "Black" ]

    DisplayString("            Alpha Source: ", lAlphaSources[pTexture.GetAlphaSource()])
    DisplayDouble("            Cropping Left: ", pTexture.GetCroppingLeft())
    DisplayDouble("            Cropping Top: ", pTexture.GetCroppingTop())
    DisplayDouble("            Cropping Right: ", pTexture.GetCroppingRight())
    DisplayDouble("            Cropping Bottom: ", pTexture.GetCroppingBottom())

    lMappingTypes = [ "Null", "Planar", "Spherical", "Cylindrical", "Box", "Face", "UV", "Environment"]

    DisplayString("            Mapping Type: ", lMappingTypes[pTexture.GetMappingType()])

    if pTexture.GetMappingType() == FbxTexture.ePlanar:
        lPlanarMappingNormals = ["X", "Y", "Z" ]
        DisplayString("            Planar Mapping Normal: ", lPlanarMappingNormals[pTexture.GetPlanarMappingNormal()])

    lBlendModes   = ["Translucent", "Add", "Modulate", "Modulate2"]   
    if pBlendMode >= 0:
        DisplayString("            Blend Mode: ", lBlendModes[pBlendMode])
    DisplayDouble("            Alpha: ", pTexture.GetDefaultAlpha())

    lMaterialUses = ["Model Material", "Default Material"]

    DisplayString("            Material Use: ", lMaterialUses[pTexture.GetMaterialUse()])

    pTextureUses = ["Standard", "Shadow Map", "Light Map", "Spherical Reflexion Map", "Sphere Reflexion Map"]

    DisplayString("            Texture Use: ", pTextureUses[pTexture.GetTextureUse()])
    DisplayString("")

def FindAndDisplayTextureInfoByProperty(pProperty, pDisplayHeader, pMaterialIndex):
    if pProperty.IsValid():
        #Here we have to check if it's layeredtextures, or just textures:
        lLayeredTextureCount = pProperty.GetSrcObjectCount(fbx.FbxLayeredTexture.ClassId)
        if lLayeredTextureCount > 0:
            for j in range(lLayeredTextureCount):
                DisplayInt("    Layered Texture: ", j)
                lLayeredTexture = pProperty.GetSrcObject(fbx.FbxLayeredTexture.ClassId, j)
                lNbTextures = lLayeredTexture.GetSrcObjectCount(fbx.FbxTexture.ClassId)
                for k in range(lNbTextures):
                    lTexture = lLayeredTexture.GetSrcObject(fbx.FbxTexture.ClassId,k)
                    if lTexture:
                        if pDisplayHeader:
                            DisplayInt("    Textures connected to Material ", pMaterialIndex)
                            pDisplayHeader = False

                        # NOTE the blend mode is ALWAYS on the LayeredTexture and NOT the one on the texture.
                        # Why is that?  because one texture can be shared on different layered textures and might
                        # have different blend modes.

                        lBlendMode = lLayeredTexture.GetTextureBlendMode(k)
                        DisplayString("    Textures for ", pProperty.GetName())
                        DisplayInt("        Texture ", k)  
                        DisplayTextureInfo(lTexture, lBlendMode)
        else:
            # no layered texture simply get on the property
            lNbTextures = pProperty.GetSrcObjectCount(fbx.FbxTexture.ClassId)
            for j in range(lNbTextures):
                lTexture = pProperty.GetSrcObject(fbx.FbxTexture.ClassId,j)
                if lTexture:
                    # display connectMareial header only at the first time
                    if pDisplayHeader:
                        DisplayInt("    Textures connected to Material ", pMaterialIndex)
                        pDisplayHeader = False
                    
                    DisplayString("    Textures for ", pProperty.GetName().Buffer())
                    DisplayInt("        Texture ", j)  
                    DisplayTextureInfo(lTexture, -1)

        lNbTex = pProperty.GetSrcObjectCount(fbx.FbxTexture.ClassId)
        for lTextureIndex in range(lNbTex):
            lTexture = pProperty.GetSrcObject(fbx.FbxTexture.ClassId, lTextureIndex) 


def DisplayTexture(pGeometry):
    lNbMat = pGeometry.GetNode().GetSrcObjectCount(fbx.FbxSurfaceMaterial.ClassId)
    for lMaterialIndex in range(lNbMat):
        lMaterial = pGeometry.GetNode().GetSrcObject(fbx.FbxSurfaceMaterial.ClassId, lMaterialIndex)
        lDisplayHeader = True

        #go through all the possible textures
        if lMaterial:            
            for lTextureIndex in range(fbx.FbxLayerElement.sTypeTextureCount()):
                lProperty = lMaterial.FindProperty(fbx.FbxLayerElement.sTextureChannelNames(lTextureIndex))
                FindAndDisplayTextureInfoByProperty(lProperty, lDisplayHeader, lMaterialIndex) 

def DisplayMaterial(pGeometry):
    lMaterialCount = 0
    lNode = None
    if pGeometry:
        lNode = pGeometry.GetNode()
        if lNode:
            lMaterialCount = lNode.GetMaterialCount()

    for l in range(pGeometry.GetLayerCount()):
        leMat = pGeometry.GetLayer(l).GetMaterials()
        if leMat:
            if leMat.GetReferenceMode() == FbxLayerElement.eIndex:
                #Materials are in an undefined external table
                continue

            if lMaterialCount > 0:
                theColor = FbxColor()
                
                header = "    Materials on layer %d: " % l 
                DisplayString(header)

                for lCount in range(lMaterialCount):
                    DisplayInt("        Material ", lCount)

                    lMaterial = lNode.GetMaterial(lCount)

                    DisplayString("            Name: \"", lMaterial.GetName(), "\"") 

                    #Get the implementation to see if it's a hardware shader.
                    lImplementation = GetImplementation(lMaterial, "ImplementationHLSL")
                    lImplemenationType = "HLSL"
                    if not lImplementation:
                        lImplementation = GetImplementation(lMaterial, "ImplementationCGFX")
                        lImplemenationType = "CGFX"
                    if lImplementation:
                        #Now we have a hardware shader, let's read it
                        print("            Hardware Shader Type: %s\n" % lImplemenationType.Buffer())
                        lRootTable = lImplementation.GetRootTable()
                        lFileName = lRootTable.DescAbsoluteURL.Get()
                        lTechniqueName = lRootTable.DescTAG.Get() 


                        lTable = lImplementation.GetRootTable()
                        lEntryNum = lTable.GetEntryCount()

                        for i in range(lEntryNum):
                            lEntry = lTable.GetEntry(i)
                            lEntry.GetEntryType(True) 

                            lTest = lEntry.GetSource()
                            print("            Entry: %s\n" % lTest.Buffer())

                            if cmp( fbx.FbxPropertyEntryView.sEntryType, lEntrySrcType ) == 0:
                                lFbxProp = lMaterial.FindPropertyHierarchical(lEntry.GetSource()) 
                                if not lFbxProp.IsValid():
                                    lFbxProp = lMaterial.RootProperty.FindHierarchical(lEntry.GetSource())
                            elif cmp( FbxConstantEntryView.sEntryType, lEntrySrcType ) == 0:
                                lFbxProp = lImplementation.GetConstants().FindHierarchical(lEntry.GetSource())
                            
                            if lFbxProp.IsValid():
                                if lFbxProp.GetSrcObjectCount( FbxTexture.ClassId ) > 0:
                                    #do what you want with the texture
                                    for j in range(lFbxProp.GetSrcObjectCount(FbxTexture.ClassId)):
                                        lTex = lFbxProp.GetSrcObject(FbxTexture.ClassId,j)
                                        print("                Texture: %s\n" % lTex.GetFileName())
                                else:
                                    lFbxType = lFbxProp.GetPropertyDataType()
                                    if (lFbxType == fbx.eFbxBool):
                                        lFbxProp = fbx.FbxPropertyBool1(lFbxProp)
                                        DisplayBool("                Bool: ", lFbxProp.Get())
                                    elif (lFbxType == fbx.eFbxInt):
                                        lFbxProp = fbx.FbxPropertyInteger1(lFbxProp)
                                        DisplayInt("                Int: ", lFbxProp.Get())
                                    elif (lFbxType == fbx.eFbxEnum):
                                        lFbxProp = fbx.FbxPropertyEnum(lFbxProp)
                                        DisplayInt("                Enum: ", lFbxProp.Get())
                                    elif (lFbxType == fbx.eFbxFloat):
                                        lFbxProp = fbx.FbxPropertyFloat1(lFbxProp)
                                        DisplayDouble("                Float: ", lFbxProp.Get())
                                    elif ( lFbxType == fbx.eFbxDouble):
                                        lFbxProp = fbx.FbxPropertyDouble1(lFbxProp)
                                        DisplayDouble("                Double: ", lFbxProp.Get())
                                    elif ( lFbxType == fbx.eFbxString ):
                                        lFbxProp = fbx.FbxPropertyString(lFbxProp)
                                        DisplayString("                String: ", lFbxProp.Get())
                                    elif ( lFbxType == fbx.eFbxDouble2):
                                        lFbxProp = fbx.FbxPropertyDouble2(lFbxProp)
                                        res, lDouble2= lFbxProp.Get()
                                        lVect = []
                                        lVect[0] = lDouble2[0]
                                        lVect[1] = lDouble2[1]
                                        Display2DVector("                2D vector: ", lVect)
                                    elif ( lFbxType == fbx.eFbxDouble3):
                                        lFbxProp = fbx.FbxPropertyDouble3(lFbxProp)
                                        res, lDouble3 = lFbxProp.Get()
                                        lVect = []
                                        lVect[0] = lDouble3[0]
                                        lVect[1] = lDouble3[1]
                                        lVect[2] = lDouble3[2]
                                        Display3DVector("                3D vector: ", lVect)
                                    elif ( lFbxType == fbx.eFbxDouble4):
                                        lFbxProp = fbx.FbxPropertyDouble4(lFbxProp)
                                        res, lDouble4 = lFbxProp.Get()
                                        lVect = []
                                        lVect[0] = lDouble4[0]
                                        lVect[1] = lDouble4[1]
                                        lVect[2] = lDouble4[2]
                                        lVect[3] = lDouble4[3]
                                        Display4DVector("                4D vector: ", lVect)
                                    elif ( lFbxType == fbx.eFbxDouble4x4):
                                        lFbxProp = fbx.FbxPropertyXMatrix(lFbxProp)
                                        res, lDouble44 = lFbxProp.Get(EFbxType.eFbxDouble44)
                                        for j in range(4):
                                            lVect = []
                                            lVect[0] = lDouble44[j][0]
                                            lVect[1] = lDouble44[j][1]
                                            lVect[2] = lDouble44[j][2]
                                            lVect[3] = lDouble44[j][3]
                                            Display4DVector("                4x4D vector: ", lVect)

                    elif (lMaterial.GetClassId().Is(FbxSurfacePhong.ClassId)):
                        # We found a Phong material.  Display its properties.

                        # Display the Ambient Color
                        lFbxDouble3 = lMaterial.Ambient
                        theColor.Set(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2])
                        DisplayColor("            Ambient: ", theColor)

                        # Display the Diffuse Color
                        lFbxDouble3 = lMaterial.Diffuse
                        theColor.Set(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2])
                        DisplayColor("            Diffuse: ", theColor)

                        # Display the Specular Color (unique to Phong materials)
                        lFbxDouble3 = lMaterial.Specular
                        theColor.Set(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2])
                        DisplayColor("            Specular: ", theColor)

                        # Display the Emissive Color
                        lFbxDouble3 = lMaterial.Emissive
                        theColor.Set(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2])
                        DisplayColor("            Emissive: ", theColor)

                        # Opacity is Transparency factor now
                        lFbxDouble1 = lMaterial.TransparencyFactor
                        DisplayDouble("            Opacity: ", 1.0-lFbxDouble1.Get())

                        # Display the Shininess
                        lFbxDouble1 = lMaterial.Shininess
                        DisplayDouble("            Shininess: ", lFbxDouble1.Get())

                        # Display the Reflectivity
                        lFbxDouble3 = lMaterial.Reflection
                        theColor.Set(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2])
                        DisplayColor("            Reflectivity: ", theColor)
                    elif lMaterial.GetClassId().Is(FbxSurfaceLambert.ClassId):
                        # We found a Lambert material. Display its properties.
                        # Display the Ambient Color
                        lFbxDouble3 = lMaterial.Ambient
                        theColor.Set(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2])
                        DisplayColor("            Ambient: ", theColor)

                        # Display the Diffuse Color
                        lFbxDouble3 = lMaterial.Diffuse
                        theColor.Set(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2])
                        DisplayColor("            Diffuse: ", theColor)

                        # Display the Emissive
                        lFbxDouble3 = lMaterial.Emissive
                        theColor.Set(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2])
                        DisplayColor("            Emissive: ", theColor)

                        # Display the Opacity
                        lFbxDouble1 = lMaterial.TransparencyFactor
                        DisplayDouble("            Opacity: ", 1.0-lFbxDouble1.Get())
                    else:
                        DisplayString("Unknown type of Material")

                    lString = lMaterial.ShadingModel
                    DisplayString("            Shading Model: ", lString.Get().Buffer())
                    DisplayString("")


def DisplayControlsPoints(pMesh):
    lControlPointsCount = pMesh.GetControlPointsCount()
    lControlPoints = pMesh.GetControlPoints()

    DisplayString("    Control Points")

    for i in range(lControlPointsCount):
        DisplayInt("        Control Point ", i)
        Display3DVector("            Coordinates: ", lControlPoints[i])

        for j in range(pMesh.GetLayerCount()):
            leNormals = pMesh.GetLayer(j).GetNormals()
            if leNormals:
                if leNormals.GetMappingMode() == fbx.FbxLayerElement.eByControlPoint:
                    header = "            Normal Vector (on layer %d): " % j 
                    if leNormals.GetReferenceMode() == fbx.FbxLayerElement.eDirect:
                        Display3DVector(header, leNormals.GetDirectArray().GetAt(i))

    DisplayString("")


def DisplayPolygons(pMesh):
    lPolygonCount = pMesh.GetPolygonCount()
    lControlPoints = pMesh.GetControlPoints() 

    DisplayString("    Polygons")

    vertexId = 0
    for i in range(lPolygonCount):
        DisplayInt("        Polygon ", i)

        for l in range(pMesh.GetLayerCount()):
            lePolgrp = pMesh.GetLayer(l).GetPolygonGroups()
            if lePolgrp:
                if lePolgrp.GetMappingMode() == fbx.FbxLayerElement.eByPolygon:
                    if lePolgrp.GetReferenceMode() == fbx.FbxLayerElement.eIndex:
                        header = "        Assigned to group (on layer %d): " % l 
                        polyGroupId = lePolgrp.GetIndexArray().GetAt(i)
                        DisplayInt(header, polyGroupId)
                else:
                    # any other mapping modes don't make sense
                    DisplayString("        \"unsupported group assignment\"")

        lPolygonSize = pMesh.GetPolygonSize(i)

        for j in range(lPolygonSize):
            lControlPointIndex = pMesh.GetPolygonVertex(i, j)

            Display3DVector("            Coordinates: ", lControlPoints[lControlPointIndex])

            for l in range(pMesh.GetLayerCount()):
                leVtxc = pMesh.GetLayer(l).GetVertexColors()
                if leVtxc:
                    header = "            Color vertex (on layer %d): " % l 

                    if leVtxc.GetMappingMode() == fbx.FbxLayerElement.eByControlPoint:
                        if leVtxc.GetReferenceMode() == fbx.FbxLayerElement.eDirect:
                            DisplayColor(header, leVtxc.GetDirectArray().GetAt(lControlPointIndex))
                        elif leVtxc.GetReferenceMode() == fbx.FbxLayerElement.eIndexToDirect:
                                id = leVtxc.GetIndexArray().GetAt(lControlPointIndex)
                                DisplayColor(header, leVtxc.GetDirectArray().GetAt(id))
                    elif leVtxc.GetMappingMode() == fbx.FbxLayerElement.eByPolygonVertex:
                            if leVtxc.GetReferenceMode() == fbx.FbxLayerElement.eDirect:
                                DisplayColor(header, leVtxc.GetDirectArray().GetAt(vertexId))
                            elif leVtxc.GetReferenceMode() == fbx.FbxLayerElement.eIndexToDirect:
                                id = leVtxc.GetIndexArray().GetAt(vertexId)
                                DisplayColor(header, leVtxc.GetDirectArray().GetAt(id))
                    elif leVtxc.GetMappingMode() == fbx.FbxLayerElement.eByPolygon or \
                         leVtxc.GetMappingMode() ==  fbx.FbxLayerElement.eAllSame or \
                         leVtxc.GetMappingMode() ==  fbx.FbxLayerElement.eNone:       
                         # doesn't make much sense for UVs
                        pass

                leUV = pMesh.GetLayer(l).GetUVs()
                if leUV:
                    header = "            Texture UV (on layer %d): " % l 

                    if leUV.GetMappingMode() == fbx.FbxLayerElement.eByControlPoint:
                        if leUV.GetReferenceMode() == fbx.FbxLayerElement.eDirect:
                            Display2DVector(header, leUV.GetDirectArray().GetAt(lControlPointIndex))
                        elif leUV.GetReferenceMode() == fbx.FbxLayerElement.eIndexToDirect:
                            id = leUV.GetIndexArray().GetAt(lControlPointIndex)
                            Display2DVector(header, leUV.GetDirectArray().GetAt(id))
                    elif leUV.GetMappingMode() ==  fbx.FbxLayerElement.eByPolygonVertex:
                        lTextureUVIndex = pMesh.GetTextureUVIndex(i, j)
                        if leUV.GetReferenceMode() == fbx.FbxLayerElement.eDirect or \
                           leUV.GetReferenceMode() == fbx.FbxLayerElement.eIndexToDirect:
                            Display2DVector(header, leUV.GetDirectArray().GetAt(lTextureUVIndex))
                    elif leUV.GetMappingMode() == fbx.FbxLayerElement.eByPolygon or \
                         leUV.GetMappingMode() == fbx.FbxLayerElement.eAllSame or \
                         leUV.GetMappingMode() ==  fbx.FbxLayerElement.eNone:
                         # doesn't make much sense for UVs
                        pass
            # # end for layer
            vertexId += 1
        # # end for polygonSize
    # # end for polygonCount


    # check visibility for the edges of the mesh
    for l in range(pMesh.GetLayerCount()):
        leVisibility=pMesh.GetLayer(0).GetVisibility()
        if leVisibility:
            header = "    Edge Visibilty (on layer %d): " % l
            DisplayString(header)
            # should be eByEdge
            if leVisibility.GetMappingMode() == FbxLayerElement.eByEdge:
                # should be eDirect
                for j in range(pMesh.GetMeshEdgeCount()):
                    DisplayInt("        Edge ", j)
                    DisplayBool("              Edge visibilty: ", leVisibility.GetDirectArray().GetAt(j))

    DisplayString("")

def DisplayTextureNames(pProperty):
    lTextureName = ""
    
    lLayeredTextureCount = pProperty.GetSrcObjectCount(FbxLayeredTexture.ClassId)
    if lLayeredTextureCount > 0:
        for j in range(lLayeredTextureCount):
            lLayeredTexture = pProperty.GetSrcObject(FbxLayeredTexture.ClassId, j)
            lNbTextures = lLayeredTexture.GetSrcObjectCount(FbxTexture.ClassId)
            lTextureName = " Texture "

            for k in range(lNbTextures):
                lTextureName += "\""
                lTextureName += lLayeredTexture.GetName()
                lTextureName += "\""
                lTextureName += " "
            lTextureName += "of "
            lTextureName += pProperty.GetName().Buffer()
            lTextureName += " on layer "
            lTextureName += j
        lTextureName += " |"
    else:
        #no layered texture simply get on the property
        lNbTextures = pProperty.GetSrcObjectCount(FbxTexture.ClassId)

        if lNbTextures > 0:
            lTextureName = " Texture "
            lTextureName += " "

            for j in range(lNbTextures):
                lTexture = pProperty.GetSrcObject(FbxTexture.ClassId,j)
                if lTexture:
                    lTextureName += "\""
                    lTextureName += lTexture.GetName()
                    lTextureName += "\""
                    lTextureName += " "
            lTextureName += "of "
            lTextureName += pProperty.GetName().Buffer()
            lTextureName += " |"
            
    return lTextureName

def DisplayMaterialTextureConnections(pMaterial, pMatId, l ):
    lConnectionString = "            Material " + str(pMatId) + " (on layer " + str(l) +") -- "
    #Show all the textures

    #Diffuse Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sDiffuse)
    lConnectionString += DisplayTextureNames(lProperty)

    #DiffuseFactor Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sDiffuseFactor)
    lConnectionString += DisplayTextureNames(lProperty)

    #Emissive Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sEmissive)
    lConnectionString += DisplayTextureNames(lProperty)

    #EmissiveFactor Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sEmissiveFactor)
    lConnectionString += DisplayTextureNames(lProperty)


    #Ambient Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sAmbient)
    lConnectionString += DisplayTextureNames(lProperty)

    #AmbientFactor Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sAmbientFactor)
    lConnectionString += DisplayTextureNames(lProperty)     

    #Specular Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sSpecular)
    lConnectionString += DisplayTextureNames(lProperty)

    #SpecularFactor Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sSpecularFactor)
    lConnectionString += DisplayTextureNames(lProperty)

    #Shininess Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sShininess)
    lConnectionString += DisplayTextureNames(lProperty)

    #Bump Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sBump)
    lConnectionString += DisplayTextureNames(lProperty)

    #Normal Map Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sNormalMap)
    lConnectionString += DisplayTextureNames(lProperty)

    #Transparent Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sTransparentColor)
    lConnectionString += DisplayTextureNames(lProperty)

    #TransparencyFactor Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sTransparencyFactor)
    lConnectionString += DisplayTextureNames(lProperty)

    #Reflection Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sReflection)
    lConnectionString += DisplayTextureNames(lProperty)

    #ReflectionFactor Textures
    lProperty = pMaterial.FindProperty(FbxSurfaceMaterial.sReflectionFactor)
    lConnectionString += DisplayTextureNames(lProperty)

    #if(lMaterial != NULL)
    DisplayString(lConnectionString)

def DisplayMaterialConnections(pMesh):
    lPolygonCount = pMesh.GetPolygonCount()

    DisplayString("    Polygons Material Connections")

    #check whether the material maps with only one mesh
    lIsAllSame = True
    for l in range(pMesh.GetLayerCount()):
        lLayerMaterial = pMesh.GetLayer(l).GetMaterials()
        if lLayerMaterial:
            if lLayerMaterial.GetMappingMode() == fbx.FbxLayerElement.eByPolygon:
                lIsAllSame = False
                break

    #For eAllSame mapping type, just out the material and texture mapping info once
    if lIsAllSame:
        for l in range(pMesh.GetLayerCount()):
            lLayerMaterial = pMesh.GetLayer(l).GetMaterials()
            if lLayerMaterial:
                if lLayerMaterial.GetMappingMode() == FbxLayerElement.eAllSame:
                    lMaterial = pMesh.GetNode().GetMaterial(lLayerMaterial.GetIndexArray().GetAt(0))    
                    lMatId = lLayerMaterial.GetIndexArray().GetAt(0)
                    if lMatId >=0:
                        DisplayInt("        All polygons share the same material on layer ", l)
                        DisplayMaterialTextureConnections(lMaterial, lMatId, l)
            else:
                #layer 0 has no material
                if l == 0:
                    DisplayString("        no material applied")

    #For eByPolygon mapping type, just out the material and texture mapping info once
    else:
        for i in range(lPolygonCount):
            DisplayInt("        Polygon ", i)

            for l in range(pMesh.GetLayerCount()):
                lLayerMaterial = pMesh.GetLayer(l).GetMaterials()
                if lLayerMaterial:
                    lMatId = -1
                    lMaterial = pMesh.GetNode().GetMaterial(lLayerMaterial.GetIndexArray().GetAt(i))
                    lMatId = lLayerMaterial.GetIndexArray().GetAt(i)

                    if lMatId >= 0:
                        DisplayMaterialTextureConnections(lMaterial, lMatId, l)

def DisplayMaterialMapping(pMesh):
    lMappingTypes = [ "None", "By Control Point", "By Polygon Vertex", "By Polygon", "By Edge", "All Same" ]
    lReferenceMode = [ "Direct", "Index", "Index to Direct"]

    lMtrlCount = 0
    lNode = None
    if pMesh:
        lNode = pMesh.GetNode()
        if lNode:
            lMtrlCount = lNode.GetMaterialCount()

    for l in range(pMesh.GetLayerCount()):
        leMat = pMesh.GetLayer(l).GetMaterials()
        if leMat:
            header = "    Material layer %d: " % l
            DisplayString(header)

            DisplayString("           Mapping: ", lMappingTypes[leMat.GetMappingMode()])
            DisplayString("           ReferenceMode: ", lReferenceMode[leMat.GetReferenceMode()])

            lMaterialCount = 0

            if leMat.GetReferenceMode() == FbxLayerElement.eDirect or \
                leMat.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                lMaterialCount = lMtrlCount

            if leMat.GetReferenceMode() == FbxLayerElement.eIndex or \
                leMat.GetReferenceMode() == FbxLayerElement.eIndexToDirect:
                lString = "           Indices: "

                lIndexArrayCount = leMat.GetIndexArray().GetCount() 
                for i in range(lIndexArrayCount):
                    lString += str(leMat.GetIndexArray().GetAt(i))

                    if i < lIndexArrayCount - 1:
                        lString += ", "

                DisplayString(lString)

    DisplayString("")



## -- skeleton --
def DisplaySkeleton(pNode):
    lSkeleton = pNode.GetNodeAttribute()

    DisplayString("Skeleton Name: ", pNode.GetName())

    lSkeletonTypes = [ "Root", "Limb", "Limb Node", "Effector" ]

    DisplayString("    Type: ", lSkeletonTypes[lSkeleton.GetSkeletonType()])

    if lSkeleton.GetSkeletonType() == FbxSkeleton.eLimb:
        DisplayDouble("    Limb Length: ", lSkeleton.LimbLength.Get())
    elif lSkeleton.GetSkeletonType() == FbxSkeleton.eLimbNode:
        DisplayDouble("    Limb Node Size: ", lSkeleton.Size.Get())
    elif lSkeleton.GetSkeletonType() == FbxSkeleton.eRoot:
        DisplayDouble("    Limb Root Size: ", lSkeleton.Size.Get())

    DisplayColor("    Color: ", lSkeleton.GetLimbNodeColor())

## -- light -- 
def DisplayDefaultAnimationValues(pLight):
    DisplayString("    Default Animation Values")
    c = pLight.Color.Get()
    lColor = fbx.FbxColor(c[0], c[1], c[2])
    DisplayColor("        Default Color: ", lColor)
    DisplayDouble("        Default Intensity: ", pLight.Intensity.Get())
    DisplayDouble("        Default Inner Angle: ", pLight.InnerAngle.Get())
    DisplayDouble("        Default Outer Angle: ", pLight.OuterAngle.Get())
    DisplayDouble("        Default Fog: ", pLight.Fog.Get())

def DisplayLight(pNode):
    lLight = pNode.GetNodeAttribute()

    DisplayString("Light Name: ", pNode.GetName())

    lLightTypes = [ "Point", "Directional", "Spot" ]

    #DisplayString("    Type: ", lLightTypes[lLight.LightType.Get()])
    DisplayBool("    Cast Light: ", lLight.CastLight.Get())

    if not lLight.FileName.Get().IsEmpty():
        DisplayString("    Gobo")

        DisplayString("        File Name: \"", lLight.FileName.Get().Buffer(), "\"")
        DisplayBool("        Ground Projection: ", lLight.DrawGroundProjection.Get())
        DisplayBool("        Volumetric Projection: ", lLight.DrawVolumetricLight.Get())
        DisplayBool("        Front Volumetric Projection: ", lLight.DrawFrontFacingVolumetricLight.Get())

    DisplayDefaultAnimationValues(lLight)

## -- patch --
def DisplayPatch(pNode):
    lPatch = pNode.GetNodeAttribute()

    DisplayString("Patch Name: ", pNode.GetName())

    lSurfaceModes = [ "Raw", "Low No Normals", "Low", "High No Normals", "High" ]

    DisplayString("    Surface Mode: ", lSurfaceModes[lPatch.GetSurfaceMode()])

    lControlPointsCount = lPatch.GetControlPointsCount()
    lControlPoints = lPatch.GetControlPoints()

    for i in range(lControlPointsCount):
        DisplayInt("    Control Point ", i)
        Display3DVector("        Coordinates: ", lControlPoints[i])
        DisplayDouble("        Weight: ", lControlPoints[i][3])

    lPatchTypes = [ "Bezier", "Bezier Quadric", "Cardinal", "B-Spline", "Linear" ]

    DisplayString("    Patch U Type: ", lPatchTypes[lPatch.GetPatchUType()])
    DisplayInt("    U Count: ", lPatch.GetUCount())
    DisplayString("    Patch V Type: ", lPatchTypes[lPatch.GetPatchVType()])
    DisplayInt("    V Count: ", lPatch.GetVCount())
    DisplayInt("    U Step: ", lPatch.GetUStep())
    DisplayInt("    V Step: ", lPatch.GetVStep())
    DisplayBool("    U Closed: ", lPatch.GetUClosed())
    DisplayBool("    V Closed: ", lPatch.GetVClosed())
    DisplayBool("    U Capped Top: ", lPatch.GetUCappedTop())
    DisplayBool("    U Capped Bottom: ", lPatch.GetUCappedBottom())
    DisplayBool("    V Capped Top: ", lPatch.GetVCappedTop())
    DisplayBool("    V Capped Bottom: ", lPatch.GetVCappedBottom())

    DisplayString("")

    DisplayTexture(lPatch)
    DisplayMaterial(lPatch)
    DisplayLink(lPatch)
    DisplayShape(lPatch)

## -- nurb ---
def DisplayNurb(pNode):
    lNurb = pNode.GetNodeAttribute ()

    DisplayString("Nurb Name: ", pNode.GetName())

    lSurfaceModes = [ "Raw", "Low No Normals", "Low", "High No Normals", "High" ]

    DisplayString("    Surface Mode: ", lSurfaceModes[lNurb.GetSurfaceMode()])

    lControlPointsCount = lNurb.GetControlPointsCount()
    lControlPoints = lNurb.GetControlPoints()

    for i in range(lControlPointsCount):
        DisplayInt("    Control Point ", i)
        Display3DVector("        Coordinates: ", lControlPoints[i])
        DisplayDouble("        Weight: ", lControlPoints[i][3])

    lNurbTypes = [ "Periodic", "Closed", "Open" ]

    DisplayString("    Nurb U Type: ", lNurbTypes[lNurb.GetNurbsUType()])
    DisplayInt("    U Count: ", lNurb.GetUCount())
    DisplayString("    Nurb V Type: ", lNurbTypes[lNurb.GetNurbsVType()])
    DisplayInt("    V Count: ", lNurb.GetVCount())
    DisplayInt("    U Order: ", lNurb.GetUOrder())
    DisplayInt("    V Order: ", lNurb.GetVOrder())
    DisplayInt("    U Step: ", lNurb.GetUStep())
    DisplayInt("    V Step: ", lNurb.GetVStep())

    lUKnotCount = lNurb.GetUKnotCount()
    lVKnotCount = lNurb.GetVKnotCount()
    lUMultiplicityCount = lNurb.GetUCount()
    lVMultiplicityCount = lNurb.GetVCount()
    lUKnotVector = lNurb.GetUKnotVector()
    lVKnotVector = lNurb.GetVKnotVector()
    lUMultiplicityVector = lNurb.GetUMultiplicityVector()
    lVMultiplicityVector = lNurb.GetVMultiplicityVector()

    lString = "    U Knot Vector: "

    for i in range(lUKnotCount):
        lString += str(lUKnotVector[i])

        if i < lUKnotCount - 1:
            lString += ", "

    lString += "\n"
    print(lString)

    lString = "    V Knot Vector: "

    for i in range(lVKnotCount):
        lString += str(lVKnotVector[i])

        if i < lVKnotCount - 1:
            lString += ", "

    lString += "\n"
    print(lString)

    lString = "    U Multiplicity Vector: "

    for i in range(lUMultiplicityCount):
        lString += str(lUMultiplicityVector[i])

        if i < lUMultiplicityCount - 1:
            lString += ", "

    lString += "\n"
    print(lString)

    lString = "    V Multiplicity Vector: "

    for i in range(lVMultiplicityCount):
        lString += str(lVMultiplicityVector[i])

        if i < lVMultiplicityCount - 1:
            lString += ", "

    lString += "\n"
    print(lString)

    DisplayString("")

    DisplayTexture(lNurb)
    DisplayMaterial(lNurb)
    DisplayLink(lNurb)
    DisplayShape(lNurb)

## -- node content --
def DisplayContent(pScene):
    lNode = pScene.GetRootNode()

    if lNode:
        for i in range(lNode.GetChildCount()):
            DisplayNodeContent(lNode.GetChild(i))

def DisplayNodeContent(pNode):
    if pNode.GetNodeAttribute() == None:
        print("NULL Node Attribute\n")
    else:
        lAttributeType = (pNode.GetNodeAttribute().GetAttributeType())

        if lAttributeType == FbxNodeAttribute.eMarker:
            DisplayMarker(pNode)
        elif lAttributeType == FbxNodeAttribute.eSkeleton:
            DisplaySkeleton(pNode)
        elif lAttributeType == FbxNodeAttribute.eMesh:
            DisplayMesh(pNode)
        elif lAttributeType == FbxNodeAttribute.eNurbs:
             DisplayNurb(pNode)
        elif lAttributeType == FbxNodeAttribute.ePatch:
            DisplayPatch(pNode);
        elif lAttributeType == FbxNodeAttribute.eCamera:
            DisplayCamera(pNode)
        elif lAttributeType == FbxNodeAttribute.eLight:
            DisplayLight(pNode)

    #DisplayUserProperties(pNode)
    DisplayTarget(pNode)
    DisplayPivotsAndLimits(pNode)
    DisplayTransformPropagation(pNode)
    DisplayGeometricTransform(pNode)

    for i in range(pNode.GetChildCount()):
        DisplayNodeContent(pNode.GetChild(i))

## -- pose
def DisplayPose(pScene):
    lPoseCount = pScene.GetPoseCount()

    for i in range(lPoseCount):
        lPose = pScene.GetPose(i)

        lName = lPose.GetName()
        DisplayString("Pose Name: ", lName)

        DisplayBool("    Is a bind pose: ", lPose.IsBindPose())

        DisplayInt("    Number of items in the pose: ", lPose.GetCount())

        DisplayString("","")

        for j in range(lPose.GetCount()):
            lName = lPose.GetNodeName(j).GetCurrentName()
            DisplayString("    Item name: ", lName)

            if not lPose.IsBindPose():
                # Rest pose can have local matrix
                DisplayBool("    Is local space matrix: ", lPose.IsLocalMatrix(j))

            DisplayString("    Matrix value: ","")

            lMatrixValue = ""
            for k in range(4):
                lMatrix = lPose.GetMatrix(j)
                lRow = lMatrix.GetRow(k)

                lRowValue = "%9.4f %9.4f %9.4f %9.4f\n" % (lRow[0], lRow[1], lRow[2], lRow[3])
                lMatrixValue += "        " + lRowValue

            DisplayString("", lMatrixValue)

    lPoseCount = pScene.GetCharacterPoseCount()

    for i in range(lPoseCount):
        lPose = pScene.GetCharacterPose(i)
        lCharacter = lPose.GetCharacter()

        if not lCharacter:
            break

        DisplayString("Character Pose Name: ", lCharacter.mName.Buffer())

        lNodeId = eCharacterHips

        while lCharacter.GetCharacterLink(lNodeId, lCharacterLink):
            lAnimStack = None
            if lAnimStack == None:
                lScene = lCharacterLink.mNode.GetScene()
                if lScene:
                    lAnimStack = lScene.GetMember(FBX_TYPE(fbx.FbxAnimStack), 0)

            lGlobalPosition = lCharacterLink.mNode.GetGlobalFromAnim(KTIME_ZERO, lAnimStack)

            DisplayString("    Matrix value: ","")

            lMatrixValue = ""
            for k in range(4):
                lRow = lGlobalPosition.GetRow(k)

                lRowValue = "%9.4f %9.4f %9.4f %9.4f\n" % (lRow[0], lRow[1], lRow[2], lRow[3])
                lMatrixValue += "        " + lRowValue

            DisplayString("", lMatrixValue)

            lNodeId = ECharacterNodeId(int(lNodeId) + 1)

## -- animation 
def DisplayAnimation(pScene):
     for i in range(pScene.GetSrcObjectCount(FbxCriteria.ObjectType(FbxAnimStack.ClassId))):
        lAnimStack = pScene.GetSrcObject(FbxCriteria.ObjectType(FbxAnimStack.ClassId), i)

        lOutputString = "Animation Stack Name: "
        lOutputString += lAnimStack.GetName()
        lOutputString += "\n"
        print(lOutputString)

        DisplayAnimationStack(lAnimStack, pScene.GetRootNode(), True)
        DisplayAnimationStack(lAnimStack, pScene.GetRootNode(), False)

def DisplayAnimationStack(pAnimStack, pNode, isSwitcher):
    nbAnimLayers = pAnimStack.GetSrcObjectCount(FbxCriteria.ObjectType(FbxAnimLayer.ClassId))

    lOutputString = "Animation stack contains "
    lOutputString += str(nbAnimLayers)
    lOutputString += " Animation Layer(s)"
    print(lOutputString)

    for l in range(nbAnimLayers):
        lAnimLayer = pAnimStack.GetSrcObject(FbxCriteria.ObjectType(FbxAnimLayer.ClassId), l)

        lOutputString = "AnimLayer "
        lOutputString += str(l)
        print(lOutputString)

        DisplayAnimationLayer(lAnimLayer, pNode, isSwitcher)

def DisplayAnimationLayer(pAnimLayer, pNode, isSwitcher=False):
    lOutputString = "     Node Name: "
    lOutputString += pNode.GetName()
    lOutputString += "\n"
    print(lOutputString)

    DisplayChannels(pNode, pAnimLayer, DisplayCurveKeys, DisplayListCurveKeys, isSwitcher)
    print

    for lModelCount in range(pNode.GetChildCount()):
        DisplayAnimationLayer(pAnimLayer, pNode.GetChild(lModelCount), isSwitcher)


def DisplayChannels(pNode, pAnimLayer, DisplayCurve, DisplayListCurve, isSwitcher):
    lAnimCurve = None

    KFCURVENODE_T_X = "X"
    KFCURVENODE_T_Y = "Y"
    KFCURVENODE_T_Z = "Z"

    KFCURVENODE_R_X = "X"
    KFCURVENODE_R_Y = "Y"
    KFCURVENODE_R_Z = "Z"
    KFCURVENODE_R_W = "W"

    KFCURVENODE_S_X = "X"
    KFCURVENODE_S_Y = "Y"
    KFCURVENODE_S_Z = "Z"

    # Display general curves.
    if not isSwitcher:
        lAnimCurve = pNode.LclTranslation.GetCurve(pAnimLayer, KFCURVENODE_T_X)
        if lAnimCurve:
            print("        TX")
            DisplayCurve(lAnimCurve)
        lAnimCurve = pNode.LclTranslation.GetCurve(pAnimLayer, KFCURVENODE_T_Y)
        if lAnimCurve:
            print("        TY")
            DisplayCurve(lAnimCurve)
        lAnimCurve = pNode.LclTranslation.GetCurve(pAnimLayer, KFCURVENODE_T_Z)
        if lAnimCurve:
            print("        TZ")
            DisplayCurve(lAnimCurve)

        lAnimCurve = pNode.LclRotation.GetCurve(pAnimLayer, KFCURVENODE_R_X)
        if lAnimCurve:
            print("        RX")
            DisplayCurve(lAnimCurve)
        lAnimCurve = pNode.LclRotation.GetCurve(pAnimLayer, KFCURVENODE_R_Y)
        if lAnimCurve:
            print("        RY")
            DisplayCurve(lAnimCurve)
        lAnimCurve = pNode.LclRotation.GetCurve(pAnimLayer, KFCURVENODE_R_Z)
        if lAnimCurve:
            print("        RZ")
            DisplayCurve(lAnimCurve)

        lAnimCurve = pNode.LclScaling.GetCurve(pAnimLayer, KFCURVENODE_S_X)
        if lAnimCurve:
            print("        SX")
            DisplayCurve(lAnimCurve)
        lAnimCurve = pNode.LclScaling.GetCurve(pAnimLayer, KFCURVENODE_S_Y)
        if lAnimCurve:
            print("        SY")
            DisplayCurve(lAnimCurve)
        lAnimCurve = pNode.LclScaling.GetCurve(pAnimLayer, KFCURVENODE_S_Z)
        if lAnimCurve:
            print("        SZ")
            DisplayCurve(lAnimCurve)

    # Display curves specific to a light or marker.
    lNodeAttribute = pNode.GetNodeAttribute()

    KFCURVENODE_COLOR_RED = "X"
    KFCURVENODE_COLOR_GREEN = "Y"
    KFCURVENODE_COLOR_BLUE = "Z"
    
    if lNodeAttribute:
        lAnimCurve = lNodeAttribute.Color.GetCurve(pAnimLayer, KFCURVENODE_COLOR_RED)
        if lAnimCurve:
            print("        Red")
            DisplayCurve(lAnimCurve)
        lAnimCurve = lNodeAttribute.Color.GetCurve(pAnimLayer, KFCURVENODE_COLOR_GREEN)
        if lAnimCurve:
            print("        Green")
            DisplayCurve(lAnimCurve)
        lAnimCurve = lNodeAttribute.Color.GetCurve(pAnimLayer, KFCURVENODE_COLOR_BLUE)
        if lAnimCurve:
            print("        Blue")
            DisplayCurve(lAnimCurve)

        # Display curves specific to a light.
        light = pNode.GetLight()
        if light:
            lAnimCurve = light.Intensity.GetCurve(pAnimLayer)
            if lAnimCurve:
                print("        Intensity")
                DisplayCurve(lAnimCurve)

            lAnimCurve = light.OuterAngle.GetCurve(pAnimLayer)
            if lAnimCurve:
                print("        Cone Angle")
                DisplayCurve(lAnimCurve)

            lAnimCurve = light.Fog.GetCurve(pAnimLayer)
            if lAnimCurve:
                print("        Fog")
                DisplayCurve(lAnimCurve)

        # Display curves specific to a camera.
        camera = pNode.GetCamera()
        if camera:
            lAnimCurve = camera.FieldOfView.GetCurve(pAnimLayer)
            if lAnimCurve:
                print("        Field of View")
                DisplayCurve(lAnimCurve)

            lAnimCurve = camera.FieldOfViewX.GetCurve(pAnimLayer)
            if lAnimCurve:
                print("        Field of View X")
                DisplayCurve(lAnimCurve)

            lAnimCurve = camera.FieldOfViewY.GetCurve(pAnimLayer)
            if lAnimCurve:
                print("        Field of View Y")
                DisplayCurve(lAnimCurve)

            lAnimCurve = camera.OpticalCenterX.GetCurve(pAnimLayer)
            if lAnimCurve:
                print("        Optical Center X")
                DisplayCurve(lAnimCurve)

            lAnimCurve = camera.OpticalCenterY.GetCurve(pAnimLayer)
            if lAnimCurve:
                print("        Optical Center Y")
                DisplayCurve(lAnimCurve)

            lAnimCurve = camera.Roll.GetCurve(pAnimLayer)
            if lAnimCurve:
                print("        Roll")
                DisplayCurve(lAnimCurve)

        # Display curves specific to a geometry.
        if lNodeAttribute.GetAttributeType() == fbx.FbxNodeAttribute.eMesh or \
            lNodeAttribute.GetAttributeType() == fbx.FbxNodeAttribute.eNurbs or \
            lNodeAttribute.GetAttributeType() == fbx.FbxNodeAttribute.ePatch:
            lGeometry = lNodeAttribute

            lBlendShapeDeformerCount = lGeometry.GetDeformerCount(fbx.FbxDeformer.eBlendShape)
            for lBlendShapeIndex in range(lBlendShapeDeformerCount):
                lBlendShape = lGeometry.GetDeformer(lBlendShapeIndex, fbx.FbxDeformer.eBlendShape)
                lBlendShapeChannelCount = lBlendShape.GetBlendShapeChannelCount()
                for lChannelIndex in range(lBlendShapeChannelCount):
                    lChannel = lBlendShape.GetBlendShapeChannel(lChannelIndex)
                    lChannelName = lChannel.GetName()
                    lAnimCurve = lGeometry.GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer, True)
                    if lAnimCurve:
                        print("        Shape %s" % lChannelName)
                        DisplayCurve(lAnimCurve)

    # Display curves specific to properties
    lProperty = pNode.GetFirstProperty()
    while lProperty.IsValid():
        if lProperty.GetFlag(fbx.FbxPropertyFlags.eUserDefined):
            lFbxFCurveNodeName  = lProperty.GetName()
            lCurveNode = lProperty.GetCurveNode(pAnimLayer)

            if not lCurveNode:
                lProperty = pNode.GetNextProperty(lProperty)
                continue

            lDataType = lProperty.GetPropertyDataType()
            if lDataType.GetType() == fbx.eFbxBool or lDataType.GetType() == fbx.eFbxDouble or lDataType.GetType() == fbx.eFbxFloat or lDataType.GetType() == fbx.eFbxInt:
                lMessage =  "        Property "
                lMessage += lProperty.GetName()
                if lProperty.GetLabel().GetLen() > 0:
                    lMessage += " (Label: "
                    lMessage += lProperty.GetLabel()
                    lMessage += ")"

                DisplayString(lMessage)

                for c in range(lCurveNode.GetCurveCount(0)):
                    lAnimCurve = lCurveNode.GetCurve(0, c)
                    if lAnimCurve:
                        DisplayCurve(lAnimCurve)
            elif lDataType.GetType() == fbx.eFbxDouble3 or lDataType.GetType() == fbx.eFbxDouble4 or lDataType.Is(fbx.FbxColor3DT) or lDataType.Is(fbx.FbxColor4DT):
                if lDataType.Is(fbx.FbxColor3DT) or lDataType.Is(fbx.FbxColor4DT):
                    lComponentName1 = KFCURVENODE_COLOR_RED
                    lComponentName2 = KFCURVENODE_COLOR_GREEN
                    lComponentName3 = KFCURVENODE_COLOR_BLUE                    
                else:
                    lComponentName1 = "X"
                    lComponentName2 = "Y"
                    lComponentName3 = "Z"
                
                lMessage =  "        Property "
                lMessage += lProperty.GetName()
                if lProperty.GetLabel().GetLen() > 0:
                    lMessage += " (Label: "
                    lMessage += lProperty.GetLabel()
                    lMessage += ")"
                DisplayString(lMessage)

                for c in range(lCurveNode.GetCurveCount(0)):
                    lAnimCurve = lCurveNode.GetCurve(0, c)
                    if lAnimCurve:
                        DisplayString("        Component ", lComponentName1)
                        DisplayCurve(lAnimCurve)

                for c in range(lCurveNode.GetCurveCount(1)):
                    lAnimCurve = lCurveNode.GetCurve(1, c)
                    if lAnimCurve:
                        DisplayString("        Component ", lComponentName2)
                        DisplayCurve(lAnimCurve)

                for c in range(lCurveNode.GetCurveCount(2)):
                    lAnimCurve = lCurveNode.GetCurve(2, c)
                    if lAnimCurve:
                        DisplayString("        Component ", lComponentName3)
                        DisplayCurve(lAnimCurve)
            elif lDataType.GetType() == fbx.eFbxEnum:
                lMessage =  "        Property "
                lMessage += lProperty.GetName()
                if lProperty.GetLabel().GetLen() > 0:
                    lMessage += " (Label: "
                    lMessage += lProperty.GetLabel()
                    lMessage += ")"
                DisplayString(lMessage)

                for c in range(lCurveNode.GetCurveCount(0)):
                    lAnimCurve = lCurveNode.GetCurve(0, c)
                    if lAnimCurve:
                        DisplayListCurve(lAnimCurve, lProperty)

        lProperty = pNode.GetNextProperty(lProperty)


def InterpolationFlagToIndex(flags):
    #if (flags&KFCURVE_INTERPOLATION_CONSTANT)==KFCURVE_INTERPOLATION_CONSTANT:
    #    return 1
    #if (flags&KFCURVE_INTERPOLATION_LINEAR)==KFCURVE_INTERPOLATION_LINEAR:
    #    return 2
    #if (flags&KFCURVE_INTERPOLATION_CUBIC)==KFCURVE_INTERPOLATION_CUBIC:
    #    return 3
    return 0

def ConstantmodeFlagToIndex(flags):
    #if (flags&KFCURVE_CONSTANT_STANDARD)==KFCURVE_CONSTANT_STANDARD:
    #    return 1
    #if (flags&KFCURVE_CONSTANT_NEXT)==KFCURVE_CONSTANT_NEXT:
    #    return 2
    return 0

def TangeantmodeFlagToIndex(flags):
    #if (flags&KFCURVE_TANGEANT_AUTO) == KFCURVE_TANGEANT_AUTO:
    #    return 1
    #if (flags&KFCURVE_TANGEANT_AUTO_BREAK)==KFCURVE_TANGEANT_AUTO_BREAK:
    #    return 2
    #if (flags&KFCURVE_TANGEANT_TCB) == KFCURVE_TANGEANT_TCB:
    #    return 3
    #if (flags&KFCURVE_TANGEANT_USER) == KFCURVE_TANGEANT_USER:
    #    return 4
    #if (flags&KFCURVE_GENERIC_BREAK) == KFCURVE_GENERIC_BREAK:
    #    return 5
    #if (flags&KFCURVE_TANGEANT_BREAK) ==KFCURVE_TANGEANT_BREAK:
    #    return 6
    return 0

def TangeantweightFlagToIndex(flags):
    #if (flags&KFCURVE_WEIGHTED_NONE) == KFCURVE_WEIGHTED_NONE:
    #    return 1
    #if (flags&KFCURVE_WEIGHTED_RIGHT) == KFCURVE_WEIGHTED_RIGHT:
    #    return 2
    #if (flags&KFCURVE_WEIGHTED_NEXT_LEFT) == KFCURVE_WEIGHTED_NEXT_LEFT:
    #    return 3
    return 0

def TangeantVelocityFlagToIndex(flags):
    #if (flags&KFCURVE_VELOCITY_NONE) == KFCURVE_VELOCITY_NONE:
    #    return 1
    #if (flags&KFCURVE_VELOCITY_RIGHT) == KFCURVE_VELOCITY_RIGHT:
    #    return 2
    #if (flags&KFCURVE_VELOCITY_NEXT_LEFT) == KFCURVE_VELOCITY_NEXT_LEFT:
    #    return 3
    return 0

def DisplayCurveKeys(pCurve):
    interpolation = [ "?", "constant", "linear", "cubic"]
    constantMode =  [ "?", "Standard", "Next" ]
    cubicMode =     [ "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" ]
    tangentWVMode = [ "?", "None", "Right", "Next left" ]

    lKeyCount = pCurve.KeyGetCount()

    for lCount in range(lKeyCount):
        lTimeString = ""
        lKeyValue = pCurve.KeyGetValue(lCount)
        lKeyTime  = pCurve.KeyGetTime(lCount)

        lOutputString = "            Key Time: "
        lOutputString += lKeyTime.GetTimeString(lTimeString)
        lOutputString += ".... Key Value: "
        lOutputString += str(lKeyValue)
        lOutputString += " [ "
        lOutputString += interpolation[ InterpolationFlagToIndex(pCurve.KeyGetInterpolation(lCount)) ]
        #if (pCurve.KeyGetInterpolation(lCount)&KFCURVE_INTERPOLATION_CONSTANT) == KFCURVE_INTERPOLATION_CONSTANT:
        #    lOutputString += " | "
        #    lOutputString += constantMode[ ConstantmodeFlagToIndex(pCurve.KeyGetConstantMode(lCount)) ]
        #elif (pCurve.KeyGetInterpolation(lCount)&KFCURVE_INTERPOLATION_CUBIC) == KFCURVE_INTERPOLATION_CUBIC:
        #    lOutputString += " | "
        #    lOutputString += cubicMode[ TangeantmodeFlagToIndex(pCurve.KeyGetTangeantMode(lCount)) ]
        #    lOutputString += " | "
        #    lOutputString += tangentWVMode[ TangeantweightFlagToIndex(pCurve.KeyGetTangeantWeightMode(lCount)) ]
        #    lOutputString += " | "
        #    lOutputString += tangentWVMode[ TangeantVelocityFlagToIndex(pCurve.KeyGetTangeantVelocityMode(lCount)) ]
            
        lOutputString += " ]"
        print(lOutputString)

def DisplayCurveDefault(pCurve):
    lOutputString = "            Default Value: "
    lOutputString += pCurve.GetValue()
    
    print(lOutputString)

def DisplayListCurveKeys(pCurve, pProperty):
    lKeyCount = pCurve.KeyGetCount()

    for lCount in range(lKeyCount):
        lKeyValue = static_cast<int>(pCurve.KeyGetValue(lCount))
        lKeyTime  = pCurve.KeyGetTime(lCount)

        lOutputString = "            Key Time: "
        lOutputString += lKeyTime.GetTimeString(lTimeString)
        lOutputString += ".... Key Value: "
        lOutputString += lKeyValue
        lOutputString += " ("
        lOutputString += pProperty.GetEnumValue(lKeyValue)
        lOutputString += ")"

        print(lOutputString)

def DisplayListCurveDefault(pCurve, pProperty):
    DisplayCurveDefault(pCurve)

## generic info
def DisplayGenericInfo(pScene):
    lRootNode = pScene.GetRootNode()

    for i in range(lRootNode.GetChildCount()):
        DisplayNodeGenericInfo(lRootNode.GetChild(i), 0)

    #Other objects directly connected onto the scene#
    for i in range(pScene.GetSrcObjectCount(FbxCriteria.ObjectType(fbx.FbxObject.ClassId))):
        DisplayProperties(pScene.GetSrcObject(FbxCriteria.ObjectType(fbx.FbxObject.ClassId), i))


def DisplayNodeGenericInfo(pNode, pDepth):
    lString = ""
    for i in range(pDepth):
        lString += "     "

    lString += pNode.GetName()
    lString += "\n"

    DisplayString(lString)

    #Display generic info about that Node
    DisplayProperties(pNode)
    DisplayString("")
    for i in range(pNode.GetChildCount()):
        DisplayNodeGenericInfo(pNode.GetChild(i), pDepth + 1)

def DisplayProperties(pObject):
    DisplayString("Type: %s     Name: %s" % (pObject.ClassId.GetFbxFileTypeName(), pObject.GetName()))

    # Display all the properties
    lCount = 0
    lProperty = pObject.GetFirstProperty()
    while lProperty.IsValid():
        lCount += 1
        lProperty = pObject.GetNextProperty(lProperty)

    lTitleStr = "    Property Count: "

    if lCount == 0:
        return # there are no properties to display

    DisplayInt(lTitleStr, lCount)

    i=0
    lProperty = pObject.GetFirstProperty()
    while lProperty.IsValid():
        # exclude user properties
        DisplayInt("        Property ", i)
        lString = lProperty.GetLabel()
        DisplayString("            Display Name: ", lString.Buffer())
        lString = lProperty.GetName()
        DisplayString("            Internal Name: ", lString.Buffer())
        lString = lProperty.GetPropertyDataType().GetName()
        DisplayString("            Type: ", lString)
        if lProperty.HasMinLimit():
            DisplayDouble("            Min Limit: ", lProperty.GetMinLimit())
        if lProperty.HasMaxLimit():
            DisplayDouble("            Max Limit: ", lProperty.GetMaxLimit())
        DisplayBool  ("            Is Animatable: ", lProperty.GetFlag(fbx.FbxPropertyFlags.eAnimatable))

        if lProperty.GetPropertyDataType().GetType() == fbx.eFbxBool:
            lProperty = fbx.FbxPropertyBool1(lProperty)
            DisplayBool("            Default Value: ", lProperty.Get())
        elif lProperty.GetPropertyDataType().GetType() == fbx.eFbxDouble:
            lProperty = fbx.FbxPropertyDouble1(lProperty)
            DisplayDouble("            Default Value: ",lProperty.Get())
        elif lProperty.GetPropertyDataType().GetType() == fbx.eFbxDouble4:
            lProperty = fbx.FbxPropertyDouble4(lProperty)
            lDefault = lProperty.Get()
            lBuf = "R=%f, G=%f, B=%f, A=%f" % (lDefault[0], lDefault[1], lDefault[2], lDefault[3])
            DisplayString("            Default Value: ", lBuf)
        elif lProperty.GetPropertyDataType().GetType() == fbx.eFbxInt:
            lProperty = fbx.FbxPropertyInteger1(lProperty)
            DisplayInt("            Default Value: ", lProperty.Get())
        elif lProperty.GetPropertyDataType().GetType() == fbx.eFbxDouble3:
            lProperty = fbx.FbxPropertyDouble3(lProperty)
            lDefault = lProperty.Get()
            lBuf  = "X=%f, Y=%f, Z=%f" % (lDefault[0], lDefault[1], lDefault[2])
            DisplayString("            Default Value: ", lBuf)
        #case  DTEnum:
        #    DisplayInt("            Default Value: ", lProperty.Get())
        #    break

        elif lProperty.GetPropertyDataType().GetType() == fbx.eFbxFloat:
            lProperty = fbx.FbxPropertyFloat1(lProperty)
            DisplayDouble("            Default Value: ", lProperty.Get())
        elif lProperty.GetPropertyDataType().GetType() == fbx.eFbxString:
            lProperty = fbx.FbxPropertyString(lProperty)
            lString = lProperty.Get()
            DisplayString("            Default Value: ", lString.Buffer())
        else:
            DisplayString("            Default Value: UNIDENTIFIED")
        
        i += 1
        lProperty = pObject.GetNextProperty(lProperty)
    
# Prepare the FBX SDK.
lSdkManager, lScene = InitializeSdkObjects()
# Load the scene.

# The example can take a FBX file as an argument.
print "File: %s\n" % args.fbx[0] 
lResult = LoadScene(lSdkManager, lScene, args.fbx[0])
if not lResult:
    print("\n\nAn error occurred while loading the scene...")
else :
    print("\n--------------------\nMetadata\n--------------------\n")
    DisplayMetaData(lScene)
    print("\n--------------------\nGlobal Time Settings\n--------------------\n")
    DisplayGlobalTimeSettings(lScene.GetGlobalSettings())
    print("\n---------\nHierarchy\n---------\n")
    DisplayHierarchy(lScene)
    print("\n\n----\nPose\n----\n")
    DisplayPose(lScene)
    print("\n\n---------\nAnimation\n---------\n")
    DisplayAnimation(lScene)
    #now display generic information
    print("\n\n---------\nGeneric Information\n---------\n")
    DisplayGenericInfo(lScene)

