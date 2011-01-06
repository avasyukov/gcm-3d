#ifndef _GCM_MESH_H
#define _GCM_MESH_H  1

#include <string>
#include <vector>

#include "../system/logger.h"
#include "../rheotypes/RheologyCalculator.h"
#include "../methods/NumericalMethod.h"

using std::string;
using std::vector;

class Mesh
{
public:
	Mesh();
	~Mesh();
	void attach(Logger* new_logger);
	void attach(NumericalMethod* new_numerical_method);
	void attach(RheologyCalculator* new_rheology);
	virtual int step(); // TODO check if it is universal

	int zone_num;
	int mesh_num;
	string mesh_type;
	vector<Node> nodes;
	vector<Element> elems;
protected:
	Logger* logger;
	RheologyCalculator* rheology;
	NumericalMethod* method;
};

#include "Mesh.inl"

#endif
