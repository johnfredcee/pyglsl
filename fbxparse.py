import sys
import os
import math
import euclid
import mesh
import geomgen
import fbx
import mesh

def create_sdk_manager():
    result = fbx.FbxManager.Create()
    return result

def create_scene(manager, name = "FbxScene"):
    ios = fbx.FbxIOSettings.Create(manager, fbx.IOSROOT)
    manager.SetIOSettings(ios)
    scene = fbx.FbxScene.Create(manager, name)
    return scene

def load_scene(manager, scene, file_name):
    importer = fbx.FbxImporter.Create(manager, "")    
    result = importer.Initialize(file_name, -1, manager.GetIOSettings())
    if not result:
        return False    

    if importer.IsFBX():
        manager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_MATERIAL, True)
        manager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_TEXTURE, True)
        manager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_EMBEDDED, True)
        manager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_SHAPE, True)
        manager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_GOBO, True)
        manager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_ANIMATION, True)
        manager.GetIOSettings().SetBoolProp(fbx.EXP_FBX_GLOBAL_SETTINGS, True)
        
    result = importer.Import(scene)
    importer.Destroy()
    return result

# Node methods
def get_geometry_transform(node):
    gt = node.GetGeometricTranslation(fbx.FbxNode.eSourcePivot)
    gr = node.GetGeometricRotation(fbx.FbxNode.eSourcePivot)
    gs = node.GetGeometricScaling(fbx.FbxNode.eSourcePivot)
    return fbx.FbxAMatrix(gt, gr, gs)

def get_global_transform(node, time = 0.0):
    t = fbx.FbxTime(time)
    return node.EvaluateGlobalTransform(t)

def get_local_transform(node, time = 0.0):
    t = fbx.FbxTime(time)
    return node.EvaluateLocalTransform(t)

# get absolute transform
def get_node_transform(node, time = 0.0):
    result = get_global_transform(node, time)
    if (node.GetNodeAttribute()):
        gt = get_geometry_transform(node)
        result = result * gt
    return result

# get local transform
def get_node_local_transform(node, time = 0.0):
    transform = get_node_transform(node, time)
    parent = node.GetParent()
    if parent:
        parent_transform = get_node_transform(parent, time)
        inverse_parent_transform = parent_transform.Inverse()
        transform = transform * inverse_parent_transform
    return transform

# got global transform with animation
def get_node_animated_transform(node, time = 0.0):
    global_transform = node.EvaluateGlobalTransform(time)
    geometry_transform = get_geometry_transform(node)
    return global_transform * geometry_transform

def walk_nodes_aux(node):
    for i in range(node.GetChildCount()):
        print node.GetChild(i).GetName()
        local_tf = get_node_local_transform(node.GetChild(i))
        for vec4 in local_tf:
            print vec4[0], vec4[1], vec4[2], vec4[3]
        walk_nodes_aux(node.GetChild(i))
        
def walk_nodes(scene):
    root = scene.GetRootNode()
    print root.GetName()
    local_tf = get_node_local_transform(root)
    for vec4 in local_tf:
        print vec4[0], vec4[1], vec4[2], vec4[3]
    walk_nodes_aux(root)

if __name__ == "__main__":
    print "Hello world"
    sdk_manager = create_sdk_manager()
    scene = create_scene(sdk_manager, "Generic")
    print load_scene(sdk_manager, scene, "GenericMan.fbx" )
    axis_system = fbx.FbxAxisSystem(fbx.FbxAxisSystem.eOpenGL)    
    axis_system.ConvertScene(scene)
    root_node = scene.GetRootNode()
    scene_units = fbx.FbxSystemUnit.cm 
    scene_units.ConvertScene(scene)
    walk_nodes(scene)