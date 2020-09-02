
#include "StdAfx.h"
#include <Types.h>
#include <Mathematics/PbMaths.h>
#include <MetaData/Dynamic/DynamicMetaData.h>
#include <MetaData/Dynamic/DynamicMetaParser.h>
#include <aqua/Material.h>
#include "SceneManager.h"
#include "MetaSerializer.h"
#include "ShaderNameManager.h"
#include "MetaMaterialManager.h"

using namespace aqua;

/**
 * Serialise a material into metadata
 * @param data Data of thing
 * @param parser <doc>
 */
template<> void MetaSerialiser<Material>(const Material& data, DynamicMetaParser& parser) {

	PbColor						 diffuse;
	aqua::HashString			 shader;
	//bool						 textureAlpha;
	aqua::Array<TextureSlotData> slotData;

	PBASSERT(parser.IsValid(), "Material parser not valid");
	DynamicMetaParser diffuseParser(parser.Find("diffuse"));
	diffuseParser.Find(CalcCRC("r")).SetValue(data.diffuse.r());
	diffuseParser.Find(CalcCRC("g")).SetValue(data.diffuse.g());
	diffuseParser.Find(CalcCRC("b")).SetValue(data.diffuse.b());
	diffuseParser.Find(CalcCRC("a")).SetValue(data.diffuse.a());		
	DynamicMetaParser specularParser(parser.Find("specular"));
	specularParser.Find("factor").SetValue(data.specular.factor);
	specularParser.Find("power").SetValue(data.specular.power);
	parser.Find("shader").SetValue(data.shader.getHashValue());
	parser.Find("emissiveLevel").SetValue(data.emissiveLevel);
	parser.Find("textureAlpha").SetValue(data.textureAlpha);
	DynamicMetaParser slotDataParser(parser.Find("slotData"));
	slotDataParser.SetArrayCount(data.getTextureCount());
	for(Uint32 texIndex = 0; texIndex < data.getTextureCount(); ++texIndex) {
		DynamicMetaParser textureParser(slotDataParser.Find(texIndex, MetaParser::FIND_BY_INDEX));
		textureParser.Find("texture").SetValue(data.slotData[texIndex].texture);
		textureParser.Find("textureCoordSlot").SetValue(data.slotData[texIndex].textureCoordSlot);
		textureParser.Find("blendMode").SetValue(data.slotData[texIndex].blendMode);
		textureParser.Find("specialMode").SetValue(data.slotData[texIndex].specialMode);			
		textureParser.Find("uModPerSecond").SetValue(data.slotData[texIndex].uModPerSecond);
		textureParser.Find("vModPerSecond").SetValue(data.slotData[texIndex].vModPerSecond);
		textureParser.Find("textureAnimationEnabled").SetValue(data.slotData[texIndex].textureAnimationEnabled);
	}
	return;
}

template <> void MetaDeserialiser(Material& data, DynamicMetaParser& parser) {
	// NOT IMPLEMENTED JUST YET
	(void) parser;
	(void) data;
}

MetaMaterialManager::MetaMaterialManager() {

	
}

MetaMaterialManager::~MetaMaterialManager() {


}

/**
 * Add a material to the system
 * @param id Id of material
 * @param name name of material
 * @param shaderName shader associated with the material
 * @param alphaFlag alpha flag
 */
crc32 MetaMaterialManager::addMaterial(const std::string &name, const Material& mat) {

	const Locator::Service<SceneManager>& scene(Locator::checkOut<SceneManager>());
	crc32 result = CalcCRC(name.c_str());
	materialNames_[result] = name;
	
	const Locator::Service<ShaderNameManager>& shaderManager(Locator::checkOut<ShaderNameManager>());	
	shaderNames_[result] = shaderManager->getShaderName(mat.shader.getHashValue());
	Locator::checkIn(shaderManager);
	
	std::string materialTypeName("Material");
	DynamicMetaParser materialParser(scene->allocateData(name.c_str(), materialTypeName));
	MetaSerialiser<Material>(mat, materialParser);
	Locator::checkIn(scene);
	return result;	
}

/**
 * Get the name of a material (just the material name portion, no shader
 * @param id Material id
 * @return name of material
 */
std::string MetaMaterialManager::getName(crc32 id) const {
	std::map<crc32, std::string>::const_iterator it = materialNames_.find(id); 
	return it->second;
}

/**
 * Get the name of a material (just the material name portion, no shader
 * @param id Material id
 * @return name of material
 */
