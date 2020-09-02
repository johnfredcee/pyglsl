
#include "StdAfx.h"
#include "Config.h"
#include "DirectoryValidator.h"
#include "PreferencesDialog.h"

// using wxConfig instead of writing wxFileConfig or wxRegConfig enhances
// portability of the code

// ids for buttons
wxWindowID ID_PROJECT_DIR_BROWSE		= wxNewId();
wxWindowID ID_PROJECT_NAME_BROWSE		= wxNewId();
wxWindowID ID_META_DIR_BROWSE			= wxNewId();
wxWindowID ID_ART_DIR_BROWSE			= wxNewId();
wxWindowID ID_TEXTURE_DIR_BROWSE		= wxNewId();
wxWindowID ID_CONFIG_FILE_BROWSE		= wxNewId();

// ids for text fields
wxWindowID ID_PROJECT_TEXT	  = wxNewId();
wxWindowID ID_CODE_TEXT		  = wxNewId();
wxWindowID ID_META_TEXT		  = wxNewId();
wxWindowID ID_ART_TEXT		  = wxNewId();
wxWindowID ID_TEXTURE_TEXT	  = wxNewId();
wxWindowID ID_CONFIG_TEXT	  = wxNewId();

BEGIN_EVENT_TABLE(PreferencesDialog, wxDialog)
	EVT_BUTTON(ID_PROJECT_DIR_BROWSE, PreferencesDialog::OnProjectDirectory)
	EVT_BUTTON(ID_PROJECT_NAME_BROWSE, PreferencesDialog::OnProjectName)
	EVT_BUTTON(ID_META_DIR_BROWSE, PreferencesDialog::OnMetaDirectory)
	EVT_BUTTON(ID_ART_DIR_BROWSE, PreferencesDialog::OnArtDirectory)
//	EVT_BUTTON(ID_TEXTURE_DIR_BROWSE, PreferencesDialog::OnTextureDirectory)
	EVT_BUTTON(ID_CONFIG_FILE_BROWSE, PreferencesDialog::OnConfigFile)
END_EVENT_TABLE()

/**
 * Constructor
 * @param parent Parent window
 * @param id Dialog id
 * @param title Dialog title
 * @param pos Position of dialog
 * @param size Size of dialog
 * @param style Style of dialong
 */
