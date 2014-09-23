#ifndef _GCM_SMOOTH_BORDER_CALCULATOR_H
#define _GCM_SMOOTH_BORDER_CALCULATOR_H  1

#include <gsl/gsl_linalg.h>

#include "libgcm/calc/border/BorderCalculator.hpp"


class SmoothBorderCalculator : public BorderCalculator
{
public:
    SmoothBorderCalculator();
    ~SmoothBorderCalculator();
    void doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                            vector<CalcNode>& previousNodes, bool inner[],
                            float outer_normal[], float scale);

    inline string getType() {
        return "SmoothBorderCalculator";
    }
	
	void setParameters(const xml::Node& params);

protected:

private:
    USE_LOGGER;
};

#endif
