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
		void do_calc(float time, float* cur_coords, CalcNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], float outer_normal[]);
	};
}

#endif
