#include "StdAfx.h"

#include <Utils/stringfunctions.h>

#include "SceneManager.h"
#include "MetaEditorPanel.h"

#include "MetaScenePanel.h"

using namespace aqua;

IMPLEMENT_CLASS(MetaScenePanel, wxPanel)

wxWindowID  wxID_SCENE_SIZER = wxNewId();
wxWindowID  wxID_ITEM_SIZER  = wxNewId();
wxWindowID  wxID_ITEM_PANEL  = wxNewId();
wxWindowID  wxID_SCENE_LIST  = wxNewId();


///////////////////////////////////////////////////////////////////////////

MetaScenePanel::MetaScenePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sceneSizer;
	sceneSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_SCENE_SIZER, wxT("Scene") ), wxHORIZONTAL );
	
	sceneList = new wxListCtrl( this, wxID_SCENE_LIST, wxDefaultPosition, wxDefaultSize, wxLC_SINGLE_SEL | wxLC_REPORT);
	sceneSizer->Add( sceneList, 1, wxALL | wxEXPAND, 5 );
	Connect(wxID_SCENE_LIST, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(MetaScenePanel::OnSceneItemActivated));

// Insert three columns
	wxListItem itemCol;
	itemCol.SetText(wxT("Item"));
	itemCol.SetImage(-1);
	sceneList->InsertColumn(0, itemCol);
	sceneList->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER );
	itemCol.SetText(wxT("Item Type"));
	sceneList->InsertColumn(1, itemCol);
	sceneList->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER );
	itemCol.SetText(wxT("Layer"));
	sceneList->InsertColumn(2, itemCol);
	sceneList->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER );
	
	mainSizer->Add( sceneSizer, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* itemSizer;
	itemSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ITEM_SIZER, wxT("Item") ), wxHORIZONTAL );
	
	metaEditorPanel = new MetaEditorPanel( this, wxID_ITEM_PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	itemSizer->Add( metaEditorPanel, 1, wxEXPAND | wxALL, 5 );
	
	mainSizer->Add( itemSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}


MetaScenePanel::~MetaScenePanel()
{
}

void MetaScenePanel::refresh(aqua::shared_ptr<DynamicMetaData> metaData) {
	SceneManager::Handle selectedHandle = 0;
	long selIndex = sceneList->GetNextItem(-1, wxLIST_NEXT_ALL,	 wxLIST_STATE_SELECTED);
	if (selIndex != -1) {
		selectedHandle = (SceneManager::Handle) sceneList->GetItemData(selIndex);
	}
	sceneList->DeleteAllItems( );
	sceneList->Freeze();
	aqua::Array<std::string> layerNames;
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
	sceneManager->getLayers(layerNames);
	int itemIndex = 0;
	for(aqua::Array<std::string>::iterator l_it = layerNames.begin(); 
		l_it != layerNames.end();
		++l_it) {
		wxString layerName(l_it->c_str());
		aqua::Array<SceneManager::Handle> handles;
		sceneManager->getSceneObjectsInLayer(*l_it, handles);
		for(aqua::Array<SceneManager::Handle>::iterator h_it = handles.begin();
			h_it != handles.end();
			++h_it) {
			// TO DO.. populate list ctrl
			aqua::shared_ptr<const aqua::IGraphicObject > object = sceneManager->getObject(*h_it);
			wxString name(object->getObjectName().c_str());
			Rtti type(object->type());
			wxString typeName(type.getName());
			sceneList->InsertItem(itemIndex, name);
			sceneList->SetItem(itemIndex, 1, typeName);
			sceneList->SetItem(itemIndex, 2, layerName);
			sceneList->SetItemData(itemIndex, (long) *h_it);
			if (*h_it == selectedHandle) {
				sceneList->SetItemState(itemIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
			}
			++itemIndex;
		}
	}
	aqua::Locator::checkIn(sceneManager);
	metaEditorPanel->setMetaData(metaData);
	sceneList->Thaw();
}

void MetaScenePanel::OnSceneItemActivated(wxListEvent& evt) {

	wxListItem item = evt.GetItem();
	SceneManager::Handle handle = (SceneManager::Handle) item.GetData();
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
	boost::shared_ptr<IGraphicObject> object = sceneManager->getObject(handle);
	aqua::Locator::checkIn(sceneManager);	
	std::vector<crc32> ids;
	object->ids(ids);
	metaEditorPanel->refresh(ids);
	return;
}
