/**
 * Note that we can't follow the usual conventions regarding putting .h files inside other .h
 * files. .h files must always be included from .cpp files so that we can strictly control the
 * order since wx, fbx, and our header files have to be included in a specific order in order
 * to avoid problems due to redefinition of macros in windows.h
 * It's a pain. JFC.
 */

#include "StdAfx.h"

#include <wx/glcanvas.h>

#include "Config.h"
#include "FbxParse.h"
#include "SceneClientData.h"
#include "LogPanel.h"
#include "ScenePanel.h"
#include "SceneManager.h"
#include "OpenGLPanel.h"
#include "RenderPanel.h"
#include "LayerManager.h"
#include "CodeEditorDefaults.h"
#include "PreferencesDialog.h"
#include "CodeEditorPreferences.h"
#include "CodeEditor.h"
#include "CodePanel.h"
#include "CodePanelCollection.h"
#include "MetaEditorPanel.h"
#include "MetaScenePanel.h"
#include "MetaDataUtils.h"
#include "ShaderPanel.h"
#include "TextureDefinitionManager.h"
#include "Viewer.h"
#include "ViewerFrame.h"


namespace aqua {

	class LoggerOutputWx : public LoggerOutputFunctor {
		public:
			LoggerOutputWx():LoggerOutputFunctor() {}
			void operator()(const char* logMessage) const { wxLogDebug("%s", wxString::Format("%s", wxString(logMessage).utf8_str())); };
	};

}	// namespace aqua

const wxWindowID ID_FILE_MENU_NEW		  = wxNewId();
const wxWindowID ID_FILE_MENU_OPEN		  = wxNewId();
const wxWindowID ID_FILE_MENU_SAVE		  = wxNewId();
const wxWindowID ID_FILE_MENU_OPEN_CODE	  = wxNewId();
const wxWindowID ID_FILE_MENU_CLOSE		  = wxNewId();
const wxWindowID ID_FILE_MENU_EXIT		  = wxNewId();
const wxWindowID ID_FILE_MENU_IMPORT	  = wxNewId();
const wxWindowID ID_FILE_MENU_EXPORT	  = wxNewId();
const wxWindowID ID_VIEW_MENU_FREE_CAMERA = wxNewId();
const wxWindowID ID_VIEW_MENU_WIREFRAME	  = wxNewId();
const wxWindowID ID_DIRECTORIES_AND_PATHS = wxNewId();
const wxWindowID ID_VERBOSE_LOGGING		  = wxNewId();

BEGIN_EVENT_TABLE(ViewerFrame, wxFrame)
	EVT_MENU(ID_FILE_MENU_NEW,			ViewerFrame::OnFileNew)
	EVT_MENU(ID_FILE_MENU_OPEN,			ViewerFrame::OnFileOpen)
	EVT_MENU(ID_FILE_MENU_OPEN_CODE,	ViewerFrame::OnFileOpenCode)
	EVT_MENU(ID_FILE_MENU_SAVE,			ViewerFrame::OnFileSave)
	EVT_MENU(ID_FILE_MENU_CLOSE,		ViewerFrame::OnFileClose)
	EVT_MENU(ID_FILE_MENU_EXIT,			ViewerFrame::OnFileExit)
	EVT_MENU(ID_FILE_MENU_IMPORT,		ViewerFrame::OnFileImport)
	EVT_MENU(ID_FILE_MENU_EXPORT,		ViewerFrame::OnFileExport)
	EVT_MENU(ID_VIEW_MENU_FREE_CAMERA,	ViewerFrame::OnViewFreeCamera)
	EVT_MENU(ID_VIEW_MENU_WIREFRAME,	ViewerFrame::OnViewWireframe)
	EVT_MENU(ID_DIRECTORIES_AND_PATHS,	 ViewerFrame::OnDirectoriesAndPaths)
	EVT_CLOSE(ViewerFrame::OnClose)
END_EVENT_TABLE()

using namespace aqua;

