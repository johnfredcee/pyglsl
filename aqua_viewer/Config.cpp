#include "StdAfx.h"

#include <wx/config.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/fileconf.h>
#include <wx/wfstream.h>

#include "Config.h"

wxString iniFile("AquaViewer.ini");

Config::Config()  {

	wxString defaultProjectDir;


	wxFileInputStream configFile(iniFile);
	if (configFile.IsOk())
		config_ = new wxFileConfig(configFile);
	else
		config_ = new wxFileConfig;

	if (!wxGetEnv(wxT("PB_PROJECT"), &defaultProjectDir)) {
		defaultProjectDir = wxString("W:") + wxFileName::GetPathSeparator(wxPATH_UNIX) + wxString(wxT("prototype_template/Projects"));
	}

	if (!config_->Read("ProjectDirectory", &projectDirectory_)) {
		projectDirectory_ = defaultProjectDir;
	}

	if (!config_->Read("ProjectName", &projectName_)) {
		projectName_ = "Simple3dExample";
	}

	if (!config_->Read("MetaDirectory", &metaDirectory_)) {
		metaDirectory_ = defaultProjectDir;
	}

	if (!config_->Read("ArtDirectory", &artDirectory_)) {
		artDirectory_ = defaultProjectDir;
	}

	if (!config_->Read("TextureDirectory", &textureDirectory_)) {
		textureDirectory_ = defaultProjectDir;
	}

	// TODO: Not sure any of the below have meaning
	if (!config_->Read("useShadowmaps", &useShadowmaps_)) {
		useShadowmaps_ = true;
	};

	if (!config_->Read("useTGAS", &useTGAS_)) {
		useTGAS_ = false;
	};

	if (!config_->Read("filterInstances", &filterInstances_)) {
		filterInstances_ = true;
	};

	if (!config_->Read("addHelpers", &addHelpers_)) {
		addHelpers_ = false;
	};

	if (!config_->Read("originReset", &originReset_)) {
		originReset_ = false;
	};

	if (!config_->Read("retainScale", &retainScale_)) {
		retainScale_ = false;
	}

	if (!config_->Read("retainRotation", &retainRotation_)) {
		retainRotation_ = false;
	}

	if (!config_->Read("bakeAll", &bakeAll_)) {
		bakeAll_ = false;
	}

	if (!config_->Read("hidePhysics", &hidePhysics_)) {
		hidePhysics_ = true;
	}

	if (!config_->Read("redundantAlphas", &redundantAlphas_)) {
		redundantAlphas_  = true;
	}

	if (!config_->Read("verbose", &verbose_)) {
		verbose_  = true;
	}

	if (!config_->Read("optimizeMesh", &optimizeMesh_)) {
		optimizeMesh_  = false;
	}

	if (!config_->Read("logStats", &logStats_)) {
		logStats_  = false;
	}

	wxString exePath = wxStandardPaths::Get().GetExecutablePath();
	wxString xmlFile = wxFileName::GetCwd() + wxString("\\layerNames.xml");
	if (!config_->Read("configFile", &configFile_)) {
		configFile_	 = xmlFile;
	}

}

void Config::Save() {

	config_->Write("ProjectDirectory", projectDirectory_);
	config_->Write("ProjectName", projectName_);
	config_->Write("MetaDirectory", metaDirectory_);
	config_->Write("ArtDirectory", artDirectory_);
	config_->Write("TextureDirectory", textureDirectory_);
	config_->Write("useShadowmaps", useShadowmaps_);
	config_->Write("useTGAS", useTGAS_);
	config_->Write("filterInstances", filterInstances_);
	config_->Write("addHelpers", addHelpers_);
	config_->Write("originReset", originReset_);
	config_->Write("retainScale", retainScale_);
	config_->Write("retainRotation", retainRotation_);
	config_->Write("bakeAll", bakeAll_);
	config_->Write("hidePhysics", hidePhysics_);
	config_->Write("redundantAlphas", redundantAlphas_);
	config_->Write("verbose", verbose_);
	config_->Write("optimizeMesh", optimizeMesh_);
	wxFileOutputStream configFile(iniFile);
	config_->Save(configFile);

}

Config::~Config()  {
	delete config_;
}


bool Config::Read(const wxString& key, wxString* str) const {
	return config_->Read(key, str);
}

bool Config::Read(const wxString& key, wxString* str, const wxString& defaultVal) const {
	return config_->Read(key, str, defaultVal);
}

