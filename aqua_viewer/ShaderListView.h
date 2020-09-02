#if !defined(SHADERLISTVIEW_H_INCLUDED)
#define SHADERLISTVIEW_H_INCLUDED

//!< List view that knows about shaders, maintains a list of them
class ShaderListView : public wxListView
{
public:
	//!< C'tor
	ShaderListView();
	
	//!
	//! Main constructor
	//! @param parent Parent Window
	//! @param winid Window Id
	//! @param pos Position
	//! @param size Size
	//! @param style Style (stick with the default)
	//! @param validator Validator
	//! @param name Name of window 
	//!
	ShaderListView( wxWindow *parent,
					wxWindowID winid = wxID_ANY,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = wxLC_REPORT,
					const wxValidator& validator = wxDefaultValidator,
					const wxString &name = wxListCtrlNameStr);
	
	//!
	//! Return the path of the directory this list view uses to locate shaders
	//! @param filename name of shader file
	//! @return full path of shader file
	//!
	wxFileName GetFullShaderPath(const wxString& filename) const;
	
	//!
	//! Return a pointer to the sole instance of the window in the app
	//! @return Pointer to list view
	//!
	static const ShaderListView* Instance();
	
	//!
	//! Check to see if the given shader name is a valid shader
	//! @param filename File name of shader
	//! @return true if the shader is valid
	//!
	bool IsValidShader(const wxString& filename) const;

	//!
	//! Scan the shader directory and populate the control
	//! @return array of file names
	//!
	aqua::shared_ptr<wxArrayString> ScanShaderDirectory();
	
protected:
	wxDir*                          shaderDirectory_;
	aqua::shared_ptr<wxArrayString> files_;
	
	aqua::shared_ptr<wxArrayString> BrowseForShaderDirectory();
	virtual wxString OnGetItemText(long item, long column) const;

	
private:	
	aqua::shared_ptr<wxArrayString> ShaderDirectoryContents();
	void OnShaderLocate(wxCommandEvent& event);
	void OnShaderRefresh(wxCommandEvent& event);	
	void ShowContextMenu(const wxPoint& pos);
	void OnRightUp(wxMouseEvent& event);

	DECLARE_EVENT_TABLE();
	
};
#endif