PreferencesDialog::PreferencesDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) :
wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE) {
	PBUNREFERENCED_PARAMETER(style);

	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());	
	MainSizer_staticbox = new wxStaticBox(this, -1, wxT("Directories"), wxDefaultPosition, wxSize(620,240));
	ProjectDirectory = new wxStaticText(this, wxID_ANY, wxT("Project Directory"));
	ProjectDirCtrl = new wxTextCtrl(this, ID_PROJECT_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0L, DirectoryValidator(&config->projectDirectory_));
	BrowseProjectButton = new wxButton(this, ID_PROJECT_DIR_BROWSE, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

	ProjectName = new wxStaticText(this, wxID_ANY, wxT("Project Name"));
	NameCtrl = new wxTextCtrl(this, ID_CODE_TEXT, wxEmptyString);

	MetaDirectory = new wxStaticText(this, wxID_ANY, wxT("Meta Directory"));
	MetaCtrl = new wxTextCtrl(this, ID_META_TEXT, wxEmptyString);
	BrowseMetaButton = new wxButton(this, ID_META_DIR_BROWSE, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

	ArtDirectory = new wxStaticText(this, wxID_ANY, wxT("Art Directory"));
	ArtCtrl = new wxTextCtrl(this, ID_ART_TEXT, wxEmptyString);
	BrowseArtButton = new wxButton(this, ID_ART_DIR_BROWSE, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

	ConfigFile = new wxStaticText(this, wxID_ANY, wxT("Config File"));
	ConfigCtrl = new wxTextCtrl(this, ID_CONFIG_TEXT, wxEmptyString);
	BrowseConfigButton = new wxButton(this, ID_CONFIG_FILE_BROWSE, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

	SetInitialSize(wxSize(640,420));
	set_properties();
	do_layout();
	TransferDataToWindow();
	aqua::Locator::checkIn(config);	
}

/** Destructor **/
PreferencesDialog::~PreferencesDialog() {
}

/** Set any associated properties  */
void PreferencesDialog::set_properties() {
	SetTitle(wxT("Preferences"));
}


/** Layout the dialog */
void PreferencesDialog::do_layout() {
	// basically a verticial list of files/dir with standard buttons at the bottom
	wxStaticBoxSizer* MainSizer = new wxStaticBoxSizer(MainSizer_staticbox, wxVERTICAL);
	wxBoxSizer* AnotherSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* YetAnotherSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* WowAnotherSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* ArtSizer = new wxBoxSizer(wxVERTICAL);
	//wxBoxSizer* TextureSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* ConfigSizer = new wxBoxSizer(wxVERTICAL);
	wxStdDialogButtonSizer* ButtonSizer = new wxStdDialogButtonSizer();
	
	AnotherSizer->Add(ProjectDirectory, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 3);
	AnotherSizer->Add(ProjectDirCtrl, 0, wxEXPAND|wxALIGN_RIGHT, 3);
	AnotherSizer->Add(BrowseProjectButton, 0, 0, 0);
	MainSizer->Add(AnotherSizer, 1, wxEXPAND, 0);

	YetAnotherSizer->Add(ProjectName, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 3);
	YetAnotherSizer->Add(NameCtrl, 0, wxEXPAND|wxALIGN_RIGHT, 3);
	MainSizer->Add(YetAnotherSizer, 1, wxEXPAND, 0);

	// meta dir
	WowAnotherSizer->Add(MetaDirectory, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 3);
	WowAnotherSizer->Add(MetaCtrl, 0, wxEXPAND|wxALIGN_RIGHT, 3);
	WowAnotherSizer->Add(BrowseMetaButton, 0, 0, 0);
	MainSizer->Add(WowAnotherSizer, 1, wxEXPAND, 0);

	// art dir
	ArtSizer->Add(ArtDirectory, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 3);
	ArtSizer->Add(ArtCtrl, 0, wxEXPAND|wxALIGN_RIGHT, 3);
	ArtSizer->Add(BrowseArtButton, 0, 0, 0);
	MainSizer->Add(ArtSizer, 1, wxEXPAND, 0);
	
	
	ConfigSizer->Add(ConfigFile, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 3);
	ConfigSizer->Add(ConfigCtrl, 0, wxEXPAND|wxALIGN_RIGHT, 3);
	ConfigSizer->Add(BrowseConfigButton, 0, 0, 0);
	MainSizer->Add(ConfigSizer, 1, wxEXPAND, 0);
	ButtonSizer->AddButton(new wxButton(this, wxID_OK));
	ButtonSizer->AddButton(new wxButton(this, wxID_CANCEL));
	MainSizer->Add(ButtonSizer, 1, wxEXPAND, 0);
	SetSizer(MainSizer);
	ButtonSizer->Realize();
	Layout();
}

wxString PreferencesDialog::BrowseForDirectory(const wxString& dir) {
	wxString result = dir;
	wxString candidate;
	wxDirDialog dirDlg(GetParent(), wxT("Browse for Directory"), dir, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if(dirDlg.ShowModal() == wxID_OK) {
		candidate = dirDlg.GetPath();
	}
	wxFileName dirName(candidate);
	if(dirName.DirExists()) {
		result = candidate;
	}
	return result;
}

wxString PreferencesDialog::BrowseForFile(const wxString& dir) {
	wxString result = dir;
	wxString candidate;
	wxFileDialog fileDlg(GetParent(), wxT("Browse for File"), dir, "", "XML Files (*.xml)|*.xml|All Files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if(fileDlg.ShowModal() == wxID_OK) {
		candidate = fileDlg.GetPath();
	}
	wxFileName fileName(candidate);
	if(fileName.FileExists()) {
		result = candidate;
	}
	return result;
}

void PreferencesDialog::OnProjectDirectory(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	config->projectDirectory_ = BrowseForDirectory(config->projectDirectory_);
	ProjectDirCtrl->SetValue(config->projectDirectory_);
	config->Write("ProjectDirectory", config->projectDirectory_);
	aqua::Locator::checkIn(config);	
}

void PreferencesDialog::OnProjectName(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	config->projectName_ = BrowseForDirectory(config->projectName_);
	NameCtrl->SetValue(config->projectName_);
	config->Write("ProjectName", config->projectName_);
	aqua::Locator::checkIn(config);	
}

void PreferencesDialog::OnMetaDirectory(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	config->metaDirectory_ = BrowseForDirectory(config->metaDirectory_);
	MetaCtrl->SetValue(config->metaDirectory_);
	config->Write("MetaDirectory", config->metaDirectory_);
	aqua::Locator::checkIn(config);	
}

void PreferencesDialog::OnArtDirectory(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	config->artDirectory_ = BrowseForDirectory(config->artDirectory_);
	ArtCtrl->SetValue(config->artDirectory_);
	config->Write("ArtDirectory", config->artDirectory_);
	aqua::Locator::checkIn(config);	
}

void PreferencesDialog::OnTextureDirectory(wxCommandEvent& event) {

	PBUNREFERENCED_PARAMETER(event);
/*
   const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	config->textureDirectory_ = BrowseForDirectory(config->textureDirectory_);
	TextureCtrl->SetValue(config->textureDirectory_);
	config->Write("TextureDirectory", config->textureDirectory_);
	aqua::Locator::checkIn(config);	
*/
}

void PreferencesDialog::OnConfigFile(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	config->configFile_ = BrowseForFile(config->configFile_);
	ConfigCtrl->SetValue(config->configFile_);
	config->Write("configFile", config->configFile_);
	aqua::Locator::checkIn(config);	
}


bool PreferencesDialog::TransferDataToWindow() {
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	ProjectDirCtrl->SetValue(config->projectDirectory_);
	NameCtrl->SetValue(config->projectName_);
	MetaCtrl->SetValue(config->metaDirectory_);
	ConfigCtrl->SetValue(config->configFile_);
	ArtCtrl->SetValue(config->artDirectory_);
	aqua::Locator::checkIn(config);	
	return true;
}

bool PreferencesDialog::TransferDataFromWindow() {
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	config->projectDirectory_ = ProjectDirCtrl->GetValue();
	config->projectName_ = NameCtrl->GetValue();
	config->metaDirectory_ = MetaCtrl->GetValue();
	config->configFile_ = ConfigCtrl->GetValue();
	config->artDirectory_ = ArtCtrl->GetValue();
	aqua::Locator::checkIn(config);	
	return true;
}
