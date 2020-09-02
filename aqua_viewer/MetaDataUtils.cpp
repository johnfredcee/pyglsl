#include "StdAfx.h"
#include "Config.h"
#include "SceneManager.h"
#include "MetaDataUtils.h"

/** Look for the meta_header directory by scanning upward */
void findMetaHeaderDirectory() {
	// TO DO : Pull in latest meta_headers!
	wxLogVerbose("Seeing meta data header dir");
	const aqua::Locator::Service<Config>& config(aqua::Locator::checkOut<Config>());
	if ((!config->metaDirectory().IsEmpty()) && (wxDir::Exists(config->metaDirectory()))) {
		SceneManager::headerDirectory_ = config->metaDirectory();
		aqua::Locator::checkIn(config);
		return;
	}
	aqua::Locator::checkIn(config);
	// TODO: Probaly want to promt the user for meta directory
	wxString dirString = "..";
	for (int i = 0; i < 4; i++) {
		wxLogVerbose(wxString::Format("Seeing meta data header dir %s", dirString + "\\meta_headers_aqua"));
		if (wxDir::Exists(dirString + "\\meta_headers_aqua")) {
			SceneManager::headerDirectory_ = dirString + "\\meta_headers_aqua";
			wxLogVerbose("Found Meta Headers in %s ", dirString.fn_str());
			return;
		}
		dirString = dirString + "\\..";
	}
	wxMessageBox(wxT("Unable to find meta header directory."), wxT("MetaHeaders?"), wxICON_INFORMATION | wxOK);
	wxExit();
}

/** Grab the latest meta_headers directory from svn */
void updateMetaDataHeaders() {
	wxLogVerbose("Updating metadata headers ");
	wxArrayString output;

	/*
	  wxArrayString errorOutput;
	  if (wxExecute(wxT("TortoiseProc.exe /command:update /closeonend:3	 /path:") + SceneManager::headerDirectory_, output, errorOutput, wxEXEC_SYNC) >= 0) {
	  // we are executing !!!!!!!!!!!
	  } else {
	  wxMessageBox(wxT("Could not find Tortise svn on your hard drive. Please install."), wxT("SVN Missing"), wxICON_INFORMATION | wxOK | wxCANCEL);
	  }
	  for (int i = 0; i < output.GetCount(); ++i) {
	  wxLogVerbose(output[i]);
	  }
	  for (int i = 0; i < errorOutput.GetCount(); ++i) {
	  wxLogError(errorOutput[i]);
	  }
	*/
	return;
}

/**
 * Actually read in meta headers with type definitons that apply to the viewers data
 * @param md Metadata object
 * @param subDirectory Directory that header files live in
 */
void readMetaDataHeaders(DynamicMetaData& md, const wxString subDirectory) {
	wxLogVerbose("Reading meta subdirectory %s ",subDirectory.c_str());
	wxString metaHeader;
	wxDir headerDirectory;
	headerDirectory.Open(SceneManager::headerDirectory_ + "\\" + subDirectory);
	bool cont = headerDirectory.GetFirst(&metaHeader, "*.h");
	while (cont) {
		wxLogVerbose(wxString::Format("Found %s.", metaHeader));
		wxString headerFileName(wxString::Format("%s\\%s", headerDirectory.GetName(), metaHeader));
		md.ReadHeader(headerFileName.utf8_str());
		cont = headerDirectory.GetNext(&metaHeader);
	}
	return;
}
