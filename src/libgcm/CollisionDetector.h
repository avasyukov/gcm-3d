#ifndef _GCM_COLLISION_DETECTOR
#define _GCM_COLLISION_DETECTOR 1

#include <vector>
#include <string>
#include "util/AABB.h"
#include "Logging.h"
#include "elem/TriangleFirstOrder.h"
#include "mesh/tetr/TetrMeshSecondOrder.h"

using namespace std;

namespace gcm {
	
	class AABB;
	class CalcNode;

	class CollisionDetector	{
		
	protected:
		USE_LOGGER;
		float threshold;
		bool static_operation;
		
	public:
		CollisionDetector();
		~CollisionDetector();
		virtual string getType () const = 0;
		void set_threshold(float value);
		float get_threshold();
		virtual void find_collisions(vector<CalcNode> &virt_nodes) = 0;
		bool find_intersection(AABB &outline1, AABB &outline2, AABB &intersection);
		// return elements that are in intersection
		void find_nodes_in_intersection(TetrMeshSecondOrder* mesh, AABB &intersection, vector<CalcNode> &result);
		void find_nodes_in_intersection(TetrMeshSecondOrder* mesh, AABB &intersection, vector<int> &result);
		void find_faces_in_intersection(TetrMeshSecondOrder* mesh, AABB &intersection, vector<TriangleFirstOrder> &result);
		void find_faces_in_intersection(TetrMeshSecondOrder* mesh, AABB &intersection, vector<int> &result);
		// number returned surface elements (nodes and triangles) sequentially
		//void renumber_surface(vector<TriangleFirstOrder> &faces, vector<CalcNode> &nodes);
		// returns surface elements (nodes and triangles) renumbered sequentially
		//void renumber_volume(vector<Tetrahedron_1st_order> &tetrs, vector<CalcNode> &nodes);
		void set_static(bool state);
		bool is_static();
	};

}

#endif
