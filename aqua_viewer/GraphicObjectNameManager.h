#ifndef GRAPHICOBJECTNAMEMANAGER_H
#define GRAPHICOBJECTNAMEMANAGER_H

#include <string>
#include <vector>
#include <PbSystemTypes.h>


class GraphicObjectNameManager : public aqua::Locator::ServiceSingleton<GraphicObjectNameManager> 
{
	friend class aqua::Locator::ServiceSingleton<GraphicObjectNameManager>;

	/** Array of names we have already seen */
	typedef std::vector<aqua::Uint32> GameObjectNameTable;

   
	/** Array of metadata ids for metadata that describes this object. */
	GameObjectNameTable objects_;	

public:
	
    /** Description for GraphicObjectNameManager. */
	GraphicObjectNameManager();
	
    /** Description for GraphicObjectNameManager. */
	virtual ~GraphicObjectNameManager();

	/**
	 * Strip instance qualification from the name
	 * @param name raw name of object
	 * @return name sans instance qualification
	 */
	std::string stripInstance(const std::string& name);

	/**
	 * Get the CRC of the instance unqualified name
	 * @param name raw name
	 * @return crc of name sans instance qualification
	 */
	crc32 getInstanceCRC(const std::string& name);

	/**
	 * Test to see if an isntance of this object has been seen before
	 * @param name Instance qualified name
	 * @return true if the instance has been seen before
	 */
	bool instanceSeen(const std::string& name);


};
#endif
