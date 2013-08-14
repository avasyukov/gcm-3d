/* 
 * File:   InterpolationFixedAxis.h
 * Author: anganar
 *
 * Created on May 3, 2013, 12:00 AM
 */

#ifndef GCM_INTERPOLATION_FIXED_AXIS_H
#define	GCM_INTERPOLATION_FIXED_AXIS_H

#include "NumericalMethod.h"
#include "../mesh/Mesh.h"
#include "../node/CalcNode.h"
#include "../Logging.h"
#include "../Exception.h"
#include "../util/ElasticMatrix3D.h"
#include "../mesh/TetrMeshFirstOrder.h"
#include "../mesh/TetrMeshSecondOrder.h"

namespace gcm
{

	class InterpolationFixedAxis : public NumericalMethod {
	public:
		InterpolationFixedAxis();
		~InterpolationFixedAxis();
		int getNumberOfStages();
		void doNextPartStep(CalcNode* cur_node, CalcNode* new_node, float time_step, int stage, Mesh* mesh);
		float getMaxLambda(CalcNode* node);
		inline string getType() {
			return "InterpolationFixedAxis";
		}
	protected:
		int prepare_node(CalcNode* cur_node, float time_step, int stage, Mesh* mesh);
		int find_nodes_on_previous_time_layer(CalcNode* cur_node, int stage, Mesh* mesh);
		void interpolateNode(Mesh* mesh, int tetrInd, int prevNodeInd);
		
		// Used for real node
		ElasticMatrix3D elastic_matrix3d;
		// Used for interpolated virtual node in case of contact algorithm
		ElasticMatrix3D virt_elastic_matrix3d;

		// Variables used in calculations internally

		// Delta x on previous time layer for all the omegas
		// 	omega_new_time_layer(ksi) = omega_old_time_layer(ksi+dksi)
		float dksi[9];

		// If the corresponding point on previous time layer is inner or not
		bool inner[9];

		// We will store interpolated nodes on previous time layer here
		// We know that we need five nodes for each direction (corresponding to Lambdas -C1, -C2, 0, C2, C1)
		// TODO  - We can  deal with (lambda == 0) separately
		CalcNode previous_nodes[5];

		// Outer normal at current point
		float outer_normal[3];

		// This array will link omegas with corresponding interpolated nodes they should be copied from
		int ppoint_num[9];
		
		USE_LOGGER;
	};
}

#endif	/* GCM_INTERPOLATION_FIXED_AXIS_H */

