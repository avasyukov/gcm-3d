#ifndef _GCM_BORDER_CONDITION_H
#define _GCM_BORDER_CONDITION_H 1

#include <string.h>

#include "util/areas/Area.h"
#include "calc/border/BorderCalculator.h"

namespace gcm {
	class PulseForm;
	class CalcNode;
	
	class BorderCondition
	{
	public:
		BorderCondition();
		BorderCondition(Area* _area, PulseForm* _form, BorderCalculator* _calc);
		~BorderCondition();
	
		Area* area;
		PulseForm* form;
		BorderCalculator* calc;
		void doCalc(float time, CalcNode& cur_node, CalcNode& new_node, RheologyMatrix3D& matrix, 
							vector<CalcNode>& previousNodes, bool inner[], float outer_normal[]);
	};
}

#endif
