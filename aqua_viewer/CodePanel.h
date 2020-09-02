#if !defined(WXCODEPANEL_H_INCLUDED)
#define WXCODEPANEL_H_INCLUDED

class wxCloseEvent;
class CodeEditor;
class wxConfigBase;
class wxWindow;
class wxString;

class CodePanel : public wxFrame
{
public:	
	CodePanel();

	CodePanel(wxWindow *parent,
			  wxWindowID winid,
			  const wxPoint& pos = wxDefaultPosition,
			  const wxSize& size = wxDefaultSize,
 			  long style = wxDEFAULT_FRAME_STYLE,
			  const wxString& name = wxT("Code"));

	void WriteConfig();
	void ReadConfig();
	void OpenFile(const wxString& path);
	
	virtual ~CodePanel();
	
private:
	void createFileMenu();
	void createMenuBar();

    void OnFileNew(wxCommandEvent& event);
    void OnFileOpen(wxCommandEvent& event);
    void OnFileSave(wxCommandEvent& event);

	void OnClose(wxCloseEvent& evt);

	wxWindowID	editorId_;
	CodeEditor* editor_;
	wxString    codeFileName_;
    wxMenuBar*	mMenuBar;
    wxMenu*		mFileMenu;        

	DECLARE_EVENT_TABLE();
};

#endif
