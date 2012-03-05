#ifndef _GCM_ELASTIC_NODE_H
#define _GCM_ELASTIC_NODE_H  1

#include <vector>
using std::vector;

#include "Node.h"

class TetrMesh_1stOrder;

class ElasticNode : public Node
{
/* Inherited from Node 
*	int zone_num;
*	int local_num;
*	int remote_num;
*	int absolute_num;
*	int placement_type;
*	float coords[3];
*	float fixed_coords[3];
*/
public:
	float values[9];	// 3 velocity components and 6 tension tensor components
	float la;	// TODO If la and mu should be replaced with some ID of rheology class???
	float mu;	//
	float rho;
	float yield_limit;
	vector<int>* elements;
	vector<int>* border_elements;
	TetrMesh_1stOrder* mesh;
protected:
	// TODO should we switch from vector to memmory block with offsets in it?
	// int elems_offset;
	// int elems_size;
	// TODO How to deal with temporary variables???
	// int element_for_interpolation;
	// float random_axis[3];
	// float maxL;
	// .. and so on
};

#endif
