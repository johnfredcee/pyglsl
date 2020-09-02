#ifndef TEXTUREDEFINITIONMANAGER_H
#define TEXTUREDEFINITIONMANAGER_H

#include <string>
#include <map>
#include <Types.h>
#include <PbSystemTypes.h>

typedef std::map<aqua::Int32, std::string> EnumEntryInfo;

class EnumInfo
{
	public:
		std::string name_;
		EnumEntryInfo members_;
};

class TextureDefinitionManager : public aqua::Locator::ServiceSingleton<TextureDefinitionManager> {
		friend class aqua::Locator::ServiceSingleton<TextureDefinitionManager>;

	public:
		typedef struct s_TextureDefinition {
				std::string	 name;
				aqua::Uint32 width;
				aqua::Uint32 height;
				std::string	 path;
				wxString	 row;
				int			 alphaBits;
				int			 attributes;
		} TextureDefinition;


		typedef std::map<crc32, TextureDefinition> TextureDefinitionMapType;

		TextureDefinitionManager();

		virtual ~TextureDefinitionManager();


		/** Write all texture defintions to output */
		void writeOutputFile(const std::string& fileName, DynamicMetaData *metaData);

		/** Clear all texture defintions in the system */
		void clear();

		/**
		 * Add a texture to the database
		 * @param id Id of texture
		 * @param name Name of texture
		 * @param width Width of texture
		 * @param height Heigh tof texture
		 * @param path Path + Filename of texture
		 * @param alphaBits non zero if this thing has alpha
		 */
		void addTexture(crc32 id, const std::string& name, aqua::Uint32 width, aqua::Uint32 height, const std::string &path, int alphaBits = 0, int attributes = 0);

		/**
		 * Return true if the texture has a non-opaque alpha channel
		 * @param id Texture id
		 * @return Alpha flag
		 */
		bool isAlphaTexture(crc32 id);

	private:
		TextureDefinitionMapType textureDefinitions_;

		void enumerateEnum(DynamicMetaData* metaData, wxString& eenumName, EnumInfo& info);

		std::string filename_;

};

#endif
