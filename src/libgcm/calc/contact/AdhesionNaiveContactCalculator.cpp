#include "libgcm/calc/contact/AdhesionNaiveContactCalculator.hpp"


using namespace gcm;
using std::vector;

AdhesionNaiveContactCalculator::AdhesionNaiveContactCalculator()
{
	type = "AdhesionNaiveContactCalculator";
};

AdhesionNaiveContactCalculator::~AdhesionNaiveContactCalculator()
{
};

void AdhesionNaiveContactCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, CalcNode& virt_node,
                            RheologyMatrixPtr matrix, vector<CalcNode>& previousNodes, bool inner[],
                            RheologyMatrixPtr virt_matrix, vector<CalcNode>& virtPreviousNodes, bool virt_inner[],
                            float outer_normal[], float scale)
{
    for (int i = 0; i < 9; i++)
        new_node.values[i] = (previousNodes[0].values[i] + virt_node.values[i]) / 2.0;
};
