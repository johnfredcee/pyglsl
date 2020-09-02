#ifndef SCENE_PANEL_H_INCLUDED
#define SCENE_PANEL_H_INCLUDED


class wxBoxSizer;
class wxTreeCtrl;
class wxFlexGridSizer;
class wxPanel;
class wxPropertyGrid;
class wxPropertyGridEvent;
class SceneTree;

namespace FBXSDK_NAMESPACE {
class FbxObject;
class FbxDisplayLayer;
class FbxManager;
};

extern FBXSDK_NAMESPACE::FbxManager* fbxSDKManager;

class FbxParse;


/** Panel that contains a tree describing the structure of the
 * scene */
class ScenePanel : public wxPanel
{
	DECLARE_DYNAMIC_CLASS(ScenePanel);
	
public:

	/* Item data associated with each item in the tree */
	class SceneTreeItemData : public wxTreeItemData
	{
	public:
		SceneTreeItemData(FbxObject* node = NULL) : object_(node)
		{
		}

		FbxNode* getNode() {
			if ((object_ != NULL) && (object_->Is(FbxNode::ClassId)))
				return FbxCast<FbxNode>(object_);
			else
				return NULL;
		}

		FbxObject *getData()
		{
			return object_;
		}

		bool valid() {
			return (object_ != NULL);
		}
		
	private:
		FbxObject* object_;
	};

	/**
	 * Second stage constructor
	 * @param parent Parent widget
	 * @param id Window ID
	 * @param pos Window position
	 * @param size Window Size
	 * @param style Style flags for window
	 * @param name Name of window
	 */
	ScenePanel(wxWindow* parent, wxWindowID id = wxID_ANY,
			   const wxPoint& pos = wxDefaultPosition,
			   const wxSize& size = wxDefaultSize,
			   long style = wxTAB_TRAVERSAL,
			   const wxString& name = wxT("FBX Scene File Contents"));


	/**
	 * Called back to establish the tooltip of a node
	 * @param evt Tooltip Event
	 */
	void SceneTooltipSet(wxTreeEvent& evt);

	/**
	 * Called when a node in the scene tree is selected
	 * @param evt Selection Event
	 */
	void SelectedNode(wxTreeEvent& evt);

	/**
	 * Called when a node in the scene tree is activated
	 * @param evt Activation Event
	 */
	void ActivatedNode(wxTreeEvent& evt);

	
	/**
	 * Called when a new FBX scene is to be imported
	 * @param fileName name of file to import
	 * @return true if import was successful
	 */
	bool SetScene(const std::string& fileName);

	/**
	 * Accsessor to scene root
	 * @return root of the FBX scene
	 */
	const FbxScene* Scene() const { return scene_; };

	/**
	 * Clear the scene
	 * @return <doc>
	 */
	virtual bool ClearScene();
	
	/** < D'Tor - to clean up our own resources	 */
	virtual ~ScenePanel();

private:
	void OnCreateScene(wxCommandEvent& evt);
	void OnLoadScene(wxCommandEvent& evt);
	void OnUnloadScene(wxCommandEvent& evt);
	void OnClearScene(wxCommandEvent& evt);
	
	bool ProcessNode(FbxNode* pNode, wxTreeItemId treeItem, bool root, FbxDisplayLayer* layer = NULL);	
	
	//!< Actual scene root
	FbxScene* scene_;

	//!< Origin node of scene
	FbxNode* origin_;

	/** Id of scene tree control for event purposes */
	wxWindowID sceneTreeId_;
	
	//!< Scene tree control 
	SceneTree* sceneTree_;

	//!< Panel With Control buttons
	wxPanel*   buttonPanel_;
	
	//!< Sizer to contain tree
	wxBoxSizer *sizer_;

	// action buttons
    wxButton     *createButton_, *loadButton_, *unloadButton_, *clearButton_;
	
	/** Description for parser_. */
	FbxParse   *parser_;

	DECLARE_EVENT_TABLE()
	
};

#endif