wxString ViewerFrame::headerDirectory_ = "Unknown";

/**
 * Constructor for main frame window
 * @param parent Parent (usually the desktop)
 * @param id	 Window id
 * @param title	 Title of window as string
 * @param pos	 Initial position
 * @param size	 Initial size
 * @param style	 Window style flags
 */
ViewerFrame::ViewerFrame(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
	: wxFrame(parent, id, title, pos, size, style) {
	PBUNREFERENCED_PARAMETER(style);
	codePanelCollection_ = new CodePanelCollection(this);
	createAuiManager();
	createMenuBar();
	if(!createRendererPane()) {
		throw std::exception();
	}
	createInformationPane();
	createMetaEditorPane();
	// TO DO: This needs work -- frames inside AUi notebook == mess
	// createCodePane();
	auiManager_->Update();
}

/** Destructor - needed to remove AUI manager event handlers */
ViewerFrame::~ViewerFrame() {
	auiManager_->UnInit();
}
/**
 * Accessor functions for other windows to get a pointer to the main window
 * @return	Pointer to the main frame window
 */
ViewerFrame* ViewerFrame::getViewerFrame()	{
	extern wxWindowID ID_MAIN_FRAME;
	return static_cast<ViewerFrame*>(wxWindow::FindWindowById(ID_MAIN_FRAME));
}

/**
 * Return a pointer to the code panels
 * @return pointer to collections
 */
CodePanelCollection* ViewerFrame::getCodePanelCollection() {
	return codePanelCollection_;
}

/** Create the top level AUI manager for things like floating, docking subwindows */
void ViewerFrame::createAuiManager() {
	auiManager_ = new wxAuiManager();
	auiManager_->SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_TRANSPARENT_DRAG);
	auiManager_->SetManagedWindow(this);
	wxAuiDockArt* art = auiManager_->GetArtProvider();
	art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 1);
	art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 4);
	art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 17);
	art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR, wxColour(49, 106, 197));
	art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(90, 135, 208));
	art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR, wxColour(255, 255, 255));
	art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, wxColour(200, 198, 183));
	art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(228, 226, 209));
	art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, wxColour(0, 0, 0));
	auiManager_->Update();
}

/** Cretate the menu bar for the top level window **/
void ViewerFrame::createMenuBar() {
	menuBar_ = new wxMenuBar();
	createFileMenu();
	createViewMenu();
//	createEditMenu();
	createToolsMenu();
//	createWindowMenu();
//	createHelpMenu();
	SetMenuBar(menuBar_);
}

/** Create the file menu and items for the top level window */
void ViewerFrame::createFileMenu() {
	fileMenu_ = new wxMenu();
	// new metadta file
	wxMenuItem *menuItem = new wxMenuItem(fileMenu_, ID_FILE_MENU_NEW, wxT("&New Metadata"));
	fileMenu_->Append(menuItem);
	// open metadata file
	menuItem = new wxMenuItem(fileMenu_, ID_FILE_MENU_OPEN, wxT("&Open Metadata"));
	fileMenu_->Append(menuItem);
	menuItem->Enable(false);
	menuItem = new wxMenuItem(fileMenu_, ID_FILE_MENU_OPEN_CODE, wxT("Open &Code..."));
	//enuItem_->SetBitmap(IconManager::getSingleton().getIcon(IconManager::SAVE_AS));
	fileMenu_->Append(menuItem);
	menuItem = new wxMenuItem(fileMenu_, ID_FILE_MENU_SAVE, wxT("&Save Metadata"));
	//enuItem_->SetBitmap(IconManager::getSingleton().getIcon(IconManager::SAVE));
	fileMenu_->Append(menuItem);
	fileMenu_->AppendSeparator();
	menuItem = new wxMenuItem(fileMenu_, ID_FILE_MENU_IMPORT, wxT("Import Art"));
	//enuItem_->SetBitmap(IconManager::getSingleton().getIcon(IconManager::SAVE_AS));
	fileMenu_->Append(menuItem);
	menuItem = new wxMenuItem(fileMenu_, ID_FILE_MENU_EXPORT, wxT("Export Textures"));
	//enuItem_->SetBitmap(IconManager::getSingleton().getIcon(IconManager::SAVE_AS));
	fileMenu_->Append(menuItem);
	fileMenu_->AppendSeparator();
	menuItem = new wxMenuItem(fileMenu_, ID_FILE_MENU_CLOSE, wxT("&Close"));
	//enuItem_->SetBitmap(IconManager::getSingleton().getIcon(IconManager::CLOSE));
	fileMenu_->Append(menuItem);
	menuItem = new wxMenuItem(fileMenu_, ID_FILE_MENU_EXIT, wxT("&Exit"));
	//enuItem_->SetBitmap(IconManager::getSingleton().getIcon(IconManager::CLOSE));
	fileMenu_->Append(menuItem);
	fileMenu_->UpdateUI();
	menuBar_->Append(fileMenu_, wxT("&File"));
}


