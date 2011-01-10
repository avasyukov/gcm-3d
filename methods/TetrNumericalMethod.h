#ifndef _GCM_TETR_NUMERICAL_METHOD_H
#define _GCM_TETR_NUMERICAL_METHOD_H  1

#include "NumericalMethod.h"
#include "../datatypes/ElasticNode.h"
#include "../meshtypes/TetrMesh.h"
class TetrMesh;

class TetrNumericalMethod : public NumericalMethod
{
public:
	TetrNumericalMethod();
	~TetrNumericalMethod();
	virtual int do_next_part_step(ElasticNode* cur_node, ElasticNode* new_node, float time_step, int stage, TetrMesh* mesh) = 0;
};

#include "TetrNumericalMethod.inl"

#endif
