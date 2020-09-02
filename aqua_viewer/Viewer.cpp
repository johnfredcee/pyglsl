
#include "StdAfx.h"

// gets in the way when debugging
#ifndef NDEBUG
#define NO_STUPID_SPLASHSCREEN
#endif

#include <wx/xml/xml.h>
#include <wx/splash.h>
#include <wx/mstream.h>

// playbox includes
#include <aqua/Material.h>
#include <aqua/VertexBufferData.h>
#include <aqua/VertexIndexBufferData.h>
#include <aqua/SkeletonBufferData.h>
#include <GraphicsSystem/GraphicsSystem.h>
#include <Managers/ShaderManager.h>
#include <Shader/ShaderCache.h>

// this is last because Playbox uses GLee, which wants to be the first
// to pull in Opengl
#include <wx/glcanvas.h>

// pull in splash screen data
#include "splash.png.h"


// viewer includes
#include "GraphicObject.h"
#include "OpenGLPanel.h"
#include "Config.h"
#include "Viewer.h"
#include "ViewerFrame.h"
#include "IntermediateMesh.h"
#include "IMesh.h"
#include "GraphicObjectNameManager.h"
#include "TextureDefinitionManager.h"
#include "ShaderNameManager.h"
#include "MetaBufferManager.h"
#include "MetaMaterialManager.h"
#include "MetaBitmapManager.h"
#include "SceneManager.h"
#include "LayerManager.h"
#include "PreferencesDialog.h"

KFbxSdkManager* fbxSDKManager = 0;
unsigned int	g_REVISION	 =	2;

IMPLEMENT_APP(ViewerApp)

wxWindowID ID_MAIN_FRAME = wxNewId();

/** Destructor. Call this for post GUI cleanup */
ViewerApp::~ViewerApp()
{
    //TODO : need to be sure scene can be unloaded
	aqua::Locator::removeService<aqua::ShaderCache>();
	aqua::Locator::removeService<aqua::ShaderManager>();
	aqua::Locator::removeService<aqua::MaterialManager>();
	aqua::Locator::removeService<aqua::TextureManager>();	
	aqua::Locator::removeService<aqua::GraphicsSystem>();
}

/** Read in the related .xml file for setting up the viewer */
bool ViewerApp::SetConfiguration()
{
	// now find the xml file
	wxXmlDocument doc;
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());	
	wxString xmlFile = config->configFile_;
	if (!wxFileName::FileExists(xmlFile)) {
		wxFileDialog *fd = new wxFileDialog(NULL, wxT("Browse for config"), wxEmptyString, wxEmptyString,
										wxT("Xml (*.xml)|*.xml|ALL files (*.*)|*.*"),
										wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	    fd->ShowModal();										
		config->configFile_ = fd->GetPath();
		xmlFile = config->configFile_;
	}
	wxString projectPath = config->projectDirectory();
	if (!wxFileName::DirExists(projectPath)) {
		PreferencesDialog* prefDlg = new PreferencesDialog(NULL, wxID_ANY, wxT("Preferences"));
		prefDlg->ShowModal();
		delete prefDlg;
	}
	if(!doc.Load(xmlFile)) {
		wxMessageBox(wxString::Format(wxT("Unable to parse layerNames.xml in %s."), config->projectDirectory()), wxT("Layers"), wxOK | wxICON_ERROR);
		aqua::Locator::checkIn(config);
		return false;
	} 
	aqua::Locator::checkIn(config);	
	if(doc.GetRoot()->GetName() == "playbox")  {
		wxXmlNode *child = doc.GetRoot()->GetChildren();
		while(child) {
			if(child->GetName() == "layer") {
				// check out the layer manager
				const aqua::Locator::Service<LayerManager>& layerManager(aqua::Locator::checkOut<LayerManager>());
				// parse the layer
				wxString layerName = child->GetAttribute("name", "unknown");
				wxString layerType = child->GetAttribute("type", "unknown");
				wxString layerObject = child->GetAttribute("method", "unknown");
				wxString regexFilter = child->GetAttribute("filter", "");
				if((layerName != "unknown") && (layerType != "unknown")) {
					layerManager->addLayer(std::string(layerName.c_str()),
										   std::string(layerType.c_str()),
										   layerManager->newExporterObject(layerObject),
										   std::string(regexFilter.c_str()));
				}
				aqua::Locator::checkIn(layerManager);																				
			} else {
				if(child->GetName() == "shader") {
					const aqua::Locator::Service<ShaderNameManager> &shaderManager(aqua::Locator::checkOut<ShaderNameManager>());				
					std::string shaderName(child->GetAttribute("name").utf8_str());
					std::string formatsAllowed(child->GetAttribute("elementsAllowed","").utf8_str());
					std::string formatsForbidden(child->GetAttribute("elementsForbidden", "").utf8_str());
					aqua::Uint32	allowed	  = parseFormatDescription(formatsAllowed);
					aqua::Uint32	forbidden = parseFormatDescription(formatsForbidden);
					wxXmlNode *grandchild = child->GetChildren();
					ShaderNameManager::TextureSlotVector slots;
					if ((grandchild) && (grandchild->GetName() == "textures")) {
						wxXmlNode *greatgrandchild = grandchild->GetChildren();
						while(greatgrandchild) {
							std::string alpha;
							if(greatgrandchild->GetName() == "slot") {
								alpha = std::string(greatgrandchild->GetAttribute("alpha", "ANY").utf8_str());
								if((alpha != "ANY") && (alpha != "SPECULAR") && (alpha != "TRANSPARENT")) {
									aqua::Locator::checkIn(shaderManager);	
									goto OhBumAnXMLError;
								}
							}
							slots.push_back(parseTextureSlotDescription(alpha));
							greatgrandchild = greatgrandchild->GetNext();
						}
					}					
					shaderManager->addShaderName(shaderName, allowed, forbidden, slots);
					aqua::Locator::checkIn(shaderManager);
				} else
					goto OhBumAnXMLError;
			}
			child=child->GetNext();
		}
	} else {
		goto OhBumAnXMLError;
	}
	goto ItWentOk;
OhBumAnXMLError:
	wxMessageBox(wxT("XML Parsing of layerName.xml failed"), "Layers", wxOK | wxICON_ERROR);
	aqua::Locator::checkIn(config);
	return false;
ItWentOk:
	aqua::Locator::checkIn(config);
	return true;
}

