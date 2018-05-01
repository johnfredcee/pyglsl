import sys
import os
import math
import euclid
import mesh
import geomgen
import fbx
import mesh
sys.path = sys.path + [os.curdir + os.sep + "fbx2018_1"]    
from FbxCommon import *

def create_sdk_manager():
    result = FbxManager.Create()
    return result

def create_scene(manager, name = "FbxScene"):
    ios = FbxIOSettings.Create(manager, IOSROOT)
    manager.SetIOSettings(ios)
    scene = FbxScene.Create(manager, name)
    return scene

def load_scene(manager, scene, file_name):
    importer = FbxImporter.Create(manager, "")    
    result = importer.Initialize(file_name, -1, manager.GetIOSettings())
    if not result:
        return False    
    if importer.IsFBX():
        manager.GetIOSettings().SetBoolProp(EXP_FBX_MATERIAL, True)
        manager.GetIOSettings().SetBoolProp(EXP_FBX_TEXTURE, True)
        manager.GetIOSettings().SetBoolProp(EXP_FBX_EMBEDDED, True)
        manager.GetIOSettings().SetBoolProp(EXP_FBX_SHAPE, True)
        manager.GetIOSettings().SetBoolProp(EXP_FBX_GOBO, True)
        manager.GetIOSettings().SetBoolProp(EXP_FBX_ANIMATION, True)
        manager.GetIOSettings().SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, True)
    result = importer.Import(scene)
    importer.Destroy()
    return result

def walk_nodes_aux(node):
    for i in range(node.GetChildCount()):
        print node.GetChild(i).GetName()
        walk_nodes_aux(node.GetChild(i))
        
def walk_nodes(scene):
    root = scene.GetRootNode()
    walk_nodes_aux(root)

if __name__ == "__main__":
    sdk_manager = create_sdk_manager()
    scene = create_scene(sdk_manager, "Generic")
    load_scene(sdk_manager, scene, "GenericMan.fbx" )
    root_node = scene.GetRootNode()
    walk_nodes(scene)