
#ifndef _LOGPANEL_H_
#define _LOGPANEL_H_

class wxBoxSizer;
class wxTextCtrl;

class LogPanel : public wxScrolledWindow, public wxLog
{
public:
	LogPanel(wxWindow* parent, wxWindowID id = wxID_ANY,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             long style = wxTAB_TRAVERSAL, const wxString& name = wxT("LogPanel"));

	virtual ~LogPanel();

	virtual void debugMessageLogged(const std::string& message);
	virtual void errorMessageLogged(const std::string& message);
	
	void DoLogTextAtLevel(wxLogLevel level, const wxString& message);
		
protected:
	wxLog*      mOldLog;
	wxBoxSizer* mBoxSizer;
	wxTextCtrl* mDebugTextControl;
	wxTextCtrl* mErrorTextControl;
};
#endif // _LOGPANEL_H_

