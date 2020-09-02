
#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H



class PreferencesDialog: public wxDialog {
public:

    PreferencesDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxSize(640,240), long style=wxDEFAULT_DIALOG_STYLE);
	virtual ~PreferencesDialog();
	
private:
    void set_properties();
    void do_layout();


	wxString BrowseForDirectory(const wxString& dir);
	wxString BrowseForFile(const wxString& dir);

	void OnProjectDirectory(wxCommandEvent& event);
	void OnProjectName(wxCommandEvent& event);
	void OnMetaDirectory(wxCommandEvent& event);	
	void OnShaderDir(wxCommandEvent& event);
	void OnArtDirectory(wxCommandEvent& event);
	void OnTextureDirectory(wxCommandEvent& event);
	void OnConfigFile(wxCommandEvent& event);
	
	virtual bool TransferDataFromWindow();
	virtual bool TransferDataToWindow();
	
	
protected:
    wxStaticBox* MainSizer_staticbox;
	
    wxStaticText* ProjectDirectory;
    wxTextCtrl*   ProjectDirCtrl;
    wxButton*     BrowseProjectButton;

    wxStaticText* ProjectName;
    wxTextCtrl*   NameCtrl;

    wxStaticText* MetaDirectory;
    wxTextCtrl* MetaCtrl;
    wxButton* BrowseMetaButton;
	
    wxStaticText* ArtDirectory;
    wxTextCtrl* ArtCtrl;
    wxButton* BrowseArtButton;

    wxStaticText* TextureDirectory;
    wxTextCtrl* TextureCtrl;
    wxButton* BrowseTextureButton;
	
    wxStaticText* ConfigFile;
    wxTextCtrl* ConfigCtrl;
    wxButton* BrowseConfigButton;
	
	DECLARE_EVENT_TABLE();
}; 


#endif // PREFERENCESDIALOG_H
