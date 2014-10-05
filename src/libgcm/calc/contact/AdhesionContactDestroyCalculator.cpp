#include "libgcm/calc/contact/AdhesionContactDestroyCalculator.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::vector;

AdhesionContactDestroyCalculator::AdhesionContactDestroyCalculator()
{
	type = "AdhesionContactDestroyCalculator";

    scc = new SlidingContactCalculator();
    acc = new AdhesionContactCalculator();
};

AdhesionContactDestroyCalculator::~AdhesionContactDestroyCalculator()
{
    delete scc;
    delete acc;
};

void AdhesionContactDestroyCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                            RheologyMatrixPtr matrix, vector<CalcNode>& previousNodes, bool inner[],
                            RheologyMatrixPtr virt_matrix, vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                            float outer_normal[], float scale)
{
    //Update current node 'damage' status
    if ( !cur_node.isContactDestroyed() )
    {
        float force_cur[3] = {
            cur_node.sxx*outer_normal[0] + cur_node.sxy*outer_normal[1] + cur_node.sxz*outer_normal[2],
            cur_node.sxy*outer_normal[0] + cur_node.syy*outer_normal[1] + cur_node.syz*outer_normal[2],
            cur_node.sxz*outer_normal[0] + cur_node.syz*outer_normal[1] + cur_node.szz*outer_normal[2]
        };

        float force_cur_abs = scalarProduct(force_cur, outer_normal);
        float adhesionTreshold = Engine::getInstance().getContactCondition( 
                                        cur_node.getContactConditionId() )->getConditionParam();
        //if (force_cur_abs > 0.1 * adhesionTreshold)
        //{
        //    std::cout << "Force: " << force_cur_abs << "\n";
        //}
        if (force_cur_abs > adhesionTreshold)
        {
            cur_node.setContactDestroyed(true);
        }
    }

    //Check if we must use Sliding, otherwise use adhesion
    if (cur_node.isContactDestroyed() || virt_node.isContactDestroyed())
    {
        scc->doCalc(cur_node, new_node, virt_node, matrix, previousNodes, inner,
                        virt_matrix, virtPreviousNodes, virt_inner, outer_normal, scale);
    }
    else
    {
        acc->doCalc(cur_node, new_node, virt_node, matrix, previousNodes, inner,
                        virt_matrix, virtPreviousNodes, virt_inner, outer_normal, scale);
    }
};
