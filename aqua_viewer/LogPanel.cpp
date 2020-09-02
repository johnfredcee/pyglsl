#include "StdAfx.h"

#include <wx/textctrl.h>
#include <wx/strconv.h>
#include <wx/scrolwin.h>

#include <windows.h> // used for outputdebugstring (need platform independent alternative)

#include "LogPanel.h"


wxWindowID ID_DEBUG_WINDOW = wxNewId();
wxWindowID ID_ERROR_WINDOW = wxNewId();

LogPanel::LogPanel(wxWindow* parent, wxWindowID id /* = wxID_ANY */, const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxDeafultSize */, long style /* = wxTAB_TRAVERSAL */, const wxString& name /* =  */)
	: wxScrolledWindow(parent, id, pos, size, style, name),
	  wxLog()
{
	mBoxSizer = new wxBoxSizer(wxHORIZONTAL);
	
	mDebugTextControl = new wxTextCtrl(this, ID_DEBUG_WINDOW, wxT(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTE_MULTILINE);
	mDebugTextControl->SetEditable(false);
	mBoxSizer->Add(mDebugTextControl, 1, wxEXPAND | wxALL, 0);

	mErrorTextControl = new wxTextCtrl(this, ID_ERROR_WINDOW, wxT(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTE_MULTILINE);
	mErrorTextControl->SetEditable(false);
	mBoxSizer->Add(mErrorTextControl, 1, wxEXPAND | wxALL, 0);
	
	SetSizer(mBoxSizer);
	Layout();
	wxLog::SetTimestamp(wxT("[%d/%b/%y %H:%M:%S] "));
	mOldLog = wxLog::GetActiveTarget();
	mDebugTextControl->AppendText(wxT("Debug Log"));
	mErrorTextControl->AppendText(wxT("Error Messages"));
}

LogPanel::~LogPanel()
{
	wxLog::SetActiveTarget(mOldLog);
}

void LogPanel::debugMessageLogged(const std::string& message)
{
	wxString msg(message.c_str(), wxConvUTF8);
	mDebugTextControl->AppendText(msg);
	mDebugTextControl->AppendText(wxT("\n"));
//	OutputDebugStringA(message.c_str());
//	OutputDebugStringA("\n");	
}

void LogPanel::errorMessageLogged(const std::string& message)
{
	wxString msg(message.c_str(), wxConvUTF8);
	mErrorTextControl->AppendText(msg);
	mErrorTextControl->AppendText(wxT("\n"));
//	OutputDebugStringA(message.c_str());
//	OutputDebugStringA("\n");	
}

void LogPanel::DoLogTextAtLevel(wxLogLevel level, const wxString& msg)
{
	if ((level == wxLOG_FatalError) || (level == wxLOG_Error))
		errorMessageLogged(std::string(msg.utf8_str()));
	else
		debugMessageLogged(std::string(msg.utf8_str()));
	return;
}

