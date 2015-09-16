#ifndef GCM_RECTANGULARCUT_CUBIC_MESH_H_
#define GCM_RECTANGULARCUT_CUBIC_MESH_H_

#include "libgcm/mesh/cube/BasicCubicMesh.hpp"

namespace gcm
{
    class RectangularCutCubicMesh: public BasicCubicMesh {
    protected:
		AABB cutArea;
		
        void preProcessGeometry() override;
		
		int findNeighbourPoint(CalcNode& node, float dx, float dy, float dz, 
		                       bool debug, float* coords, bool* innerPoint);
        USE_LOGGER;

    public:
		RectangularCutCubicMesh();
        void doNextPartStep(float tau, int stage) override;
		
		void findBorderNodeNormal(const CalcNode& node, float* x, float* y, float* z, bool debug) override;

        bool interpolateBorderNode(real x, real y, real z, 
                                real dx, real dy, real dz, CalcNode& node) override;
		
		void transfer(float x, float y, float z) override;
		
		void setCutArea(const AABB& area) {
			cutArea = area;
		};
    };
}

#endif
