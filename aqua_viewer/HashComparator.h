#ifndef HASHCOMPARATOR_H
#define HASHCOMPARATOR_H

class HashComparator
{
public:
	bool operator()(aqua::HashString h1, aqua::HashString h2) const {
		return h1.getHashValue() < h2.getHashValue();
	}
		
};

#endif
