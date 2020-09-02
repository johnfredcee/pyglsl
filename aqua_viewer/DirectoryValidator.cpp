
#include "StdAfx.h"
#include <wx/validate.h>
#include "DirectoryValidator.h"

bool DirectoryValidator::TransferToWindow() {
    wxASSERT(GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));	
    if ( m_var )
    {
        wxTextCtrl* cb = (wxTextCtrl*)GetWindow();
        if ( !cb )
            return false;

        cb->SetValue(*m_var);
    }

    return true;	
}

bool DirectoryValidator::TransferFromWindow() {
    wxASSERT(GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));	
    if ( m_var )
    {
        wxTextCtrl* cb = (wxTextCtrl*)GetWindow();
        if ( !cb )
            return false;

        cb->SetValue(*m_var);
    }

    return true;	
}


bool DirectoryValidator::Validate(wxWindow *WXUNUSED(parent))
{
    wxASSERT(GetWindow()->IsKindOf(CLASSINFO(wxTextCtrl)));

    wxTextCtrl* cb = (wxTextCtrl*)GetWindow();
	wxString entered = cb->GetValue();
	wxFileName pathname(entered + wxFileName::GetPathSeparator(wxPATH_UNIX));
	if (!pathname.DirExists()) {
		return false;
	}		
    if (m_var)
        *m_var = pathname.GetFullPath();
    return true;
}

