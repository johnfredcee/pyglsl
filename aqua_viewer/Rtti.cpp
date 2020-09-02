#include "StdAfx.h"
#include "Rtti.h"

Rtti::Rtti(const char* acName) : acName_(acName) {
}

const char* Rtti::getName() {
	return acName_.c_str();
}

bool Rtti::is(const Rtti& other) {
	return acName_ == other.acName_;
}
