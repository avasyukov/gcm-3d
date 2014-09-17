#ifndef MARKEREDSURFACE_HPP
#define MARKEREDSURFACE_HPP 

#include <vector>

#include "libgcm/elem/TriangleFirstOrder.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/util/AABB.hpp"
#include "libgcm/util/Types.hpp"

namespace gcm
{
    class MarkeredSurface
    {
        protected:
            std::vector<CalcNode> markers;
            std::vector<TriangleFirstOrder> faces;
            std::vector<int> regions;
            AABB aabb;
            
            void updateAABB();
        public:
            MarkeredSurface();
            MarkeredSurface(std::vector<CalcNode>& markers, std::vector<TriangleFirstOrder>& faces, std::vector<int>& regions);

            const std::vector<CalcNode>& getMarkerNodes() const;
            unsigned int getNumberOfMarkerNodes() const;
            const std::vector<TriangleFirstOrder>& getMarkerFaces() const;
            unsigned int getNumberOfMarkerFaces() const;

            const AABB& getAABB() const;
            
            const std::vector<int> getRegions() const;

            void moveMarker(uint index, const vector3r& ds);
    };
};

#endif /* MARKEREDSURFACE_HPP */
