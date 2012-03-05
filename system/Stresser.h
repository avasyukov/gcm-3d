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
	virtual void set_current_stress(ElasticNode* cur_node, ElasticNode* new_node, float current_time) = 0;
	virtual bool loadTask(string fname) = 0;
	virtual void print() = 0;
protected:
	string stresser_type;
	Logger* logger;
};

#include "Stresser.inl"

#endif
