#include "StdAfx.h"

#include <wx/treectrl.h>
#include <wx/progdlg.h>
#include <wx/kbdstate.h>

#include "FbxParse.h"
#include "SceneTree.h"
#include "ScenePanel.h"
#include "ViewerFrame.h"
#include "SceneClientData.h"

wxWindowID SCENE_TREE_ID           = wxNewId();
wxWindowID SCENE_PROPERTIES_ID     = wxNewId();
wxWindowID ID_TREE_MENU_CONVERT    = wxNewId();
wxWindowID ID_TREE_MENU_VIEW       = wxNewId();
wxWindowID ID_CREATE_SCENE         = wxNewId();
wxWindowID ID_LOAD_SCENE           = wxNewId();
wxWindowID ID_UNLOAD_SCENE         = wxNewId();
wxWindowID ID_CLEAR_SCENE          = wxNewId();

IMPLEMENT_CLASS(ScenePanel, wxPanel)

BEGIN_EVENT_TABLE(ScenePanel, wxPanel)
    EVT_TREE_ITEM_GETTOOLTIP(SCENE_TREE_ID,			ScenePanel::SceneTooltipSet)
    EVT_TREE_ITEM_ACTIVATED(SCENE_TREE_ID,			ScenePanel::ActivatedNode)
    EVT_TREE_SEL_CHANGED(SCENE_TREE_ID,				ScenePanel::SelectedNode)
    EVT_BUTTON(ID_CREATE_SCENE,						ScenePanel::OnCreateScene)
    EVT_BUTTON(ID_LOAD_SCENE,						ScenePanel::OnLoadScene)
    EVT_BUTTON(ID_UNLOAD_SCENE,						ScenePanel::OnUnloadScene)
    EVT_BUTTON(ID_CLEAR_SCENE,						ScenePanel::OnClearScene)
END_EVENT_TABLE()

using namespace aqua;


/**
 * Scene tree constructor
 * @param parent Parent window (usually main frame)
 * @param id     Id of window (usually dynamically assigned)
 * @param pos    Initial position
 * @param size   Initial size
 * @param style  Style (bog standard child window)
 * @param name   Name for identification purposes
 */
ScenePanel::ScenePanel(wxWindow* parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name) : wxPanel(parent, id, pos, size, style, name), scene_(NULL), origin_(NULL), sceneTree_(NULL), sizer_(NULL)                          {
    wxGridSizer  *buttonSizer;

    sizer_ =			new wxBoxSizer(wxVERTICAL);
    sceneTreeId_ =		wxNewId();
    sceneTree_ =		new SceneTree(this, sceneTreeId_, wxDefaultPosition, wxDefaultSize, wxTR_SINGLE  | wxTR_DEFAULT_STYLE);
    buttonPanel_ =		new wxPanel(this);
    buttonSizer  =		new wxGridSizer(2,2,5,5);
    createButton_ =		new wxButton(buttonPanel_, ID_CREATE_SCENE, "Create Scene");
	createButton_->Disable();
    loadButton_ =		new wxButton(buttonPanel_, ID_LOAD_SCENE,	"Load Scene");
	loadButton_->Disable();
    unloadButton_ =		new wxButton(buttonPanel_, ID_UNLOAD_SCENE, "Unload Scene");
	unloadButton_->Disable();
    clearButton_  =		new wxButton(buttonPanel_, ID_CLEAR_SCENE,	"Clear Scene");
	clearButton_->Disable();
    buttonSizer->Add(createButton_, 1);
    buttonSizer->Add(loadButton_,   1);
    buttonSizer->Add(unloadButton_, 1);
    buttonSizer->Add(clearButton_,  1);
    buttonPanel_->SetSizer(buttonSizer);
    sizer_->Add(sceneTree_, 10, wxEXPAND);
    sizer_->Add(buttonPanel_, 1);
    SetSizer(sizer_);
    Layout();
	return;
}

/** Scene Panel Destructor */
ScenePanel::~ScenePanel()
{
    ClearScene();
    if (scene_ != 0)
        scene_->Destroy();
}

/**
 * Clear the current scene from the panel
 * @return true if done
 */
bool ScenePanel::ClearScene() {
    bool result = false;
    if (scene_ != NULL) {
        sceneTree_->Hide();
        sceneTree_->DeleteAllItems();
        if (scene_ != 0)
            scene_->Destroy();
        scene_ = NULL;
        sceneTree_->Show();
        sceneTreeId_ = wxID_ANY;
        sceneTree_->Disconnect(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, wxTreeEventHandler(ScenePanel::SceneTooltipSet),  this);
        sceneTree_->Disconnect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED,  wxTreeEventHandler(ScenePanel::ActivatedNode),    this);
        sceneTree_->Disconnect(wxEVT_COMMAND_TREE_SEL_CHANGED,     wxTreeEventHandler(ScenePanel::SelectedNode),     this);
        result = true;
    }
    return result;
}

