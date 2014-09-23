#ifndef MARKEREDMESH_HPP
#define MARKEREDMESH_HPP 

#include <unordered_map>

#include "libgcm/mesh/markers/MarkeredSurface.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/util/Types.hpp"
#include "libgcm/mesh/Mesh.hpp"
#include "libgcm/Logging.hpp"

namespace gcm
{
    class MarkeredMesh: public Mesh
    {
        protected:
            LineFirstOrderInterpolator interpolator;
            MarkeredSurface surface;            
            uint meshElems;
            real elemSize;

            vector3r pivot;
            std::unordered_map<uint, uint> borderFacesMap;
            
            

            void findBorderCells();
            void fillInterior();

            void getCellCoords(vector3r p, int& i, int& j, int&k);
            
            void logMeshStats();
            void calcMinH();
            void preProcessGeometry();            
            
            USE_LOGGER;

        public:
            MarkeredMesh(const MarkeredSurface& surface, uint meshElems);
            MarkeredMesh();
            ~MarkeredMesh();
            
            uint getCellLocalIndex(const uint i, const uint j, const uint k) const;
            void getCellIndexes(uint num, uint& i, uint& j, uint &k) const;
            CalcNode& getCellByLocalIndex(const uint i, const uint j, const uint k);
            
            const vector3r& getPivot() const;
            real getElemSize() const;
            uint getMeshElemes() const;
            
            const MarkeredSurface& getMarkeredSurafce() const;
            
            void generateMesh();            
            
            float getRecommendedTimeStep();
            float getMinH();
            void doNextPartStep(float tau, int stage);
            void checkTopology(float tau);

            void findBorderNodeNormal(unsigned int border_node_index, float* x, float* y, float* z, bool debug);
            bool interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug,
                                    CalcNode& targetNode, bool& isInnerPoint);
            bool interpolateNode(CalcNode& node);
            bool interpolateBorderNode(real x, real y, real z, 
                                    real dx, real dy, real dz, CalcNode& node);

            void setMeshElems(uint meshElems);

            void setSurface(MarkeredSurface surface);
        
            void transfer(float x, float y, float z) override;
            
            virtual const SnapshotWriter& getSnaphotter() const override;
            virtual const SnapshotWriter& getDumper() const override;

            void moveCoords(float tau) override;


    };
};
#endif /* MARKEREDMESH_HPP */
