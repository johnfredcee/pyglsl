
#ifndef __MetaScenePanel__
#define __MetaScenePanel__

class MetaEditorPanel;

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


class MetaEditorPanel;
class wxListItem;

///////////////////////////////////////////////////////////////////////////////
/// Class MetaScenePanel
///////////////////////////////////////////////////////////////////////////////
class MetaScenePanel : public wxPanel 
{
private:
	DECLARE_CLASS(MetaEditorPanel);

protected:
	wxListCtrl*		 sceneList;
	MetaEditorPanel* metaEditorPanel;
	
public:
		
	/**
	 * Constructor
	 * @param parent Parent window
	 * @param id Window Id
	 * @param pos Initial position
	 * @param size initial size
	 * @param style style flags
	 */
	MetaScenePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
	~MetaScenePanel();

	/**
	 * Refresh the display with a new metadata scene
	 * @param metaData metadata to use
	 * @param ids ids to display
	 */
	void refresh(aqua::shared_ptr<DynamicMetaData> metaData);
private:

	/**
	 * Handle a scene item in the list being activated.
	 * @param evt Event
	 */
	void OnSceneItemActivated(wxListEvent& evt);


};

#endif //__MetaScenePanel__