/**
 * Called when the app starts up, but before the main loop.
 * @return true if the app is set up correctly
 */
bool ViewerApp::OnInit()
{
	wxInitAllImageHandlers();
	aqua::Locator::registerService<Config>("Config");
	aqua::Locator::registerService<LayerManager>("LayerManager");
	aqua::Locator::registerService<SceneManager>("SceneManager");
	aqua::Locator::registerService<MetaBitmapManager>("MetaBitmapManager");
	aqua::Locator::registerService<MetaMaterialManager>("MetaMaterialManager");
	aqua::Locator::registerService<MetaBufferManager>("MetaBufferManager");
	aqua::Locator::registerService<ShaderNameManager>("ShaderManagerTools");
  	aqua::Locator::registerService<TextureDefinitionManager>("TextureDefinitionManager");
 	aqua::Locator::registerService<GraphicObjectNameManager>("GraphicObjectNameManager");
	aqua::Locator::registerService<aqua::GraphicsSystem>("GraphicsSystem");
	aqua::Locator::registerService<aqua::TextureManager>("TextureManager");
	aqua::Locator::registerService<aqua::MaterialManager>("MaterialManager");
	aqua::Locator::registerService<aqua::ShaderManager>("ShaderManager");	
	aqua::Locator::registerService<aqua::ShaderCache>("ShaderCache");

//	wxMemoryInputStream istream(splash_png, sizeof(splash_png));
//	wxImage::AddHandler(new wxTGAHandler);
//	wxImage splash(istream, wxBITMAP_TYPE_PNG);
//	wxBitmap bitmap(splash);
//	wxSplashScreen* splashScreen = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 2000, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER | wxSTAY_ON_TOP);

	// Set up the config
	SetConfiguration();
	// Set up fbx
	fbxSDKManager = KFbxSdkManager::Create();
	// set up the main frame
	ViewerFrame* mainFrame = new ViewerFrame(NULL, ID_MAIN_FRAME, wxT("Playbox Viewer"), wxDefaultPosition,	 wxSize(1024, 768), wxBORDER_DOUBLE | wxMAXIMIZE | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCAPTION | wxCLIP_CHILDREN | wxSYSTEM_MENU | wxCLOSE_BOX );
	SetTopWindow(mainFrame);
	mainFrame->Show();
	mainFrame->createRenderWindow();
	return true;
}

/**
 * Close down the viewer
 * @return non-zero if something went wrong
 */
int ViewerApp::OnExit()
{
	aqua::Locator::removeService<GraphicObjectNameManager>();
	aqua::Locator::removeService<TextureDefinitionManager>();
	aqua::Locator::removeService<ShaderNameManager>();
	aqua::Locator::removeService<MetaBufferManager>();
	aqua::Locator::removeService<MetaMaterialManager>();
	aqua::Locator::removeService<MetaBitmapManager>();
	aqua::Locator::removeService<SceneManager>();
	aqua::Locator::removeService<LayerManager>();
	aqua::Locator::removeService<Config>();

	if(fbxSDKManager)
		fbxSDKManager->Destroy();

	return 0;
}

