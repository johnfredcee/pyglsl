
#include "StdAfx.h"

#include <Types.h>
#include <aqua/BitmapData.h>
#include <aqua/TextureData.h>
#include <aqua/Material.h>

#include "SceneManager.h"
#include "MetaBitmapManager.h"

using namespace aqua;

template<> void MetaSerialiser<BitmapData>(const BitmapData& data, DynamicMetaParser& parser)
{
	PBASSERT(parser.IsValid(), "Invalid parser!");
	parser.Find("width").SetValue(data.width);
	parser.Find("height").SetValue(data.height);
	
	parser.Find("pixelFormat").SetValue(data.pixelFormat);
	parser.Find("attributes").SetValue(data.attributes);
	parser.Find("mask").SetValue(data.mask);
	parser.Find("data").CopyBinBlock(data.data.dataPtr, data.data.sizeInBytes);
}

template<> void MetaSerialiser<TextureData>(const TextureData& data, DynamicMetaParser& parser)
{
	parser.Find("filterType").SetValue(data.filterType);
	parser.Find("mipmaps").SetArrayCount(data.mipmaps.size());
	for(Uint32 i = 0; i < data.mipmaps.size(); ++i) {
		parser.Find("mipmaps").Find(i, MetaParser::FIND_BY_INDEX).SetValue(data.mipmaps[i]);
	}
}

MetaBitmapManager::MetaBitmapManager() {
	
};

MetaBitmapManager::~MetaBitmapManager() {
}

/**
 * Add a material to the system
 * @param id Id of material
 * @param name name of material
 * @param shaderName shader associated with the material
 * @param alphaFlag alpha flag
 */
crc32 MetaBitmapManager::addBitmap(const std::string &name, const BitmapData& tex) {
	const Locator::Service<SceneManager>& scene(Locator::checkOut<SceneManager>());
	crc32 result = CalcCRC(name.c_str());

	if (!scene->existsInScene(name)) {
										
		std::string bitmap_name(name + std::string("_0"));
		std::string bitmapDataTypeName("BitmapData");
		DynamicMetaParser bitmapParser(scene->allocateData(bitmap_name.c_str(), bitmapDataTypeName));
		wxLogDebug("Allocated bitmap id %08x", CalcCRC(bitmap_name.c_str()));
		MetaSerialiser<BitmapData>(tex, bitmapParser);

		TextureData texData;		
		std::string textureDataTypeName("TextureData");
		DynamicMetaParser texDataParser(scene->allocateData(name.c_str(), textureDataTypeName));
		wxLogDebug("Allocated texture id %08x", CalcCRC(name.c_str()));
		texData.filterType = FILTERING_BILINEAR;
		texData.mipmaps.pushBack(aqua::HashString(bitmap_name.c_str()));
		MetaSerialiser<TextureData>(texData, texDataParser);
	
		bitmapNames_[result] = name;		
	} // to do -- check match?
	
	Locator::checkIn(scene);
	return result;	
}


void MetaBitmapManager::getBitmapName(crc32& bitmapName, std::string& name)
{
	std::map<crc32, std::string> bitmapNames_;
	name = bitmapNames_[bitmapName];
	return;
}