/** Create the view menu for the top level window */
void ViewerFrame::createViewMenu() {
	viewMenu_ = new wxMenu();
	wxMenuItem *menuItem = new wxMenuItem(viewMenu_, ID_VIEW_MENU_FREE_CAMERA,
										  wxT("&Free Camera"),
										  wxT("Switch between free or orbital camera"),
										  wxITEM_CHECK);
	viewMenu_->Append(menuItem);
	menuItem = new wxMenuItem(viewMenu_, ID_VIEW_MENU_WIREFRAME, wxT("&Wireframe"),
							  wxT("Switch between Wireframe or solid"),
							  wxITEM_CHECK);
	viewMenu_->Append(menuItem);
	viewMenu_->UpdateUI();
	menuBar_->Append(viewMenu_, wxT("&View"));
	viewMenu_->Check(ID_VIEW_MENU_FREE_CAMERA, true);
}


/** Creates the tools menu */
void ViewerFrame::createToolsMenu() {
	toolMenu_ = new wxMenu();
	wxMenuItem* menuItem = new wxMenuItem(toolMenu_, ID_VERBOSE_LOGGING,
										  wxT("Verbose Output"),
										  wxT("Incredibly Verbose Info (slows export)"),
										  wxITEM_CHECK);
	toolMenu_->Append(menuItem);
	//toolMenu_->Check(ID_VERBOSE_LOGGING, Config::instance()->verbose_);
	PbLog.setOutputFunctor(aqua::LoggerOutputWx());
	PbLog.setLogLevel(Logger::LOG_WARNING);
	wxLog::SetLogLevel(wxLOG_Max);
	wxLog::SetVerbose(true);
	//wxLog::SetVerbose(Config::instance()->verbose_);
	menuItem = new wxMenuItem(toolMenu_, ID_DIRECTORIES_AND_PATHS,
							  wxT("Directories and &Paths"),
							  wxT("Configure system dirs"));
	toolMenu_->Append(menuItem);
	toolMenu_->UpdateUI();
	menuBar_->Append(toolMenu_, wxT("&Tools"));
	return;
}

//!< Description for createRenderWindow.
void ViewerFrame::createRenderWindow() {
	rendererPanel_->Initialise();
}

/**
 * Create the main pane for containing the aqua renderer
 * @return true if initialisation was successful
 */
bool ViewerFrame::createRendererPane() {
	rendererPanel_ = new RenderPanel(this);
	wxString caption(wxT("Renderer"));
	auiManager_->AddPane(rendererPanel_, wxCENTER, wxT("Renderer Pane"));
	return true;
}