wxString Config::Read(const wxString& key, const wxString& defaultVal) const {
	return config_->Read(key, defaultVal);
}

bool Config::Read(const wxString& key, long* l) const {
	return config_->Read(key, l);
}

bool Config::Read(const wxString& key, long* l, long defaultVal) const {
	return config_->Read(key, l, defaultVal);
}

long Config::Read(const wxString& key, long defaultVal) const {
	return config_->Read(key, defaultVal);
}

bool Config::Write(const wxString& key, const wxString& value) {
	return config_->Write(key, value);
}

bool Config::Write(const wxString& key, long value) {
	return config_->Write(key, value);
}

bool Config::Write(const wxString& key, double value) {
	return config_->Write(key, value);
}

bool Config::Write(const wxString& key, bool value) {
	return config_->Write(key, value);
}

void Config::SetPath(const wxString& strPath) {
	return config_->SetPath(strPath);
}


wxString Config::projectDirectory() const
{
	return projectDirectory_ + wxFileName::GetPathSeparator(wxPATH_UNIX) + projectName_ + wxFileName::GetPathSeparator(wxPATH_UNIX);
}


wxString Config::projectArtDirectory() const
{
	wxString artDirName(wxT("art"));
	wxString result;

	//result	= projectDirectory_ +  wxFileName::GetPathSeparator(wxPATH_UNIX);
	result  = artDirectory_;		//+  wxFileName::GetPathSeparator(wxPATH_UNIX);
	return result;
}

wxString Config::projectScriptsDirectory() const
{
	wxString dataDirName(wxT("data"));
	wxString scriptsDirName(wxT("Scripts"));
	wxString result;

	result = projectDirectory_ +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += projectName_	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += dataDirName	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += scriptsDirName   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
 return result;
}

wxString Config::projectRawDirectory() const
{
	wxString rawDirName(wxT("exports"));
	wxString dataDirName(wxT("data"));
	wxString result;
	result = projectDirectory_ +   wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += projectName_	   +   wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += dataDirName	   +   wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += rawDirName	   +   wxFileName::GetPathSeparator(wxPATH_UNIX);
	return result;
}

wxString Config::projectDataDirectory() const
{
	wxString rawDirName(wxT("processed"));
	wxString dataDirName(wxT("data"));
	wxString result;
	result = projectDirectory_ +   wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += projectName_	   +   wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += dataDirName	   +   wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += rawDirName	   +   wxFileName::GetPathSeparator(wxPATH_UNIX);
	return result;
}

// TODO: Can we have meta back?
wxString Config::projectRawMetaDirectory() const
{
	wxString rawDirName(wxT("raw"));
	wxString globalDirName(wxT("Global"));
//		wxString metaDirName(wxT("Meta"));

	wxString result;
	result = projectDirectory_ +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += projectName_	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += rawDirName	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += globalDirName	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
//		result += metaDirName	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	return result;
}

wxString Config::projectShadersDirectory() const
{
//	wxString rawDirName(wxT("raw"));
//	wxString globalDirName(wxT("Global"));
	wxString shaderDirName(wxT("shaders"));
	wxString result;
//	result = projectDirectory_ +	wxFileName::GetPathSeparator(wxPATH_UNIX);
//	result += projectName_	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
//	result += rawDirName	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
//	result += globalDirName	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	result = wxString(wxT("viewer"))    +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += shaderDirName	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	return result;
}

wxString Config::projectTexturesDirectory() const
{
	wxString rawDirName(wxT("raw"));
	wxString globalDirName(wxT("Global"));
	wxString texturesDirName(wxT("Textures"));
	wxString result;
	result = projectDirectory_ +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += projectName_	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += rawDirName	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += globalDirName	   +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += texturesDirName	 +	wxFileName::GetPathSeparator(wxPATH_UNIX);
	return result;
}

wxString Config::projectMissingTexture() const
{
	wxString rawDirName(wxT("raw"));
	wxString globalDirName(wxT("Global"));
	wxString texturesDirName(wxT("Textures"));
	wxString artDirName(wxT("art"));
	wxString missingName(wxT("missing.tga"));

	wxString result;
	result = projectDirectory_ +  wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += projectName_	   +  wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += rawDirName	   +  wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += globalDirName	   +  wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += texturesDirName  +  wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += artDirName	   +  wxFileName::GetPathSeparator(wxPATH_UNIX);
	result += missingName;
	return result;
}

wxString Config::metaDirectory() const
{
	return metaDirectory_;
}

