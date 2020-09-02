#include "StdAfx.h"

#include <wx/textfile.h>

#include <aqua/BitmapData.h>
#include <Managers/TextureManager.h>
#include "TextureDefinitionManager.h"

using namespace aqua;

void TextureDefinitionManager::enumerateEnum(DynamicMetaData* metaData, wxString& eenumName, EnumInfo& info)
{
	wxArrayString result;
	PBLOG_INFO(Logger::BLUE <<	"Reflecting over enum " << (const char*)(eenumName.c_str()) << Logger::WHITE << Logger::ENDL);
	const MetaEnum* eenum = metaData->FindEnumInNameSpace(eenumName.c_str());
	if (eenum) {
		info.name_ = eenumName.ToStdString();
		Int32 itemCount = eenum->m_entries.count;
		for(Int32 i = 0; i < itemCount; ++i) {
			MetaEnumEntry entry(eenum->m_entries[i]);
			Uint32 eId = entry.m_id;
			const char* eText = metaData->FindString(eId);
			Uint32 eValue = entry.m_value;
			wxString enumText(eText);
			info.members_[eValue] = enumText.Upper().ToStdString();
			PBLOG_INFO(Logger::YELLOW << eText << " = " << eValue << Logger::WHITE << Logger::ENDL);
		}
	}
	return;
}

TextureDefinitionManager::TextureDefinitionManager()
{
}

TextureDefinitionManager::~TextureDefinitionManager()
{
}



/** Actually write out the texture definition file, textures per-platform are hard-coded */
void TextureDefinitionManager::writeOutputFile(const std::string& filename, DynamicMetaData* enumsInHere)
{
	wxString pixelFormatEnum("PixelFormat");
	EnumInfo pixelFormatEnumInfo;
	enumerateEnum(enumsInHere, pixelFormatEnum, pixelFormatEnumInfo);
	
	wxString attributesEnum("Attributes");
	EnumInfo attributesEnumInfo;
	enumerateEnum(enumsInHere, attributesEnum, attributesEnumInfo);

	wxString maskTypeEnum("MaskType");
	EnumInfo maskTypeEnumInfo;
	enumerateEnum(enumsInHere, maskTypeEnum, maskTypeEnumInfo);
	
	wxString filterTypeEnum("FilterType");
	EnumInfo filterTypeEnumInfo;
	enumerateEnum(enumsInHere, filterTypeEnum, filterTypeEnumInfo);
		
	wxTextFile textureDefinitionFile;
	textureDefinitionFile.Create(wxString(filename.c_str()));
	wxString defline(wxString::Format(" Row Count PC=%d", textureDefinitions_.size()));
	textureDefinitionFile.AddLine(defline);
	for(TextureDefinitionMapType::iterator it = textureDefinitions_.begin(); it != textureDefinitions_.end(); ++it) {
		// below line was for bbr	
		//if(it->second.alphaBits == 0) {
		//	textureDefinitionFile.AddLine(wxString::Format(wxString(it->second.row.c_str()), DXT1));
		//} else {
		//	textureDefinitionFile.AddLine(wxString::Format(wxString(it->second.row.c_str()), DXT5));
		//}
		wxString filterStr(filterTypeEnumInfo.members_[1].c_str());
		wxString pixFormatStr(pixelFormatEnumInfo.members_[RGBA32], 1);
		wxString alphaBitsStr(maskTypeEnumInfo.members_[it->second.alphaBits = 0 ? RGBA32 : RGB24]);
		textureDefinitionFile.AddLine(wxString::Format(wxString(it->second.row.c_str()), filterStr.c_str(), pixFormatStr.c_str(), alphaBitsStr.c_str()));
	}
#ifdef NON_PC_FORMATS	
	// write out Wii defines
	defline = wxString::Format(" Row Count Wii=%d", textureDefinitions_.size());
	textureDefinitionFile.AddLine(defline);
	for(TextureDefinitionMapType::iterator it = textureDefinitions_.begin(); it != textureDefinitions_.end(); ++it) {
		textureDefinitionFile.AddLine(wxString::Format(wxString(it->second.row.c_str()), RGBA32));
	}
	// write out XBox360 defines
	defline = wxString::Format(" Row Count X360=%d", textureDefinitions_.size());
	textureDefinitionFile.AddLine(defline);
	for(TextureDefinitionMapType::iterator it = textureDefinitions_.begin(); it != textureDefinitions_.end(); ++it) {
		if(it->second.alphaBits == 0) {
			textureDefinitionFile.AddLine(wxString::Format(wxString(it->second.row.c_str()), DXT1));
		} else {
			textureDefinitionFile.AddLine(wxString::Format(wxString(it->second.row.c_str()), DXT5));
		}
	}
	// wite out ps3 definitions
	defline = wxString::Format(" Row Count PS3=%d", textureDefinitions_.size());
	textureDefinitionFile.AddLine(defline);
	for(TextureDefinitionMapType::iterator it = textureDefinitions_.begin(); it != textureDefinitions_.end(); ++it) {
		if(it->second.alphaBits == 0) {
			textureDefinitionFile.AddLine(wxString::Format(wxString(it->second.row.c_str()), DXT1));
		} else {
			textureDefinitionFile.AddLine(wxString::Format(wxString(it->second.row.c_str()), DXT5));
		}
	}
	// write out IPhone definitions !
	defline = wxString::Format(" Row Count iPhone=%d", textureDefinitions_.size());
	textureDefinitionFile.AddLine(defline);
	for(TextureDefinitionMapType::iterator it = textureDefinitions_.begin(); it != textureDefinitions_.end(); ++it) {
		textureDefinitionFile.AddLine(wxString::Format(wxString(it->second.row.c_str()), 1));
	}
#endif	
	textureDefinitionFile.Write();
	textureDefinitionFile.Close();
}

/** Clear all texture definitions */
void TextureDefinitionManager::clear()
{
	textureDefinitions_.clear();
}

/**
 * Add a texture to the list of defined textures
 * @param id Blue texture Id
 * @param name Name of texture
 * @param width Width of image
 * @param height Height of image
 * @param path Path to texture
 * @param alphaBits number of alpha channels
 */
void TextureDefinitionManager::addTexture(crc32 id, const std::string& name, Uint32 width, Uint32 height, const std::string &path, int alphaBits, int attributes)
{
	TextureDefinition entry;
	entry.width      =  width;
	entry.height     =  height;
	entry.path       =  path;
	entry.name       =  name;
	entry.alphaBits  =  alphaBits;
	entry.attributes =  attributes;
// below line was for bbr	
// note carefully the %% in the format argument - this becomes a single % and is used again in a Format() call later!
//	wxString row(wxString::Format(wxT("Name=%s;Group=0;Width=%04d;Height=%04d;Filter=4;Format=%%d;Path=%s;MipMaps=1;Alpha=%d"), name, width, height, path, attributes, alphaBits));
// this one is for the prototyper
	wxString row(wxString::Format(wxT("Name=%s;Group=0;Width=%04d;Height=%04d;Filter=%%s;Format=%%s;Path=%s;MipMaps=0;Alpha=%%s"), name, width, height, path));	
	entry.row       = row;
	textureDefinitions_[id] = entry;
}


/**
 * Return true if the texture has a non-opaque alpha channel
 * @param id Texture id
 * @return Alpha flag
 */
bool TextureDefinitionManager::isAlphaTexture(crc32 id)
{
	TextureDefinition def(textureDefinitions_[id]);
	return (def.alphaBits != 0);
}

