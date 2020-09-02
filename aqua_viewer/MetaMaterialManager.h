#ifndef METAMATERIALMANAGER_H
#define METAMATERIALMANAGER_H


#include <string>
#include <map>
#include <vector>

#include <PbSystemTypes.h>
#include <Types.h>
#include <aqua/BitmapData.h>
#include <Locator.h>

namespace aqua {
	class Material;
};


class MetaMaterialManager : public aqua::Locator::ServiceSingleton<MetaMaterialManager> {


	friend class aqua::Locator::ServiceSingleton<MetaMaterialManager>;

public:

	typedef std::vector< std::pair< aqua::Uint32, aqua::Uint32> > VertexFormatVector;

	typedef std::vector< MaskType > TextureSlotVector;

	MetaMaterialManager();

	~MetaMaterialManager();
	
	/**
	 * Add a material to the system
	 * @param id Id of material
	 * @param name name of material
	 * @param shaderName shader associated with the material
	 * @param alphaFlag alpha flag
	 */
	crc32 addMaterial(const std::string &name, const aqua::Material& material);

	/**
	 * Get the name of a material (just the material name portion, no shader
	 * @param id Material id
	 * @return name of material
	 */
	std::string getName(crc32 id) const;

	/**
	 * Get the name of a material (just the material name portion, no shader
	 * @param id Material id
	 * @return name of material
	 */
	std::string getShaderName(crc32 id) const;

	/**
	 * Check that the name does not already exist
	 * @return true if the material matches the existing one or it's a new one
	 */
	bool checkName(const std::string& name);


	/**
	 * Check to see if material exists and is identical to the given material
	 * @param name Name of material to check
	 * @param mat Material to check against
	 * @return true if material exists and is identical
	 */
	bool checkMaterial(const std::string& name, aqua::shared_ptr<aqua::Material> mat);

	/**
	 * Add a shader name and format to the validation list
	 * @param shaderName Name of shader in system
	 * @param formatsAllowed   - vertex buffer format elements to allow
	 * @param formatsForbidden - vertex buffer format elements to forbid
	 */
	crc32 addShaderName(const std::string& shaderName, aqua::Uint32 formatsAllowed, aqua::Uint32 formatsForbidden, TextureSlotVector& slots);
	/**
	 * Check a name against the list of valid shader names
	 * @param name Shader name to check
	 * @return true if the shader name is valid
	 */
	bool validateShaderName(const std::string& name);
	/**
	 * Test to see if the shader allows this format or not
	 * @param name Shader Name
	 * @param format Format of accompanying buffer
	 * @return true if the shader is valid
	 */
	bool validateShaderFormats(const std::string& name, aqua::Uint32 format);
	/**
	 * Check texture matches what the shader needs
	 * @param shaderName name of shader to validate against
	 * @param texId texture to check
	 * @param textureSlot slot texture is in
	 * @return true if ok
	 */
	bool validateTextureForShader(const std::string& shaderName, crc32 texId, aqua::Uint32 textureSlot);
	/**
	 * Get the texture slot flag for the slot.
	 * @param shaderName Name of shader
	 * @param textureSlot Index of slot
	 * @return slot code
	 */
	MaskType getTextureSlotFlag(const std::string& shaderName, aqua::Uint32 textureSlot);
private:
	std::map<crc32, std::string> materialNames_;
	std::map<crc32, std::string> shaderNames_;
};


const aqua::Uint32 parseFormatDescription(std::string& description);

void makeFormatDescription(aqua::Uint32 format,
						   std::vector<std::string>& elementNames);

MaskType parseTextureSlotDescription(std::string& description);

#endif
