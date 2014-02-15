#ifndef GCM_INTERPOLATION_FIXED_AXIS_H
#define	GCM_INTERPOLATION_FIXED_AXIS_H

#include "mesh/tetr/TetrMeshFirstOrder.h"
#include "mesh/tetr/TetrMeshSecondOrder.h"
#include "method/NumericalMethod.h"
#include "mesh/Mesh.h"
#include "util/ElasticMatrix3D.h"
#include "node/CalcNode.h"
#include "Logging.h"
#include "Exception.h"


namespace gcm
{

	class InterpolationFixedAxis : public NumericalMethod {
	public:
		InterpolationFixedAxis();
		virtual ~InterpolationFixedAxis();
		int getNumberOfStages();
		void doNextPartStep(CalcNode& cur_node, CalcNode& new_node, float time_step, int stage, Mesh* mesh);
		float getMaxLambda(CalcNode* node);
		string getType();
	protected:
		int prepare_node(CalcNode& cur_node, ElasticMatrix3D& elastic_matrix3d,
				float time_step, int stage, Mesh* mesh,
				float* dksi, bool* inner, vector<CalcNode>& previous_nodes,
				float* outer_normal);
		int find_nodes_on_previous_time_layer(CalcNode& cur_node, int stage, Mesh* mesh,
				float dksi[], bool inner[], vector<CalcNode>& previous_nodes,
				float outer_normal[]);
		
		USE_LOGGER;
	};
}

#endif	/* GCM_INTERPOLATION_FIXED_AXIS_H */

