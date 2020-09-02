#include "StdAfx.h"

#include <wx/treectrl.h>
#include "SceneTree.h"


BEGIN_EVENT_TABLE(SceneTree, wxTreeCtrl)
END_EVENT_TABLE();

//!< C'tor
SceneTree::SceneTree() {
	
}
	
//!
//! Description for SceneTree.
//! @param parent <doc>
//! @param id <doc>
//! @param pos <doc>
//! @param size <doc>
//! @param style <doc>
//! @param validator <doc>
//! @param name <doc>
//!
SceneTree::SceneTree(wxWindow *parent,
					 wxWindowID id,
					 const wxPoint& pos,
					 const wxSize& size,
					 long style,
					 const wxValidator &validator,
					 const wxString& name) : wxTreeCtrl(parent, id, pos, size, style, validator, name) {
	
}

//!< D'tor
SceneTree::~SceneTree() {
	
}

