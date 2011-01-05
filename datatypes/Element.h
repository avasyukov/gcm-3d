#ifndef _GCM_ELEMENT_H
#define _GCM_ELEMENT_H  1

#include <string>
#include <vector>

using std::string;
using std::vector;

class Element
{
public:
	Element();
	~Element();
	int local_num;
	int absolute_num;
protected:
private:
};

#include "Element.inl"

#endif
