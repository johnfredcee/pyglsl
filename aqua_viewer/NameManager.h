#ifndef NAMEMANAGER_H
#define NAMEMANAGER_H

#include <Types.h>

class NameManager {
public:
	typedef std::map<aqua::Uint32, std::string> NameDictionary;

private:
	NameDictionary names_;
public:
	bool addName(std::string name);
	bool checkName(std::string name);
	bool checkName(aqua::Uint32 crc);
};
#endif
