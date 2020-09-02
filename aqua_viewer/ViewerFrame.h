#ifndef FRAMEWORKFRAME_H
#define FRAMEWORKFRAME_H


#include <vector>

class wxAuiManager;
class wxAuiNotebook;
class wxNotebook;
class wxPropertyGridManager;
class wxTreeCtrl;

class DocPanel;
class EventArgs;
class LogPanel;
class OptionsPanel;
class ScenePanel;
class PropertiesPanel;
class ResourcePanel;
class WorkspacePanel;
class MeshMaker;
class ViewerApp;
class CodePanelCollection;
class wxConfig;
class OpenGLPanel;
class RenderPanel;
class CodePanelCollection;
class MetaEditorPanel;
class MetaScenePanel;

class ViewerFrame: public wxFrame
{
	public:

		ViewerFrame(wxWindow* parent, int id, const wxString& title,
					const wxPoint& pos=wxDefaultPosition,
					const wxSize& size=wxDefaultSize,
					long style=wxDEFAULT_FRAME_STYLE);

		virtual ~ViewerFrame();

		friend class ViewerApp;
		static ViewerFrame* getViewerFrame();
		CodePanelCollection* ViewerFrame::getCodePanelCollection();

		void OnSceneChange(wxCommandEvent& event);
		void OnSceneClear(wxCommandEvent& event);
		void OnSceneLoad(wxCommandEvent& event);
		void OnSceneUnload(wxCommandEvent& event);

		static wxString  headerDirectory_;

	protected:
		void createFileMenu();
		void createViewMenu();
		void createMenuBar();
		void createAuiManager(void);
		bool createRendererPane(void);
		void createInformationPane(void);
		void createOptionsPane(void);
		void createRenderWindow(void);
		void createToolsMenu();
		void createMetaEditorPane();
		void createCodePane();
		void closedown();
		void OnFileNew(wxCommandEvent& event);
		void OnFileOpen(wxCommandEvent& event);
		void OnFileSave(wxCommandEvent& event);
		void OnFileOpenCode(wxCommandEvent& event);
		void OnFileImport(wxCommandEvent& event);
		void OnFileExport(wxCommandEvent& event);
		void OnFileClose(wxCommandEvent& event);
		void OnFileExit(wxCommandEvent& event);
		void OnViewFreeCamera(wxCommandEvent& event);
		void OnViewWireframe(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnDirectoriesAndPaths(wxCommandEvent& event);

		wxMenuBar*			menuBar_;
		wxMenu*				fileMenu_;
		wxMenu*				viewMenu_;
		wxMenu*				toolMenu_;
		wxAuiNotebook*		informationNotebook_;
		wxAuiNotebook*		optionsNotebook_;
		wxAuiNotebook*		shadersNotebook_;
		wxAuiNotebook*      codeNotebook_;
		LogPanel*			logPanel_;
		LogPanel*			importLogPanel_;
		OptionsPanel*		optionsPanel_;
		RenderPanel*		rendererPanel_;
		ScenePanel*			scenePanel_;
		MetaScenePanel*		metaScenePanel_;

		wxAuiManager*		 auiManager_;
		wxString			 workingDir_;
		CodePanelCollection* codePanelCollection_;

		std::vector<wxWindowID>	 scenePanelIds_;
		DECLARE_EVENT_TABLE();

};


#endif // MESHMIXERFRAME_H

