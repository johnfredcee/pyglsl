#ifndef SCENETREE_H
#define SCENETREE_H

class SceneTree : public wxTreeCtrl
{
public:
    SceneTree();
    SceneTree(wxWindow *parent, wxWindowID id = wxID_ANY,
			  const wxPoint& pos = wxDefaultPosition,
			  const wxSize& size = wxDefaultSize,
			  long style = wxTR_DEFAULT_STYLE,
			  const wxValidator &validator = wxDefaultValidator,
			  const wxString& name = wxTreeCtrlNameStr);	
    virtual ~SceneTree();
private:
	DECLARE_EVENT_TABLE();
};

#endif
