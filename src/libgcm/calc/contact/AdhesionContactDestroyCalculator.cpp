#include "AdhesionContactDestroyCalculator.h"
#include "AdhesionContactCalculator.h"
#include "SlidingContactCalculator.h"
#include "../../node/CalcNode.h"

AdhesionContactDestroyCalculator::AdhesionContactDestroyCalculator()
{
	//U_gsl = gsl_matrix_alloc (18, 18);
	//om_gsl = gsl_vector_alloc (18);
	//x_gsl = gsl_vector_alloc (18);
	//p_gsl = gsl_permutation_alloc (18);
};

AdhesionContactDestroyCalculator::~AdhesionContactDestroyCalculator()
{
	//gsl_matrix_free(U_gsl);
	//gsl_vector_free(om_gsl);
	//gsl_vector_free(x_gsl);
	//gsl_permutation_free(p_gsl);
};

void AdhesionContactDestroyCalculator::do_calc(CalcNode* cur_node, CalcNode* new_node, CalcNode* virt_node, ElasticMatrix3D* matrix, float* values[], bool inner[], ElasticMatrix3D* virt_matrix, float* virt_values[], bool virt_inner[], float outer_normal[], float scale)
{
	//Update current node 'damage' status
	if (!cur_node->getContactConditionId())
	{	
		float force_cur[3] = {cur_node->values[3]*outer_normal[0]+cur_node->values[4]*outer_normal[1]+cur_node->values[5]*outer_normal[2],
			      cur_node->values[4]*outer_normal[0]+cur_node->values[6]*outer_normal[1]+cur_node->values[7]*outer_normal[2],
			      cur_node->values[5]*outer_normal[0]+cur_node->values[7]*outer_normal[1]+cur_node->values[8]*outer_normal[2]};
		float force_cur_abs = scalarProduct(force_cur,outer_normal);
		if (force_cur_abs > cur_node->getAdhesionThreshold())
		{
			cur_node->setContactConditionId(1);
			//LOG_INFO("-----------contact break!!");
		}
	}

	//Check if we must use Sliding, otherwise use adhesion
	if (cur_node->getContactConditionId() || virt_node->getContactConditionId())
	{
		SlidingContactCalculator* scc = new SlidingContactCalculator();
		scc->do_calc(cur_node,new_node,virt_node,matrix,values,inner,virt_matrix,virt_values,virt_inner,outer_normal,scale);
		delete scc;
	}
	else
	{
		AdhesionContactCalculator* acc = new AdhesionContactCalculator();
		acc->do_calc(cur_node,new_node,virt_node,matrix,values,inner,virt_matrix,virt_values,virt_inner,outer_normal,scale);
		delete acc;
	}
};
