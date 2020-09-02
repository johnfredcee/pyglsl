#include "StdAfx.h"

#include <CRC/CRC.h>
#include <Locator.h>
#include "GraphicObjectNameManager.h"

using namespace aqua;

GraphicObjectNameManager::GraphicObjectNameManager() {
}

GraphicObjectNameManager::~GraphicObjectNameManager() {
}

// Strip the instance from the name 
std::string GraphicObjectNameManager::stripInstance(const std::string& name) {
	
	std::size_t in = name.rfind('!');
	if (in == std::string::npos)
		return name;
	else
		return name.substr(0, in);
}


aqua::Uint32 GraphicObjectNameManager::getInstanceCRC(const std::string& name) {
	
	std::string strippedName(GraphicObjectNameManager::stripInstance(name));
	Uint32 result = CalcCRC(strippedName.c_str());
	if (!instanceSeen(name)) {
		objects_.push_back(result);
	}
	return result;
}

bool GraphicObjectNameManager::instanceSeen(const std::string& name) {
	
	Uint32 crc = CalcCRC(stripInstance(name).c_str());
	GameObjectNameTable::const_iterator it = std::find(objects_.begin(), objects_.end(), crc);
	return (it != objects_.end());
}




