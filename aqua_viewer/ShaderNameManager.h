#ifndef SHADER_NAME_MANAGER_H
#define SHADER_NAME_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <PbSystemTypes.h>
#include <aqua/BitmapData.h>

class ShaderNameManager : public aqua::Locator::ServiceSingleton<ShaderNameManager> 
{
	friend class aqua::Locator::ServiceSingleton<ShaderNameManager>;
public:

	typedef std::vector< std::pair<aqua::Uint32, aqua::Uint32> > VertexFormatVector;

	typedef std::vector< MaskType > TextureSlotVector;

	/** Constructpr */
	ShaderNameManager();

	/** Destructor */
	virtual ~ShaderNameManager();


	/**
	 * Add a shader name and format to the validation list
	 * @param shaderName Name of shader in system
	 * @param formatsAllowed   - vertex buffer format elements to allow
	 * @param formatsForbidden - vertex buffer format elements to forbid
	 */
	void addShaderName(const std::string& shaderName, aqua::Uint32 formatsAllowed, aqua::Uint32 formatsForbidden,  ShaderNameManager::TextureSlotVector& slots);

	/**
	 * Check a name against the list of valid shader names
	 * @param name Shader name to check
	 * @return true if the shader name is valid
	 */
	bool validateShaderName(const std::string& name);

	/**
	 * Get the texture slot flag for the slot.
	 * @param shaderName Name of shader
	 * @param textureSlot Index of slot
	 * @return slot code
	 */
	MaskType getTextureSlotFlag(const std::string& shaderName, aqua::Uint32 textureSlot);

	/**
	 * Validate that the shader matches what should be used with the formats (ie alpha/non alpha)
	 * @param shaderName Name of shader to check
	 * @param texId Name of texture id to check
	 * @param textureSlot Slot the texture is in
	 * @return true if the shader can use this texture in this slot
	 */
	bool validateTextureForShader(const std::string& shaderName, crc32 texId, aqua::Uint32 textureSlot);

	/**
	 * Test to see if the shader allows this format or not
	 * @param name Shader name
	 * @param format Format of accompanying buffer
	 * @return true if the shader is valid
	 */
	bool validateShaderFormats(const std::string& name, aqua::Uint32 format);

	
	/**
	 * Given the crc of a shader name, find the fucking shader in the list of
	 * known names.
	 * @param id shaderName crc
	 * @return name as string
	 */
	std::string getShaderName(crc32 id) const;
	
private:
	/** Holds a list of valid shader names */
	std::vector< std::string >	shaderNames_;
	/** Holds s list of valid input formats for each shader */
	VertexFormatVector shaderFormats_;
	// holds texture slot defintions
	std::vector< TextureSlotVector > textureFormats_;
};

#endif
