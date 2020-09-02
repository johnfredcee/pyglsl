#include "StdAfx.h"

#include <aqua/Material.h>
#include "TextureDefinitionManager.h"
#include "MetaMaterialManager.h"
#include "ShaderNameManager.h"

using namespace aqua;

ShaderNameManager::ShaderNameManager() {
	
}

ShaderNameManager::~ShaderNameManager() {
	
}

/**
 * Add a shader name and format to the validation list
 * @param shaderName Name of shader in system
 * @param formatsAllowed   - vertex buffer format elements to allow
 * @param formatsForbidden - vertex buffer format elements to forbid
 */
void ShaderNameManager::addShaderName(const std::string& shaderName, Uint32 formatsAllowed, Uint32 formatsForbidden, ShaderNameManager::TextureSlotVector& slots) {
	shaderNames_.push_back(shaderName);
	std::pair<Uint32, Uint32> format(formatsAllowed, formatsForbidden);
	shaderFormats_.push_back(format);
	textureFormats_.push_back(slots);
	return;
}

/**
 * Check a name against the list of valid shader names
 * @param name Shader name to check
 * @return true if the shader name is valid
 */
bool ShaderNameManager::validateShaderName(const std::string& name) {
	return (std::find(shaderNames_.begin(), shaderNames_.end(), name) != shaderNames_.end());
}

/**
 * Get the texture slot flag for the slot.
 * @param shaderName Name of shader
 * @param textureSlot Index of slot
 * @return slot code
 */
MaskType ShaderNameManager::getTextureSlotFlag(const std::string& shaderName, Uint32 textureSlot) {
	
	MaskType result(ANY);
	std::vector<std::string>::const_iterator sh_it = std::find(shaderNames_.begin(), shaderNames_.end(), shaderName);
	if (sh_it != shaderNames_.end()) {
		int index = sh_it - shaderNames_.begin();
		ShaderNameManager::TextureSlotVector& slotFlags = textureFormats_[index];
		if (textureSlot < slotFlags.size()) {
			result = slotFlags[textureSlot];			
		}
	}
	return result;	
}

/**
 * Check texture matches what the shader needs
 * @param shaderName name of shader to validate against
 * @param texId texture to check
 * @param textureSlot slot texture is in
 * @return true if ok
 */
bool ShaderNameManager::validateTextureForShader(const std::string& shaderName, crc32 texId, Uint32 textureSlot) {

	const aqua::Locator::Service<TextureDefinitionManager> &textureDefinitionManager(aqua::Locator::checkOut<TextureDefinitionManager>()); 
	bool hasAlpha(textureDefinitionManager->isAlphaTexture(texId));
	aqua::Locator::checkIn(textureDefinitionManager);
	std::vector<std::string>::const_iterator sh_it = std::find(shaderNames_.begin(), shaderNames_.end(), shaderName);
	bool result = (sh_it != shaderNames_.end());
	if (result) {
		MaskType textureSlotFlag(ANY);
		int index = sh_it - shaderNames_.begin();
		ShaderNameManager::TextureSlotVector& slotFlags = textureFormats_[index];
		if (textureSlot < slotFlags.size()) {
			textureSlotFlag = slotFlags[textureSlot];
			result = ((textureSlotFlag == ANY) ||
					  ((textureSlotFlag == NONE) && (!hasAlpha)) ||
					  (((textureSlotFlag == OPACITY) || (textureSlotFlag == SPECULAR)) &&
					   (hasAlpha)));
		}

	}
	return result;
}

/**
 * Test to see if the shader allows this format or not
 * @param name Shader name
 * @param format Format of accompanying buffer
 * @return true if the shader is valid
 */
bool ShaderNameManager::validateShaderFormats(const std::string& name, Uint32 format) {
	std::vector<std::string>::const_iterator sh_it = std::find(shaderNames_.begin(), shaderNames_.end(), name);
	bool result = (sh_it != shaderNames_.end());
	if (result) {
		int index = sh_it - shaderNames_.begin();
		std::pair<Uint32, Uint32> shaderFormat(shaderFormats_[index]);
		if ((result) && (shaderFormat.first != 0)) {
			if  ((format & shaderFormat.first) != shaderFormat.first) {
				// we dont have all the elements the shader needs
				Uint32 missing = (format & shaderFormat.first) ^ shaderFormat.first;
				std::vector<std::string> missingElements;
				makeFormatDescription(missing, missingElements);
				wxLogError("Missing vertex elements detected ");
				for(std::vector<std::string>::iterator s_it = missingElements.begin(); s_it != missingElements.end(); ++s_it) {
					wxLogError(wxString::Format("Element %s ", *s_it));
				}
				result = false;
			}
		}
		if ((result) && (shaderFormat.second != 0)) {
			if ((format & shaderFormat.second) != 0) {
				// we have extra elements the shader cannot cope with. BANG!
				Uint32 extra = (format & shaderFormat.second);
				std::vector<std::string> extraElements;
				makeFormatDescription(extra, extraElements);
				wxLogError("Excess vertex elements detected ");
				for(std::vector<std::string>::iterator s_it = extraElements.begin(); s_it != extraElements.end(); ++s_it) {
					wxLogError(wxString::Format("Element %s ", *s_it));
				}
				result = false;
			}
		}
	}
	return result;
}

class CrcNameMatch	{
public:
	CrcNameMatch(crc32 id) : id_(id) {	};
	bool operator()(std::string name) {
		return CalcCRC(name.c_str()) == id_;
 	};
private:	
	crc32 id_;
};

/**
 * Get the name of a material (just the material name portion, no shader
 * @param id Material id
 * @return name of material
 */
std::string ShaderNameManager::getShaderName(crc32 id) const {
	std::vector<std::string>::const_iterator sh_it = std::find_if(shaderNames_.begin(), shaderNames_.end(), CrcNameMatch(id));	
	PBASSERT(sh_it != shaderNames_.end(), "Failed to find shader name");
	return *sh_it;
}