/**
 * Given the name of an FBX file, set up the scene.
 * @param fileName Name of the file
 * @return true if successful
 */
bool ScenePanel::SetScene(const std::string& fileName)
{
    //int lFileFormat = -1;
    wxProgressDialog progress(wxT("Scene Import"), wxT("Importing Scene..."), 100, this);
    sceneTree_->Hide();
    //sceneProperties_->Freeze();
    //sceneProperties_->Clear();
    sceneTree_->DeleteAllItems();
    if (scene_ != 0)
        scene_->Destroy();
    // create the scene
    scene_ = FbxScene::Create(fbxSDKManager, fileName.c_str());
    progress.Update(1, wxT("Created Scene"));
    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(fbxSDKManager,"");
    progress.Update(2, wxT("Created Importer"));
    // Initialize the importer by providing a filename.
    bool lImportStatus = lImporter->Initialize(fileName.c_str());
    if (!lImportStatus) {
        // Destroy the importer and scene
        lImporter->Destroy();
        scene_->Destroy();
        scene_ = 0;
        return false;
    }
    // Import the scene
    progress.Pulse(wxT("Importing Scene"));
    bool lStatus = lImporter->Import(scene_);
    progress.Update(20, wxT("Reset scene conventions"));
    // get scale we want (in metres)
    FbxSystemUnit ourSystemUnit(FbxSystemUnit::m);
    // get the playbox axis system
    FbxAxisSystem ourAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
    // work out how much we need to scale by to get correct units
    //FbxGlobalSettings& settings = scene_->GetGlobalSettings();
#ifdef USE_PLAYBOX_CONVENTIONS
    if (ourAxisSystem != settings.GetAxisSystem()) {
        wxMessageDialog bum(NULL, wxT("Not exported with Y - Up"), wxT("Wrong Up Axis"), wxOK | wxICON_ERROR);
        bum.ShowModal();
        return false;
    }
    if (ourSystemUnit != settings.GetSystemUnit()) {
        wxMessageDialog bum(NULL, wxT("Not exported in meters"), wxT("Wrong Scaling"), wxOK | wxICON_ERROR);
        bum.ShowModal();
        return false;
    }
    progress.Update(40, wxT("Validating Scene to Playbox Conventions"));
#endif
    // Now traverse the scene
    progress.Pulse(wxT("Traversing Scene"));
    //
    // // set up a parser to parse this scene
    // TODO: Set layers as tree roots
    parser_ = new FbxParse(scene_);
    if (lStatus) {

        // get the root, fill with layers traverse the children
        FbxNode* root = scene_->GetRootNode();
        wxString description(wxString(root->GetName(), wxConvUTF8) + getNodeDescription(root));
        // add the root
        wxTreeItemId rootId = sceneTree_->AddRoot(description, -1, -1, new SceneTreeItemData(root));
        std::vector<std::string>  layerNames;
        parser_->layerNames(layerNames);
        std::vector<FbxDisplayLayer*> layers;
        parser_->layers(layers);
        if (!layers.empty()) {
            for(std::vector<std::string>::iterator it = layerNames.begin(); it != layerNames.end(); ++it) {
                FbxDisplayLayer* layer = layers[it - layerNames.begin()];
                bool visible = layer->Show.Get();
                if (visible) {
                    std::string layerDescription = *it + " (Layer)";
                    wxTreeItemId layerRootId = sceneTree_->AppendItem(rootId, layerDescription, -1, -1, new SceneTreeItemData(layers[it - layerNames.begin()]));
                    std::vector<FbxNode*> nodes;
                    parser_->getLayerNodes(layers[it-layerNames.begin()], nodes);
                    for(std::vector<FbxNode*>::iterator n_it = nodes.begin();
                        n_it != nodes.end();
                        ++n_it) {
                        // if my parent is the scene root, then I'm a root, and can be traversed down
                        if ((*n_it)->GetParent() == root)
                            ProcessNode(*n_it, layerRootId, false);
                    }
                }
            }
        } else {
            wxLogDebug("No layers found. Classifying by type");
            //Creating a new instance
            FbxDisplayLayer* staticFurnitureLayer = FbxDisplayLayer::Create(fbxSDKManager, "animated_furniture");
            std::string layerDescription("animated_furniture");
            wxTreeItemId layerRootId = sceneTree_->AppendItem(rootId, layerDescription, -1, -1, new SceneTreeItemData(staticFurnitureLayer));
            std::vector<FbxNode*> nodes;
            parser_->getNodes(FbxNode::ClassId, nodes);
            for(std::vector<FbxNode*>::iterator n_it = nodes.begin();
                n_it != nodes.end();
                ++n_it) {
                // if my parent is the scene root, then I'm a root, and can be traversed down
                wxLogDebug(wxString::Format("Node %s Parent %s ",
                                            (*n_it)->GetName(),
                                            (*n_it)->GetParent() == NULL ? "(Null)" : (*n_it)->GetParent()->GetName()));
                if ((*n_it)->GetParent() == root)
                    ProcessNode(*n_it, layerRootId, false, staticFurnitureLayer);

            }
        }
    }

// now apply our naming conventions
    FbxSceneRenamer renamingStrategy(scene_);
    sceneTree_->Show();
//sceneProperties_->Thaw();
//sceneProperties_->Refresh();
// wxCommandEvent sceneEvent(SceneEventType, GetId());
// SceneClientData* nodeObject = new SceneClientData(NULL);
// sceneEvent.SetClientObject(nodeObject);
// ProcessEvent(sceneEvent);
// Destroy the importer
    lImporter->Destroy();
    progress.Update(100, wxT("Done"));

    Connect(SceneEventType,       wxCommandEventHandler(ViewerFrame::OnSceneChange), NULL, ViewerFrame::getViewerFrame());
    Connect(SceneClearEventType,  wxCommandEventHandler(ViewerFrame::OnSceneClear), NULL, ViewerFrame::getViewerFrame());
    Connect(SceneLoadEventType,   wxCommandEventHandler(ViewerFrame::OnSceneLoad), NULL, ViewerFrame::getViewerFrame());
    Connect(SceneUnLoadEventType, wxCommandEventHandler(ViewerFrame::OnSceneUnload), NULL, ViewerFrame::getViewerFrame());

    sceneTree_->Connect(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, wxTreeEventHandler(ScenePanel::SceneTooltipSet), NULL, this);
    sceneTree_->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED,	wxTreeEventHandler(ScenePanel::ActivatedNode),   NULL, this);
    sceneTree_->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED,		wxTreeEventHandler(ScenePanel::SelectedNode),    NULL, this);

	clearButton_->Enable();
	createButton_->Enable();
    return true;
}

