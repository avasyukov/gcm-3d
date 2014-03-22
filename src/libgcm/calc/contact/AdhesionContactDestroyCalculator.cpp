#include "calc/contact/AdhesionContactDestroyCalculator.h"

#include "node/CalcNode.h"

AdhesionContactDestroyCalculator::AdhesionContactDestroyCalculator()
{
    scc = new SlidingContactCalculator();
    acc = new AdhesionContactCalculator();
};

AdhesionContactDestroyCalculator::~AdhesionContactDestroyCalculator()
{
    delete scc;
    delete acc;
};

void AdhesionContactDestroyCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                            RheologyMatrix3D& matrix, vector<CalcNode>& previousNodes, bool inner[],
                            RheologyMatrix3D& virt_matrix, vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                            float outer_normal[], float scale)
{
    //Update current node 'damage' status
    if (!cur_node.getContactConditionId())
    {
        float force_cur[3] = {
            cur_node.values[3]*outer_normal[0] + cur_node.values[4]*outer_normal[1] + cur_node.values[5]*outer_normal[2],
            cur_node.values[4]*outer_normal[0] + cur_node.values[6]*outer_normal[1] + cur_node.values[7]*outer_normal[2],
            cur_node.values[5]*outer_normal[0] + cur_node.values[7]*outer_normal[1] + cur_node.values[8]*outer_normal[2]
        };

        float force_cur_abs = scalarProduct(force_cur, outer_normal);
        // FIXME_ASAP return back adhesion threshold
        /*
        if (force_cur_abs > cur_node.getAdhesionThreshold())
        {
            cur_node.setContactConditionId(1); // TODO - remove magic number
        }*/
    }

    //Check if we must use Sliding, otherwise use adhesion
    if (cur_node.getContactConditionId() || virt_node.getContactConditionId())
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
