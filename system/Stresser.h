#ifndef _GCM_STRESSER_H
#define _GCM_STRESSER_H  1

#include <string>
#include <vector>

using std::string;
using std::vector;

class Stresser
{
public:
	Stresser();
	~Stresser();
	string* get_stresser_type();
protected:
	string stresser_type;
};

#include "Stresser.inl"

#endif
