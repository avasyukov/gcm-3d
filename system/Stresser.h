#ifndef _GCM_STRESSER_H
#define _GCM_STRESSER_H  1

#include <string>
#include <vector>
#include <math.h>

using std::string;
using std::vector;

#include "../datatypes/ElasticNode.h"
#include "../system/LoggerUser.h"

class Stresser: protected LoggerUser
{
public:
	Stresser();
	~Stresser();
	string* get_stresser_type();
	virtual void set_current_stress(ElasticNode* cur_node, ElasticNode* new_node, float current_time) = 0;
	virtual bool loadTask(string fname) = 0;
	virtual void print() = 0;
protected:
	string stresser_type;
};

#endif
