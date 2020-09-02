
#include "StdAfx.h"
#include "NameManager.h"

using namespace aqua;

/**
 * Add a name to the name manager.
 * @param name Name to add
 * @return true if it is unique
 */
bool NameManager::addName(std::string name)
{
	bool result = false;
	Uint32 Uint32 = CalcCRC(name.c_str());
	NameDictionary::iterator it = names_.find(Uint32);
	if (it != names_.end()) {
		result = true;
		names_[Uint32] = name;
	}
	return result;
}


/**
 * Return true if name exists
 * @param name name to test for
 * @return true if the name exists
 */
bool NameManager::checkName(std::string name) {
	Uint32 Uint32 = CalcCRC(name.c_str());
	return names_.find(Uint32) != names_.end();
}

/**
 * Check if name exists
 * @param Uint32 Uint32 of name
 * @return true of name exists
 */
bool NameManager::checkName(Uint32 Uint32) {
	return names_.find(Uint32) != names_.end();	
}
