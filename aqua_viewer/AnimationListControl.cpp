#include "stdafx.h"
#include "AnimationListControl.h"

AnimationListControl::AnimationListControl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name) : wxListCtrl(parent, id, pos, size, style, validator, name) {

	   
     	// Add name column
        wxListItem col0;
        col0.SetId(0);
        col0.SetText( _("Name") );
        col0.SetWidth(50);
        InsertColumn(0, col0);
        
        // Add length column
        wxListItem col1;
        col1.SetId(1);
        col1.SetText( _("Length") );
        InsertColumn(1, col1);
        
        // Add pos/rot key
        wxListItem col2;
        col2.SetId(2);
        col2.SetText( _("Type") );
        InsertColumn(2, col2);	
}

AnimationListControl::~AnimationListControl() {
	
}
