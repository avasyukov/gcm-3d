#ifndef _GCM_MESH_H
#define _GCM_MESH_H  1

#include <string>
#include <vector>
#include <fstream>
#include <cstring>

using std::ifstream;

#include "../system/Logger.h"
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
	string* get_mesh_type();
	virtual int step(); // TODO check if it is universal
	virtual int get_index_of_element_owner(Node* node); // TODO check if it is universal

	int zone_num;
	int mesh_num;
	//vector<Node> nodes; // TODO do we need these vectors?
	//vector<Element> elems; // TODO or will they be replaced any time with specific datatypes?

protected:
	string mesh_type;
	Logger* logger;
	RheologyCalculator* rheology;
	NumericalMethod* method;
};

#include "Mesh.inl"

#endif
