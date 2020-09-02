#if !defined(SHADERPANEL_H_INCLUDED)
#define SHADERPANEL_H_INCLUDED


class wxBoxSizer;
class wxListCtrl;
class wxStyledTextCtrl;
class wxDir;
class ShaderListView;
class wxListEvent;

class ShaderPanel : public wxPanel
{
public:
	ShaderPanel(wxWindow* parent, wxWindowID id = wxID_ANY,
				const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
				long style = wxTAB_TRAVERSAL, const wxString& name = wxT("ShaderPanel"));
	virtual ~ShaderPanel();
	
protected:
	wxBoxSizer*       mBoxSizer;
	ShaderListView*   mShaderList;
	wxStyledTextCtrl* mShaderEditor;
	
private:
	void OnListItemSelected(wxListEvent& evt);
	void OnListItemDeselected(wxListEvent& evt);
	void OnListItemActivated(wxListEvent& evt);
	
	DECLARE_EVENT_TABLE();
	
	
};
#endif



