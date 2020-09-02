#if !defined(CONFIG_H_INCLUDED)
#define CONFIG_H_INCLUDED

#include "Locator.h"

class PreferencesDialog;
class wxFileConfig;

class Config :	public aqua::Locator::ServiceSingleton<Config>
{
		friend class aqua::Locator::ServiceSingleton<Config>;
		friend class PreferencesDialog;

	public:
		void Save();

		bool Read(const wxString& key, wxString* str) const;

		bool Read(const wxString& key, wxString* str, const wxString& defaultVal) const; 

		wxString Read(const wxString& key, const wxString& defaultVal) const; 

		bool Read(const wxString& key, long* l) const; 

		bool Read(const wxString& key, long* l, long defaultVal) const; 

		long Read(const wxString& key, long defaultVal) const; 

		bool Write(const wxString& key, const wxString& value); 

		bool Write(const wxString& key, long value); 

		bool Write(const wxString& key, double value); 

		bool Write(const wxString& key, bool value); 

		void SetPath(const wxString& strPath); 

		wxString projectDirectory() const;		

		wxString projectArtDirectory() const;		

		wxString projectScriptsDirectory() const;		

		wxString projectRawDirectory() const;		

		wxString projectDataDirectory() const;		

		// TODO: Can we have meta back?
		wxString projectRawMetaDirectory() const;		

		wxString projectShadersDirectory() const;		

		wxString projectTexturesDirectory() const;		

		wxString projectMissingTexture() const;		

		wxString metaDirectory() const;		

		wxFileConfig*	config_;

		wxString		configFile_;

	private:
		wxString		projectDirectory_;
		wxString		projectName_;
		wxString		metaDirectory_;
		wxString		artDirectory_;
		wxString		textureDirectory_;
		bool			useShadowmaps_;
		bool			useTGAS_;
		bool			filterInstances_;
		bool			addHelpers_;
		bool			originReset_;
		bool			retainScale_;
		bool			retainRotation_;
		bool			bakeAll_;
		bool			hidePhysics_;
		bool			redundantAlphas_;
		bool			optimizeMesh_;
		bool			verbose_;
		bool			logStats_;

		Config();
		virtual ~Config();
};

#endif

