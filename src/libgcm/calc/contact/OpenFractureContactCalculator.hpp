#ifndef _GCM_CONTACT_OPENFRACTURE_CALCULATOR_H
#define _GCM_CONTACT_OPENFRACTURE_CALCULATOR_H  1

#include "libgcm/calc/contact/AdhesionContactCalculator.hpp"
#include "libgcm/calc/border/FreeBorderCalculator.hpp"

namespace gcm
{
    class OpenFractureContactCalculator : public ContactCalculator
    {
    public:
        OpenFractureContactCalculator();
        ~OpenFractureContactCalculator();
		void setFracArea(Area* area);
        void doCalc(CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                    RheologyMatrixPtr matrix, std::vector<CalcNode>& previousNodes, bool inner[],
                    RheologyMatrixPtr virt_matrix, std::vector<CalcNode>& virtPreviousNodes, 
					bool virt_inner[], float outer_normal[], float scale);
    private:
		Area* fracArea;
        AdhesionContactCalculator* adhesionCalc;
		FreeBorderCalculator* borderCalc;
    };
}

#endif /* _GCM_CONTACT_OPENFRACTURE_CALCULATOR_H */
