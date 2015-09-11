#include "libgcm/calc/contact/OpenFractureContactCalculator.hpp"


using namespace gcm;
using std::vector;

OpenFractureContactCalculator::OpenFractureContactCalculator()
{
	type = "OpenFractureContactCalculator";

    adhesionCalc = new AdhesionContactCalculator();
	borderCalc = new FreeBorderCalculator();
};

OpenFractureContactCalculator::~OpenFractureContactCalculator()
{
    delete adhesionCalc;
	delete borderCalc;
};

void OpenFractureContactCalculator::setFracArea(Area* area) {
	fracArea = area;
};

void OpenFractureContactCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, 
	CalcNode& virt_node, RheologyMatrixPtr matrix, vector<CalcNode>& previousNodes, 
	bool inner[], RheologyMatrixPtr virt_matrix, vector<CalcNode>& virtPreviousNodes,
	bool virt_inner[], float outer_normal[], float scale) 
{
	if(fracArea->isInArea(cur_node))
		borderCalc->doCalc(cur_node, new_node, matrix, previousNodes, inner, outer_normal, scale);
	else
		adhesionCalc->doCalc(cur_node, new_node, virt_node, matrix, previousNodes, inner,
							 virt_matrix, virtPreviousNodes, virt_inner, outer_normal, scale);
};
