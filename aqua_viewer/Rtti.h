#ifndef RTTI_H
#define RTTI_H

class Rtti {
public:
	Rtti(const char* acName);

	const char* getName();

	bool is(const Rtti& other);
	
private:
	std::string acName_;
	
};

#endif