/**
 * CallBack toset the tooltip when user is viewing scene panel
 * @param evt Event details
 */
void ScenePanel::SceneTooltipSet(wxTreeEvent& evt)
{
    wxTreeItemId item = evt.GetItem();
    SceneTreeItemData *sceneData = dynamic_cast<SceneTreeItemData*>(sceneTree_->GetItemData(item));
    if (sceneData != NULL)	{
        FbxNode *pNode = sceneData->getNode();
        if (pNode != NULL) {
			FbxAMatrix localTransform(pNode->GetScene()->GetEvaluator()->GetNodeLocalTransform(pNode));

			FbxVector4 translation(localTransform.GetT());
            wxString translationText(wxString::Format(wxT("T X %4.8f Y %4.8f Z %4.8f\n"), translation[0], translation[1], translation[2]));
			
            FbxVector4 rotation(localTransform.GetR());
			wxString rotationText(wxString::Format(wxT("R X %4.8f R %4.8f R %4.8f\n"), rotation[0], rotation[1], rotation[2]));
			
            FbxVector4 scale(localTransform.GetS());
            wxString scaleText(wxString::Format(wxT("S X %4.8f S %4.8f S %4.8f"), scale[0], scale[1], scale[2]));
            evt.SetToolTip(translationText  + rotationText  + scaleText);
        }
    }
    return;
}

/**
 * Handler for event when user double clicks on an FBX node in the
 * panel
 * @param evt tree Event details
 */
void ScenePanel::ActivatedNode(wxTreeEvent& evt)
{
    wxLogDebug("Activated Node");
    wxTreeItemId item = evt.GetItem();
    SceneTreeItemData *sceneData = dynamic_cast<SceneTreeItemData*>(sceneTree_->GetItemData(item));
    if (sceneData != NULL)	{
        wxCommandEvent sceneEvent(SceneEventType, GetId());
        SceneClientData* nodeObject = new SceneClientData(parser_, sceneData->getData());
        sceneEvent.SetClientObject(nodeObject);
        ProcessEvent(sceneEvent);
    } else {
        if (item == sceneTree_->GetRootItem()) {
            // we want to plurge everthing in one huge sponk
            wxTreeItemIdValue cookie;
            wxTreeItemId childItem = sceneTree_->GetFirstChild(item, cookie);
            for(Uint32 i = 0; i < sceneTree_->GetChildrenCount(item, false); ++i) {
                SceneTreeItemData *sceneData = dynamic_cast<SceneTreeItemData*>(sceneTree_->GetItemData(childItem));
                if (sceneData != NULL)	{
                    wxCommandEvent sceneEvent(SceneEventType, GetId());
                    SceneClientData* nodeObject = new SceneClientData(parser_, sceneData->getData());
                    sceneEvent.SetClientObject(nodeObject);
                    ProcessEvent(sceneEvent);
                }
                childItem = sceneTree_->GetNextChild(item, cookie);
            }
        }
    }

    return;
}