/** Create the meta editor pane */
void ViewerFrame::createMetaEditorPane() {
	metaScenePanel_ = new MetaScenePanel(this, wxNewId(), wxDefaultPosition, wxDefaultSize);
	wxAuiPaneInfo metaScenePanelAuiInfo;
	metaScenePanelAuiInfo.Caption(wxT("Scene Meta Data"));
	metaScenePanelAuiInfo.Dockable();
	metaScenePanelAuiInfo.Floatable();
	metaScenePanelAuiInfo.Resizable();
	metaScenePanelAuiInfo.Right();
	metaScenePanelAuiInfo.BestSize(512,128);
	metaScenePanelAuiInfo.FloatingSize(512,128);
	metaScenePanelAuiInfo.HasBorder();
	metaScenePanelAuiInfo.Gripper();
	metaScenePanelAuiInfo.MaximizeButton(true);
	auiManager_->AddPane(metaScenePanel_, metaScenePanelAuiInfo);
	auiManager_->Update();
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
	sceneManager->setView(metaScenePanel_);
	aqua::Locator::checkIn(sceneManager);
	return;
}

/** Create the information pane where logging happens */
void ViewerFrame::createInformationPane() {
	informationNotebook_ = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
											 wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxNO_BORDER);
	logPanel_ = new LogPanel(informationNotebook_);
	wxLog::SetActiveTarget(logPanel_);
	wxLog::EnableLogging();
	informationNotebook_->AddPage(logPanel_, wxT("Playbox Log"));
	wxAuiPaneInfo info;
	info.Caption(wxT("Information"));
	info.MaximizeButton(true);
	info.BestSize(512, 128);
	info.Bottom();
	auiManager_->AddPane(informationNotebook_, info);
	wxLogMessage(wxT("Hello World!"));
	return;
}

/** Create the notebook for code tabs */
void ViewerFrame::createCodePane() {
	codeNotebook_ = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
									  wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxNO_BORDER);
	wxAuiPaneInfo info;
	info.Caption(wxT("Code"));
	info.MaximizeButton(true);
	info.BestSize(128, 128);
	info.Float();
	auiManager_->AddPane(codeNotebook_, info);
	return;
}

/** closedown: shut down the app, saving config info, killing metadata, etc */
void ViewerFrame::closedown()
{
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
	sceneManager->unloadScene();
	aqua::Locator::checkIn(sceneManager);
	codePanelCollection_->writeCollectionInfo();
	delete codePanelCollection_;
	codePanelCollection_ = 0;
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	config->Save();
	aqua::Locator::checkIn(config);
}

// -- MENU EVENT HANDLERS -----------------------------

/**
 * Invoked when File|New selected from the main menu
 * @param event Menu Event details
 */
void ViewerFrame::OnFileNew(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
	sceneManager->clearScene();
	aqua::Locator::checkIn(sceneManager);
	return;
}

/**
 * Invoked when File|Open selected from the main menu
 * @param event Menu event details
 */
void ViewerFrame::OnFileOpen(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
//	bool isCompressed = false;
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	wxFileName metaDirectory(config->projectDataDirectory(), wxPATH_UNIX);
	aqua::Locator::checkIn(config);
	wxFileDialog *fd = new wxFileDialog(this, wxT("Open a metadata file"), metaDirectory.GetFullPath(), wxEmptyString,	wxT("Playbox Compressed textual metadata (*.ptmgz)|*.ptmgz|Playbox textual metadata	 (*.ptm)|*.ptm|ALL files (*.*)|*.*"), wxFD_OPEN);
	if(fd->ShowModal() == wxID_OK)	{
		// TO DO :: Scene Panel Setup goes in here
	}
	delete fd;
	return;
}

// TODO : code collection needs to go, use wxAUI instead
/**
 * Invoked when File|Open Code	selected from the main menu
 * @param event Menu event detailse
 */
