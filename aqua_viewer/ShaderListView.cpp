
#include "StdAfx.h"

#include "Config.h"
#include "ShaderClientData.h"
#include "ShaderListView.h"
#include "ShaderNameValidator.h"

wxWindowID ID_SHADERS_LOCATE  = wxNewId();
wxWindowID ID_SHADERS_REFRESH = wxNewId();


const wxEventType ShaderUpdateEventType = wxNewEventType();

BEGIN_EVENT_TABLE(ShaderListView, wxListView)
	EVT_RIGHT_UP(ShaderListView::OnRightUp)
	EVT_MENU(ID_SHADERS_LOCATE,  ShaderListView::OnShaderLocate)
	EVT_MENU(ID_SHADERS_REFRESH, ShaderListView::OnShaderRefresh)
END_EVENT_TABLE()


ShaderListView::ShaderListView() {
};

ShaderListView::ShaderListView(wxWindow *parent,
							   wxWindowID winid,
							   const wxPoint& pos,
							   const wxSize& size,
							   long style,
							   const wxValidator& validator,
							   const wxString &name) :
	shaderDirectory_(new wxDir(wxGetCwd())),
	wxListView(parent, winid, pos, size, style, validator, name) {
	InsertColumn(0, wxT("File Name"));
	SetColumnWidth(0, 180);
	InsertColumn(1, wxT("Constant"));
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	shaderDirectory_ = new wxDir(config->projectShadersDirectory());
	aqua::Locator::checkIn(config);
	files_ = ShaderDirectoryContents();
	SetItemCount(files_->GetCount());
}



const ShaderListView* ShaderListView::Instance() {
	extern wxWindowID ID_SHADER_LIST;
	return static_cast<ShaderListView*>(FindWindowById(ID_SHADER_LIST));
}

wxFileName ShaderListView::GetFullShaderPath(const wxString& filename) const {
	wxFileName result(shaderDirectory_->GetName(), filename);
	return result;
}

bool ShaderListView::IsValidShader(const wxString& filename) const {
	bool result = false;
	for(unsigned i = 0; i < files_->GetCount(); i++) {
		if(files_->Item(i).CmpNoCase(filename) == 0) {
			result = true;
			break;
		}
	}
	return result;
}


aqua::shared_ptr<wxArrayString> ShaderListView::ShaderDirectoryContents() {
	aqua::shared_ptr<wxArrayString> result(new wxArrayString);
	if(shaderDirectory_->IsOpened()) {
		wxString filename;
		bool cont = shaderDirectory_->GetFirst(&filename, wxT("*.cgfx"));
		while(cont) {
			result->Add(filename);
			cont = shaderDirectory_->GetNext(&filename);
		}
	}
	// notify anyone interested of a change
	wxCommandEvent shaderEvent(ShaderUpdateEventType, GetId());
	ShaderUpdateClientData* clientData = new ShaderUpdateClientData(result);
	shaderEvent.SetClientObject(clientData);
	ProcessEvent(shaderEvent);
	return result;
}

aqua::shared_ptr<wxArrayString> ShaderListView::BrowseForShaderDirectory() {
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	wxString shaderDirectory = config->projectShadersDirectory();
	aqua::Locator::checkIn(config);
	wxDirDialog dirDlg(this, wxT("Shader Dir"), shaderDirectory);
	if(dirDlg.ShowModal() == wxID_OK) {
		delete shaderDirectory_;
		shaderDirectory_ = new wxDir(dirDlg.GetPath());
	} else {
		wxString oldDir = shaderDirectory_->GetName();
		delete shaderDirectory_;
		shaderDirectory_ = new wxDir(oldDir);
	}
	return ShaderDirectoryContents();
}

aqua::shared_ptr<wxArrayString> ShaderListView::ScanShaderDirectory() {
	wxString oldDir = shaderDirectory_->GetName();
	delete shaderDirectory_;
	shaderDirectory_ = new wxDir(oldDir);
	return ShaderDirectoryContents();
}

wxString ShaderListView::OnGetItemText(long item, long column) const {
	switch(column) {
		case 0:
			return (item < (long) files_->GetCount()) ? files_->Item(item) : wxT("+NONE+");
			break;
		default:
			break;
	}
	return wxT("??");
}


void ShaderListView::OnShaderLocate(wxCommandEvent& WXUNUSED(event)) {
	wxLogVerbose("Locating Shaders");
	files_ = BrowseForShaderDirectory();
	SetItemCount(files_->GetCount());
}

void ShaderListView::OnShaderRefresh(wxCommandEvent& WXUNUSED(event)) {
	wxLogVerbose("Refreshing Shaders");
	files_ = BrowseForShaderDirectory();
	SetItemCount(files_->GetCount());
}

void ShaderListView::OnRightUp(wxMouseEvent& event) {
	ShowContextMenu(event.GetPosition());
}

void ShaderListView::ShowContextMenu(const wxPoint& pos) {
	wxMenu menu;
	menu.SetTitle("Shaders");
	menu.Append(ID_SHADERS_LOCATE,   _T("&Locate"));
	menu.Append(ID_SHADERS_REFRESH, _T("&Refresh"));
	PopupMenu(&menu, pos);
}