/**
 * Handler for event when user selects an FBX node in the panel
 * @param evt tree Event details
 */
void ScenePanel::SelectedNode(wxTreeEvent& evt)
{
    wxLogDebug("Selected Node");
    wxTreeItemId item = evt.GetItem();
    SceneTreeItemData *sceneData = dynamic_cast<SceneTreeItemData*>(sceneTree_->GetItemData(item));
    if (sceneData) {
        FbxObject *pNode = sceneData->getData();
        if (pNode)
            wxLogDebug("Selected node %s ", pNode->GetName());
    }
    return;
}



/**
 * Process a candidate node for adding to the tree
 * @param pNode Name of the node
 * @param treeItem Item of the tree to which we will add the node
 * @param root true if treeItem is the root node
 * @return true if successful
 */
bool ScenePanel::ProcessNode(FbxNode* pNode, wxTreeItemId treeItem, bool root, FbxDisplayLayer* layer) {
    wxTreeItemId item = treeItem;
    int lNodeChildCount = pNode->GetChildCount();
    //FbxNodeAttribute::EAttributeType attributeType = pNode->GetNodeAttribute() != NULL ? (pNode->GetNodeAttribute()->GetAttributeType()) : FbxNodeAttribute::eNULL;
    if (!root)	{
        KString newName(pNode->GetName());
        newName.ReplaceAll(' ', '_');
        pNode->SetName(newName.Buffer());
        for (int materialIndex = 0; materialIndex < pNode->GetMaterialCount(); ++materialIndex) {
            FbxSurfaceMaterial *lMaterial	= pNode->GetMaterial(materialIndex);
            newName = KString(lMaterial->GetName());
            newName.ReplaceAll(' ', '_');
            lMaterial->SetName(newName);
        }
        wxString description(wxString(pNode->GetName(), wxConvUTF8) + getNodeDescription(pNode));
        item = sceneTree_->AppendItem(treeItem, description, -1, -1, new SceneTreeItemData(pNode));
        if (layer != NULL)
            layer->AddMember(pNode);
    }
    FbxNode *lChildNode;
    while (lNodeChildCount > 0) {
        lNodeChildCount--;
        lChildNode = pNode->GetChild(lNodeChildCount);
        ProcessNode(lChildNode, item, false, layer);
    }
    return true;
}


void ScenePanel::OnCreateScene(wxCommandEvent& evt) {
    PBUNREFERENCED_PARAMETER(evt);
    wxTreeItemId item = sceneTree_->GetRootItem();
    // we want to plurge everthing in one huge sponk
    wxTreeItemIdValue cookie;
    wxTreeItemId childItem = sceneTree_->GetFirstChild(item, cookie);
    for(Uint32 i = 0; i < sceneTree_->GetChildrenCount(item, false); ++i) {
        SceneTreeItemData *sceneData = dynamic_cast<SceneTreeItemData*>(sceneTree_->GetItemData(childItem));
        if (sceneData != NULL)	{
            wxCommandEvent sceneEvent(SceneEventType, GetId());
            SceneClientData* nodeObject = new SceneClientData(parser_, sceneData->getData());
            sceneEvent.SetClientObject(nodeObject);
            ProcessEvent(sceneEvent);
        }
        childItem = sceneTree_->GetNextChild(item, cookie);
    }
	loadButton_->Enable();
	createButton_->Disable();
	clearButton_->Enable();
}

void ScenePanel::OnLoadScene(wxCommandEvent& evt) {
    PBUNREFERENCED_PARAMETER(evt);
    wxCommandEvent sceneEvent(SceneLoadEventType, GetId());
    ProcessEvent(sceneEvent);
	unloadButton_->Enable();
	loadButton_->Disable();
	clearButton_->Disable();
}

void ScenePanel::OnUnloadScene(wxCommandEvent& evt) {
    PBUNREFERENCED_PARAMETER(evt);
    wxCommandEvent sceneEvent(SceneUnLoadEventType, GetId());
    ProcessEvent(sceneEvent);
	clearButton_->Enable();
	loadButton_->Disable();
}

void ScenePanel::OnClearScene(wxCommandEvent& evt) {
    PBUNREFERENCED_PARAMETER(evt);
    wxCommandEvent sceneEvent(SceneClearEventType, GetId());
    ProcessEvent(sceneEvent);
	clearButton_->Disable();
	loadButton_->Disable();
	createButton_->Enable();
}




