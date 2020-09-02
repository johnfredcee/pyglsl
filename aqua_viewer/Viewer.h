
#if !defined(H_FRAMEWORKAPP)
#define H_FRAMEWORKAPP

extern KFbxSdkManager* fbxSDKManager;

class wxConfigBase;
class wxConfig;
class ViewerFrame;

/** ViewerApp: main class with the entry and exit points for the app */
class ViewerApp: public wxGLApp
{
	public:
		// we don't explicitly use this
		virtual ~ViewerApp();

	private:
		/** Cached pointer to the main frame. */
		ViewerFrame* mainWindow_;
		
		bool SetConfiguration();
		virtual bool OnInit();
		virtual int  OnExit();

};

#endif