std::string MetaMaterialManager::getShaderName(crc32 id) const {
	std::map<crc32, std::string>::const_iterator it = shaderNames_.find(id); 
	PBASSERT(it != shaderNames_.end(), "Failed to find shader name");
	return it->second;
}

/**
 * Check that the name does not already exist
 * @return true if the material matches the existing one or it's a new one
 */
bool MetaMaterialManager::checkName(const std::string& name) {
	
	crc32 crc = CalcCRC(name.c_str());
	bool result = materialNames_.find(crc) != materialNames_.end();
	return result;
}

bool approxEqual(Float32 a, Float32 b, Float32 epslion) {
	return (PbMaths::abs(b-a) < epslion);
}

/**
 * Check to see if material exists and is identical to the given material
 * @param name Name of material to check
 * @param mat Material to check against
 * @return true if material exists and is identical
 */
bool MetaMaterialManager::checkMaterial(const std::string& name, aqua::shared_ptr<Material> mat) {
	const Locator::Service<SceneManager>& scene(Locator::checkOut<SceneManager>());
	bool result = scene->existsInScene(name);
	if (result) {
		// TO DO check is not duplicate.
		DynamicMetaParser parser( scene->fetchData(name) );
		DynamicMetaParser diffuseParser(parser.Find("diffuse"));
		result = result && approxEqual( mat->diffuse.r(), diffuseParser.Find(CalcCRC("r")).GetValue<Float32>(), 0.1f );
		result = result && approxEqual( mat->diffuse.g(), diffuseParser.Find(CalcCRC("g")).GetValue<Float32>(), 0.1f );
		result = result && approxEqual( mat->diffuse.b(), diffuseParser.Find(CalcCRC("b")).GetValue<Float32>(), 0.1f );
		result = result && approxEqual( mat->diffuse.a(), diffuseParser.Find(CalcCRC("a")).GetValue<Float32>(), 0.1f );		
	}
	Locator::checkIn(scene);
	return result;		
}

/**
 * Oh, for the love of Pete. Bring back strtok(), all is forgiven!
 * @param str String to split
 * @param tokens Vector of split up strings
 * @param delimiters Delimiters to split strings by
 */
void tokenizeString(const std::string& str,
					std::vector<std::string>& tokens,
					const std::string& delimiters = " ")
{
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos	   = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.	 Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

/**
 * Parse a string that describes a vertex buffer format and return the format code
 * @param description Format description
 * @return format encoded as int
 */
const Uint32 parseFormatDescription(std::string& description) {

	Uint32 result = 0;

	std::map<std::string, Uint32> formatLookup;
	std::vector<std::string>	  elements;

	formatLookup["COLOR"] = 1;
	formatLookup["COLOUR"] = 1;
	formatLookup["UV1"] = 2;
	formatLookup["NORMAL"] = 4;
	formatLookup["PRIMITIVE"] = 8;
	formatLookup["UV2"] = 16;

	// step one .. split up by spaces or commas
	tokenizeString(description, elements, std::string(" ,"));
	// step two..assemble format
	for(std::vector<std::string>::iterator elementName = elements.begin();
		elementName != elements.end(); ++elementName)  {
		std::map<std::string, Uint32>::iterator f_it = formatLookup.find(*elementName);
		if (f_it != formatLookup.end())
			result += f_it->second;
		else
			wxLogError(wxString::Format("%s is unrecognised vertex buffer element, check xml file", elementName->c_str()));
	}
	return result;
}

/**
 * Construct a human readable representation of a format string
 * @param format Format to describe
 * @param elementNames Array of strings to recieve format
 */
void makeFormatDescription(Uint32 format, std::vector<std::string>& elementNames) {
	std::map<Uint32, std::string> formatLookup;
	std::vector<std::string>	  elements;

	formatLookup[1] = "COLOR";
	formatLookup[2] = "UV1";
	formatLookup[4] = "NORMAL";
	formatLookup[8] = "PRIMITIVE";
	formatLookup[16] = "UV2";
	for(int index = 1; index <= 16; index *= 2) {
		if (format & index) {
			elementNames.push_back(formatLookup[index]);
		}
	}
	return;
}


/**
 * Work out the texture Slot enum from a human readale string
 * @param description Slot descirption
 * @return Enum value
 */
MaskType  parseTextureSlotDescription(std::string& description) {
	if (description == "ANY")
		return ANY;
	if (description == "NONE")
		return NONE;
	if (description == "SPECULAR")
		return SPECULAR;
	if (description == "TRANSPARENT")
		return OPACITY;
	return ANY;
}
