#ifndef _META_EDITOR_PANEL_H_
#define _META_EDITOR_PANEL_H_

class wxPropertyGrid;
class DynamicMetaData;
class wxPropertyGridEvent;
class wxPGProperty;

class MetaEditorPanel : public wxPanel
{
	DECLARE_CLASS(MetaEditorPanel);
	
	enum PropertyType {
		TYPE_NORMAL
		,	TYPE_ARRAY
		,	TYPE_STRUCT
	};
	
public:
	MetaEditorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
	virtual ~MetaEditorPanel();

	/**
	 * Description for setMetaData.
	 * @param metaData <doc>
	 */
	void setMetaData(aqua::shared_ptr<DynamicMetaData> metaData);

	/**
	 * Update display of panel
	 * @param ids Id's of metadata to detail on the panel
	 */
	void refresh(const SceneManager::sceneIds& ids);
	
private:	
	/**
	 * Add a property to the panel, taking the parent into account
	 * @param child Child property to add
	 * @param parent Parent to add propety to (or NULL if none)
	 */
	wxPGProperty*  addProperty(wxPGProperty* child, wxPGProperty* parent);

	/**
	 * Add a PbColor meta property to the panel
	 * @param parser Colour property parser
	 * @param parent Parent property in panel
	 * @param objectName Obsolete
	 */
	void addColourProperty(DynamicMetaParser& parser, wxPGProperty* parent, const std::string& objectName);

	/**
	 * Add the properties parseable by this parser to the panel
	 * @param parser Meta parser contining data to add
	 * @param parent Parent property of the parser (where new properties appear)
	 * @param  kind  Type of property (SCALAR, STRUCT, ENUM..)
	 * @param index  Index into parser 
	 */
	void addMetaProperty(DynamicMetaParser& parser, wxPGProperty* parent, const PropertyType kind = TYPE_NORMAL, aqua::Uint index = aqua::INVALID);

//	void addMetaProperties(DynamicMetaParser& parser, wxPGProperty* parent, const PropertyType = TYPE_NORMAL, aqua::Uint index = aqua::INVALID);
		
	/**
	 * Event processed when window is resized
	 * @param event Resize event details
	 */
	void OnSize(wxSizeEvent& event);

	void OnChanged(wxPropertyGridEvent& event);
		
	void setValue(DynamicMetaParser& parser, wxPGProperty* property);
private:	
	wxPropertyGrid*						propertyGrid_;
	aqua::shared_ptr<DynamicMetaData>	metaData_;
	DECLARE_EVENT_TABLE();
};

#endif
