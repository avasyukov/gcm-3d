#ifndef _GCM_STRESSER_H
#define _GCM_STRESSER_H  1

#include <string>
#include <vector>
#include <math.h>

using std::string;
using std::vector;

#include "../datatypes/ElasticNode.h"

class Stresser
{
public:
	Stresser();
	~Stresser();
	string* get_stresser_type();
	void attach(Logger* new_logger);
	virtual void set_current_stress(ElasticNode* cur_node, ElasticNode* new_node, float current_time); // TODO avoid hardcoded impl
	virtual bool loadTask(string fname) {}
	virtual void print() {}
protected:
	string stresser_type;
	Logger* logger;
};

#include "Stresser.inl"

#endif
