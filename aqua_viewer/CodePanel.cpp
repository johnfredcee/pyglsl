#include "StdAfx.h"

#include "Config.h"
#include "ViewerFrame.h"
#include "CodeEditorDefaults.h"
#include "CodeEditorPreferences.h"
#include "CodeEditor.h"
#include "CodePanel.h"
#include "CodePanelCollection.h"

const wxWindowID ID_CODE_FILE_MENU_NEW = wxNewId();
const wxWindowID ID_CODE_FILE_MENU_OPEN = wxNewId();
const wxWindowID ID_CODE_FILE_MENU_SAVE = wxNewId();

BEGIN_EVENT_TABLE(CodePanel, wxFrame)
	EVT_MENU(ID_CODE_FILE_MENU_SAVE,         CodePanel::OnFileSave)
	EVT_CLOSE(CodePanel::OnClose)
END_EVENT_TABLE()


CodePanel::CodePanel(wxWindow *parent, wxWindowID winid,  const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxFrame(parent, winid, name, pos, size, style, name) {
	wxBoxSizer *sizer = new  wxBoxSizer(wxHORIZONTAL);
	editor_ = new CodeEditor(this, wxID_ANY);
	createMenuBar();
	sizer->Add(editor_, 1, wxEXPAND);
	SetSizer(sizer);
	Layout();
	editorId_ = wxNewId();
}

CodePanel::~CodePanel() {
}

void CodePanel::createMenuBar() {
	mMenuBar = new wxMenuBar();
	createFileMenu();
	SetMenuBar(mMenuBar);
}

/** Add an Open/Close/New File menu to the frame */
void CodePanel::createFileMenu() {
	mFileMenu = new wxMenu();
	wxMenuItem *menuItem = new wxMenuItem(mFileMenu, ID_CODE_FILE_MENU_NEW, wxT("&New"));
	mFileMenu->Append(menuItem);
	menuItem = new wxMenuItem(mFileMenu, ID_CODE_FILE_MENU_OPEN, wxT("&Open"));
	mFileMenu->Append(menuItem);
	menuItem = new wxMenuItem(mFileMenu, ID_CODE_FILE_MENU_SAVE, wxT("&Save"));
	//menuItem->SetBitmap(IconManager::getSingleton().getIcon(IconManager::SAVE));
	mFileMenu->Append(menuItem);
	mFileMenu->UpdateUI();
	mMenuBar->Append(mFileMenu, wxT("&File"));
}

void CodePanel::WriteConfig() {
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	config->Write(wxT("CodeFileName"), codeFileName_);
	aqua::Locator::checkIn(config);
}


void CodePanel::ReadConfig() {
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	config->Read(wxT("CodeFileName"), &codeFileName_,
				 wxT("Unknown"));
	aqua::Locator::checkIn(config);
	return;
}

void CodePanel::OpenFile(const wxString& fileName) {
	wxFileName fn(fileName);
	wxString ext = fn.GetExt();
	if(ext.CmpNoCase("LUA") == 0) {
		editor_->SetLexer(wxSTC_LEX_LUA);
	}
	if(!fn.FileExists()) {
		wxTextFile tf(fileName);
		tf.Create();
		tf.AddLine(wxT("-- Created with Playbox Framework"));
		tf.Write();
		tf.Close();
	}
	editor_->LoadFile(fileName);
	codeFileName_ = fileName;
	SetTitle(codeFileName_);
}


void CodePanel::OnClose(wxCloseEvent& evt) {
	(void) evt;
	ViewerFrame* viewer = ViewerFrame::getViewerFrame();
	CodePanelCollection*  collection = viewer->getCodePanelCollection();
	collection->removeMember(this);
	editor_->SaveFile();
	Destroy();
}

void CodePanel::OnFileNew(wxCommandEvent& event) {
	(void) event;
	editor_->Clear();
	codeFileName_ = wxT("Unknown");
	SetTitle(codeFileName_);
}

void CodePanel::OnFileOpen(wxCommandEvent& event) {
	(void) event;
	wxFileDialog *fd = new wxFileDialog(this, wxT("Open A LUA File."), wxEmptyString, wxEmptyString, wxT("LUA files (*.lua)|*.pbn|ALL files (*.*)|*.*"), wxFD_OPEN);
	if(fd->ShowModal() == wxID_OK) {
		OpenFile(fd->GetPath());
	}
}


/**
 * Save the file
 * @param event Menu exent details
 */
void CodePanel::OnFileSave(wxCommandEvent& event) {
	(void) event;
	editor_->SaveFile();
}

