#include "StdAfx.h"

#include "ShaderListView.h"
#include "ShaderPanel.h"

wxWindowID ID_SHADER_LIST     = wxNewId();
wxWindowID ID_SHADER_EDITOR   = wxNewId();

BEGIN_EVENT_TABLE(ShaderPanel, wxPanel)
EVT_LIST_ITEM_SELECTED(ID_SHADER_LIST, OnListItemSelected)
EVT_LIST_ITEM_DESELECTED(ID_SHADER_LIST, OnListItemDeselected)
EVT_LIST_ITEM_ACTIVATED(ID_SHADER_LIST, OnListItemActivated)
END_EVENT_TABLE()

ShaderPanel::ShaderPanel(wxWindow* parent, wxWindowID id /* = wxID_ANY */, const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxDeafultSize */, long style /* = wxTAB_TRAVERSAL */, const wxString& name /* =  */)
	: wxPanel(parent, id, pos, size, style, name) {

	mBoxSizer = new wxBoxSizer(wxVERTICAL);

	mShaderList = new ShaderListView(this, ID_SHADER_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_VIRTUAL | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL | wxLC_SORT_DESCENDING) ;
	mBoxSizer->Add(mShaderList, 1, wxEXPAND | wxALL, 0);

	mShaderEditor = new wxStyledTextCtrl(this, ID_SHADER_EDITOR);
	mShaderEditor->SetLexer(wxSTC_LEX_NULL);
	mBoxSizer->Add(mShaderEditor, 1, wxEXPAND | wxALL, 0);
	
	SetSizer(mBoxSizer);
	Layout();	
}

ShaderPanel::~ShaderPanel() {
}


void ShaderPanel::OnListItemSelected(wxListEvent& evt) {
	
	wxLogVerbose("Selected : %s ", evt.GetText());
	wxFileName shaderFile(mShaderList->GetFullShaderPath(evt.GetText()));
	mShaderEditor->LoadFile(shaderFile.GetFullPath());	
	return;
}

void ShaderPanel::OnListItemDeselected(wxListEvent& evt) {
	
	wxLogVerbose("DeSelected : %s ", evt.GetText());
	mShaderEditor->ClearAll();
	return;
	
}

void ShaderPanel::OnListItemActivated(wxListEvent& evt) {
	
	wxLogVerbose("Activated : %s ", evt.GetText());
	// actually insert into pbn
	return;
	
}

