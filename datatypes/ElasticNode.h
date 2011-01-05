#include "Node.h"

class ElasticNode : public Node
{
//	int zone_num;
//	int local_num;
//	int remote_num;
//	int absolute_num;
//	float coords[3];
//	float fixed_coords[3];
public:
	float values[9];
	float la;	// TODO If la and mu should be replaced with some ID of rheology class???
	float mu;	//
	float rho;
protected:
	int elems_offset;
	int elems_size;
	// TODO How to deal with temporary variables???
	// int element_for_interpolation;
	// float random_axis[3];
	// float maxL;
	// .. and so on
};
