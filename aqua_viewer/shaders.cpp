#include "StdAfx.h"
#include <wx/arrstr.h>
#include <wx/dir.h>

#include <Shader/ShaderProgram.h>
#include <Shader/ShaderCache.h>
#include <Managers/MaterialManager.h>
#include <Managers/ShaderManager.h>
#include "Config.h"
#include "Shaders.h"
#include "ModelViewProjectionParameterUpdater.h"

void read_shaders_directory(wxArrayString& shaderNames)
{
	const aqua::Locator::Service<Config> config(aqua::Locator::checkOut<Config>());	
	std::string shaderPath = config->projectShadersDirectory();
	aqua::Locator::checkIn(config);
	wxDir shaderDir(shaderPath);
	if (!shaderDir.IsOpened()) {
		wxLogError(wxString::Format(wxT("Unable to scan shader directory %s "), shaderPath.c_str()));
		PBINSTANTBREAK();
	}
	wxString shaderFile;
	if (shaderDir.GetFirst(&shaderFile, wxT("*.vert"), wxDIR_FILES)) {
		do {
			wxFileName shaderFileName(shaderFile);
			shaderNames.Add(shaderFileName.GetName());
		} while (shaderDir.GetNext(&shaderFile));		
	}
	return;
}

void initialise_shaders() {
// Init the Shader Manager
	const aqua::Locator::Service<aqua::ShaderManager> shaderManager(aqua::Locator::checkOut<aqua::ShaderManager>());
	shaderManager->initialise();

// TODO: *iterate over the shaders directory* and load shaders
// Push the ModelViewProjectionParameterUpdater to the Shader Cache
	const aqua::Locator::Service<aqua::ShaderCache>& shaderCache(aqua::Locator::checkOut<aqua::ShaderCache>());
	shaderCache->addUpdater(aqua::shared_ptr<ModelViewProjectionParameterUpdater>(PBNEW(MEM_DEFAULT) ModelViewProjectionParameterUpdater));
	aqua::Locator::checkIn(shaderCache);
	const aqua::Locator::Service<Config> config(aqua::Locator::checkOut<Config>());	
	std::string shaderPath = config->projectShadersDirectory();
	aqua::Locator::checkIn(config);
	aqua::Locator::checkIn(shaderCache);	
	wxArrayString shaders;
	read_shaders_directory(shaders);
	wxLogDebug(wxString::Format(wxT("Found %d shaders"), shaders.Count()));
	for(aqua::Uint i = 0; i < shaders.Count(); i++)
	{
		std::string shaderName(shaders[i].c_str());
		std::string fullShaderName(shaderPath + shaderName + ".vert");		
		aqua::Byte* vertexShader(shaderManager->loadShader(aqua::PbString(fullShaderName.c_str())));
		fullShaderName = shaderPath + shaderName + ".frag";				
		aqua::Byte* fragmentShader(shaderManager->loadShader(aqua::PbString(fullShaderName.c_str())));
		if ((vertexShader != NULL) && (fragmentShader != NULL)) {
			aqua::shared_ptr<aqua::ShaderProgram> shaderPtr(shaderManager->addShaderProgram(aqua::HashString(shaderName.c_str()), vertexShader, fragmentShader, "main", "main"));
			PBDELETE [] fragmentShader;
			PBDELETE [] vertexShader;
		}
	}
	aqua::Locator::checkIn(shaderManager);
}




