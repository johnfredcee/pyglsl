#ifndef FBX_PROPERTY_PANEL_H_
#define FBX_PROPERTY_PANEL_H_


extern KFbxSdkManager* fbxSDKManager;

class FbxPropertyPanel : public wxPanel
{
 private:
	//!< ui
	wxPropertyGrid* sceneProperties_;
	
 public:
	FbxPropertyPanel(wxWindow* parent,
			 wxWindowID id,
			 const wxPoint& pos,
			 const wxSize& size,
			 long style,
			 const wxString& name);


	void DisplayNode(KFbxNode* node);

	void PropertyDoubleClick(wxPropertyGridEvent& evt);

	void PropertyChanged(wxPropertyGridEvent& evt);

	wxBoxSizer *sizer_;
	
	DECLARE_EVENT_TABLE()
};
#endif