void ViewerFrame::OnFileOpenCode(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	wxFileName scriptsDirectory(config->projectScriptsDirectory(), wxPATH_UNIX);
	aqua::Locator::checkIn(config);
	wxFileDialog *fd = new wxFileDialog(this, wxT("Open A LUA File."), scriptsDirectory.GetFullPath(), wxEmptyString, wxT("LUA files (*.lua)|*.lua|ALL files (*.*)|*.*"), wxFD_OPEN);
	if(fd->ShowModal() == wxID_OK) {
		CodePanel  *cp	= codePanelCollection_->NewMember(wxPoint(40,40), wxSize(640, 480));
		cp->OpenFile(fd->GetPath());
		wxAuiPaneInfo info;
		info.Caption(wxT("Information"));
		info.MaximizeButton(true);
		info.BestSize(512, 128);
		info.Dockable();
		info.Dock();
		codeNotebook_->AddPage(cp, "Code");
		auiManager_->AddPane(codeNotebook_, info);
		cp->Show();
	}
	return;
}

/**
 * Invoked when File|Save selected from the main menu
 * @param event Menu event detailse
 */
void ViewerFrame::OnFileSave(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	// check to see if export is permitted
	bool allowExport = false;
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
	allowExport = sceneManager->isExportable();
	aqua::Locator::checkIn(sceneManager);
	if(!allowExport) {
		if(wxNO == wxMessageBox(wxT("Errors will mean export cannot complete successfully. Check the error log, fix and reload\nSee a Programmer if unclear as to what's wrong."), wxT("Continue Exporting?"), wxICON_ERROR | wxYES_NO, this)) {
			return;
		}
	}
	// intialise pre-requistes
	bool isCompressed = false;
	bool isBinary     = false;
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	wxFileName metaDirectory(config->projectRawMetaDirectory(), wxPATH_UNIX);
	aqua::Locator::checkIn(config);
	// prompt user for output
	wxFileDialog *fd = new wxFileDialog(this, wxT("Write a scene"), metaDirectory.GetFullPath(), wxEmptyString,
										wxT("Playbox textual metadata(*.ptm)|*.ptm|Playbox binary metadata(*.mdb)|*.mdb|ALL files (*.*)|*.*"),
										wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	// now do the outuput
	if(fd->ShowModal() == wxID_OK)	{
		if(wxFileName(fd->GetPath()).GetExt() == "ptmgz")
			isCompressed = true;
		if(wxFileName(fd->GetPath()).GetExt() == "mdb")
			isBinary = true;
		// set up directory / file names
		std::string exportPath(fd->GetPath().utf8_str());
		wxArrayString exportDirs(wxFileName(fd->GetPath()).GetDirs());
#ifdef PLAYBOX_PATH_POLICE
		if(exportDirs[exportDirs.GetCount()-1] != "Meta") {
			wxMessageBox(wxT("PTM files should be saved to a directory named Meta"), wxT("Playbox Path Police"), wxICON_ERROR | wxOK, this);
			return;
		}
#endif
		wxString tempPrefix("MetaData");
		wxString tempName(wxFileName::CreateTempFileName(tempPrefix,(wxFile*) NULL));
		if(ViewerFrame::headerDirectory_ == "Unknown")
			findMetaHeaderDirectory();
		// read the meta headers
		wxLogVerbose("Creating Metadata");
		const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
		const aqua::Locator::Service<LayerManager>& layerManager(aqua::Locator::checkOut<LayerManager>());
		// iterating over every object in the scene
		wxLogVerbose("Beginning object walk");
		Array< std::string > layerNames;
		sceneManager->getLayers(layerNames);
		if(layerNames.size() != 0) {
			// for each layer
			for(Array< std::string >::iterator l_it = layerNames.begin(); l_it != layerNames.end(); ++l_it) {
				Array< int > handle_array;
				sceneManager->getSceneObjectsInLayer(*l_it, handle_array);
				// write entities and components
				wxLogVerbose(wxString::Format("Exporting  %s ", l_it->c_str()));
				bool exported =	layerManager->exportLayer(*l_it);
				if(!exported) {
					wxLogVerbose(wxString::Format("Entity free layer %s.", l_it->c_str()));
				}
			}
			// check we can actually export (process might have lead to error)
			allowExport = sceneManager->isExportable();
			if(!allowExport) {
				wxMessageBox(wxT("Errors are preventing export: check the error log, fix and reload\nSee a Programmer if unclear as to what's wrong."), wxT("Playbox Error Police"), wxICON_ERROR | wxOK, this);
				//aqua::Locator::checkIn(sceneManager);
				//aqua::Locator::checkIn(layerManager);
				//return;
			}
		} else {
			wxLogError("No	to export.");
		}
		// write the file
		if (isBinary) {
			DynamicMetaData md;
			sceneManager->copyData(md);
			md.WriteBinary(tempName);
		} else {
			MetaData_WA mdWA;
			sceneManager->copyData(mdWA);
			mdWA.Write(tempName);
			if(isCompressed) {
				wxFileInputStream ins(tempName.fn_str());
				wxFileOutputStream outs(exportPath.c_str());
				// TODO: Make compression level UI adjustable?
				wxZlibOutputStream zouts(outs, 3, wxZLIB_GZIP);
				ins.Read(zouts);
				// this is RAII so closes on destruction.
			} else {
				wxRenameFile(tempName, exportPath.c_str());
			}
		}
		wxLogVerbose("Releasing metadata.");
		wxMessageBox(wxT("Successful Export."), wxT("Playbox Viewer"), wxICON_INFORMATION | wxOK, this);
		aqua::Locator::checkIn(sceneManager);
		aqua::Locator::checkIn(layerManager);
	}
	return;
}

/**
 * Invoked when File|Import selected from the main menu
 * @param event menu event
 */
void ViewerFrame::OnFileImport(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	wxString fbxDirectory = config->projectArtDirectory();
	aqua::Locator::checkIn(config);
	wxFileDialog *fd = new wxFileDialog(this, wxT("Import An Autodesk File."), fbxDirectory, wxEmptyString, wxT("FBX files (*.fbx)|*.fbx|Collada (*.dae)|*.dae|ALL files (*.*)|*.*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if(fd->ShowModal() == wxID_OK) {
		std::string importPath(fd->GetPath().utf8_str());
		wxFileName importName(fd->GetPath());
		std::string fileName(importName.GetName().utf8_str());
		workingDir_ = wxString(importName.GetPath());
		wxWindowID scenePanelId = wxNewId();
		ScenePanel* scenePanel = new ScenePanel(this, scenePanelId);
		wxAuiPaneInfo scenePanelAuiInfo;
		scenePanelAuiInfo.Caption(importName.GetName());
		scenePanelAuiInfo.Dockable();
		scenePanelAuiInfo.Floatable();
		scenePanelAuiInfo.Resizable();
		scenePanelAuiInfo.Left();
		scenePanelAuiInfo.BestSize(512,128);
		scenePanelAuiInfo.FloatingSize(512,128);
		scenePanelAuiInfo.HasBorder();
		scenePanelAuiInfo.Gripper();
		scenePanelAuiInfo.MaximizeButton(true);
		auiManager_->AddPane(scenePanel, scenePanelAuiInfo);
		scenePanel->SetScene(importPath);
		scenePanelIds_.push_back(scenePanelId);
		auiManager_->Update();
	}
	return;
}

/**
 * Invoked when File|Export selected from the main menu
 * @param event menu event
 */
void ViewerFrame::OnFileExport(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	const aqua::Locator::Service<TextureDefinitionManager>& texDefs(aqua::Locator::checkOut<TextureDefinitionManager>());
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
	
	wxFileDialog *fd = new wxFileDialog(this, wxT("Write a defintion file"), config->projectTexturesDirectory(), wxEmptyString, wxT("Texture defintion file  (*.txt)|*.txt|ALL files (*.*)|*.*"), wxFD_SAVE);
	if (fd->ShowModal() == wxID_OK)	{
		std::string exportPath(fd->GetPath().utf8_str());
		wxArrayString exportDirs(wxFileName(fd->GetPath()).GetDirs());
		if (exportDirs[exportDirs.GetCount()-1].CmpNoCase("Textures") != 0) {
			wxMessageBox(wxT("Texture definition files should be saved to a directory named Textures"), wxT("Playbox Path Police"), wxICON_INFORMATION | wxOK, this);
		}
		DynamicMetaData md;
		sceneManager->copyData(md);		
		texDefs->writeOutputFile(exportPath, &md);
		texDefs->clear();
	}
	aqua::Locator::checkIn(sceneManager);
	aqua::Locator::checkIn(texDefs);
	aqua::Locator::checkIn(config);

}


/**
 * Invoked when Close File is selected from the frame menu
 * @param event Menu event
 */
void ViewerFrame::OnFileClose(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
}


/**
 * Invoked when File| Exit selected from the frame menue
 * @param event menu event details
 */
void ViewerFrame::OnFileExit(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	closedown();
	Destroy();
	return;
}

void ViewerFrame::OnViewFreeCamera(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	return;
}

void ViewerFrame::OnViewWireframe(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
}

/**
 * Description for OnDirectoriesAndPaths.
 * @param event Menu Event
 */
void ViewerFrame::OnDirectoriesAndPaths(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	PreferencesDialog* prefDlg = new PreferencesDialog(this, wxID_ANY, wxT("Preferences Dialog"));
	prefDlg->ShowModal();
	delete prefDlg;
	return;
}

/**
 * Called when the user clicks something on the scene panel that causes a scene change
 * @param event Not used
 */
void ViewerFrame::OnSceneChange(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	wxLogDebug("On Scene Change");
	// TO DO - need way of mapping panel id to scene name
	//wxWindowID scenePanelId = event.GetId();
	SceneClientData *info = dynamic_cast<SceneClientData*>(event.GetClientObject());
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
	sceneManager->setParser(info->getParser());
	if(info != NULL) {
		if(info->isLayer()) {
			FbxDisplayLayer* layer(info->getLayer());
			std::vector<FbxNode*> nodes;
			info->getParser()->getLayerNodes(layer, nodes);
			for(std::vector<FbxNode*>::iterator node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
				sceneManager->addNodeToScene(*node_it);
			}
		}
		if(info->isNode()) {
			FbxNode* node = info->getNode();
			sceneManager->addNodeToScene(node);
		}
	}
	sceneManager->refreshView();
	aqua::Locator::checkIn(sceneManager);
	return;
}

/**
 * Passed to the frame by the viewer panel when the user wants to clear the scene completely
 * @param event Not used
 */
void ViewerFrame::OnSceneClear(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	wxLogDebug("On Scene Clear");
	// TO DO - need way of mapping panel id to scene name
	//wxWindowID scenePanelId = event.GetId();
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
	sceneManager->clearScene();
	aqua::Locator::checkIn(sceneManager);
}

/**
 * Load metata into aqua data managers from scene manager
 * @param event <doc>
 */
void ViewerFrame::OnSceneLoad(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	wxLogDebug("On Scene Load");
	// TO DO - need way of mapping panel id to scene name
	//wxWindowID scenePanelId = event.GetId();
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
	//if (sceneManager->isExportable()) {
		sceneManager->loadScene();
		rendererPanel_->focusOnSceneExtents();
	//}
	aqua::Locator::checkIn(sceneManager);
}

/*
 * Unload metata from aqua data managers
 */
void ViewerFrame::OnSceneUnload(wxCommandEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	wxLogDebug("On Scene UnLoad");
	// TO DO - need way of mapping panel id to scene name
	//wxWindowID scenePanelId = event.GetId();
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
	sceneManager->unloadScene();
	aqua::Locator::checkIn(sceneManager);
}

/**
 * Closing the main frame. Means the application is saying goodbye
 * @param event <doc>
 */
void ViewerFrame::OnClose(wxCloseEvent& event) {
	PBUNREFERENCED_PARAMETER(event);
	closedown();
	Destroy();
}




