#include "libgcm/mesh/markers/MarkeredMesh.hpp"

#include "libgcm/util/Assertion.hpp"
#include "libgcm/util/Tribox.hpp"
#include "libgcm/snapshot/VTKMarkeredMeshSnapshotWriter.hpp"

#include <algorithm>
#include <queue>
#include <tuple>

using namespace gcm;


MarkeredMesh::MarkeredMesh(const MarkeredSurface& surface, uint meshElems): MarkeredMesh()
{
    setSurface(surface);
    setMeshElems(meshElems);
    generateMesh();
}



MarkeredMesh::MarkeredMesh()
{
    numericalMethodType = "InterpolationFixedAxis";
    
    INIT_LOGGER("gcm.mesh.markers.MarkeredMesh");
}

MarkeredMesh::~MarkeredMesh()
{
}

uint MarkeredMesh::getCellLocalIndex(const uint i, const uint j, const uint k) const
{
    return i*meshElems*meshElems + j*meshElems + k;
}

void MarkeredMesh::getCellIndexes(uint num, uint& i, uint& j, uint &k) const
{
    i = num / (meshElems*meshElems);
    j = (num/meshElems) % meshElems;
    k = num % meshElems;
}

CalcNode& MarkeredMesh::getCellByLocalIndex(const uint i, const uint j, const uint k)
{
    return getNodeByLocalIndex(getCellLocalIndex(i, j, k));
}

void MarkeredMesh::generateMesh()
{  
    AABB aabb = surface.getAABB();

    auto l1 = aabb.maxX - aabb.minX;
    auto l2 = aabb.maxY - aabb.minY;
    auto l3 = aabb.maxZ - aabb.minZ;

    auto l = max({l1, l2, l3})*1.2;

    elemSize = l / meshElems;
    
    LOG_INFO("Generating mesh with size " << meshElems << "x" << meshElems << "x" << meshElems << " with element size " << elemSize);

    vector3r center = aabb.getCenter();

    pivot = center - vector3r(1, 1, 1)*l/2;
    vector3r point  = pivot + vector3r(elemSize, elemSize, elemSize)/2;
    
    uint idx = 0;
    
    CalcNode node;    
    node.setPlacement(true);
    node.setIsBorder(false);
    node.setUsed(false);
    for (unsigned int i = 0; i < meshElems; i++)
    {
        for (unsigned int j = 0; j < meshElems; j++) 
        {
            for (unsigned int k = 0; k < meshElems; k++)
            {
                node.x = point.x + i*elemSize;
                node.y = point.y + j*elemSize;
                node.z = point.z + k*elemSize;
                node.number = idx++;
                addNode(node);                
            }
        }
    }
}

void MarkeredMesh::getCellCoords(vector3r p, int& i, int& j, int&k){
    vector3r a = (p-pivot)/elemSize;

    i = floor(a.x);
    j = floor(a.y);
    k = floor(a.z);
}

void MarkeredMesh::findBorderCells()
{
    LOG_DEBUG("Searching for border cells");
    const auto& faces = surface.getMarkerFaces();
    const auto& nodes = surface.getMarkerNodes();

    for (auto f: faces)
    {
        const auto& v1 = nodes[f.verts[0]];
        const auto& v2 = nodes[f.verts[1]];
        const auto& v3 = nodes[f.verts[2]];

        vector3r planePoint1(v1.x, v1.y, v1.z);
        vector3r planePoint2(v2.x, v2.y, v2.z);
        vector3r planePoint3(v3.x, v3.y, v3.z);
        vector3r planeNormal;

        findTriangleFaceNormal(v1.coords, v2.coords, v3.coords, &planeNormal.x, &planeNormal.y, &planeNormal.z);

        auto minX = min({v1.x, v2.x, v3.x});
        auto maxX = max({v1.x, v2.x, v3.x});
        auto minY = min({v1.y, v2.y, v3.y});
        auto maxY = max({v1.y, v2.y, v3.y});
        auto minZ = min({v1.z, v2.z, v3.z});
        auto maxZ = max({v1.z, v2.z, v3.z});

        uint minI = floor((minX-pivot.x) / elemSize);
        uint minJ = floor((minY-pivot.y) / elemSize);
        uint minK = floor((minZ-pivot.z) / elemSize);

        uint maxI = ceil((maxX-pivot.x) / elemSize)+1;
        uint maxJ = ceil((maxY-pivot.y) / elemSize)+1;
        uint maxK = ceil((maxZ-pivot.z) / elemSize)+1;

        for (uint i = minI; i < maxI; i++)
            for (uint j = minJ; j < maxJ; j++)
                for (uint k = minK; k < maxK; k++)
                {
                    auto& cell = getCellByLocalIndex(i, j, k);

                    if (triangleIntersectsCube(cell, elemSize/2, v1, v2, v3))
                    {
                        borderFacesMap[cell.number] = f.number;
                        cell.setIsBorder(true);
                        cell.setPlacement(true);
                    }
                }
    }
    
    LOG_DEBUG("Found " << borderFacesMap.size() << " border cells");
}

void MarkeredMesh::fillInterior()
{
    LOG_DEBUG("Filling mesh interior");
    
    for (uint i = 0; i < meshElems; i++)
        for (uint j = 0; j < meshElems; j++)
            for (uint k = 0; k < meshElems; k++)
            {
                auto& cell = getCellByLocalIndex(i, j, k);
                if (!cell.isBorder())
                    cell.setUsed(false);
            }

    auto index = surface.getRegions()[0]/2;
    const auto& face = surface.getMarkerFaces()[index];
    const auto& nodes = surface.getMarkerNodes();
    const auto& node = nodes[face.verts[0]];
    vector3r dir;
    findTriangleFaceNormal(nodes[face.verts[0]].coords, nodes[face.verts[1]].coords, nodes[face.verts[2]].coords, &dir.x, &dir.y, &dir.z);
    
    int p, q, s;
    
    getCellCoords(vector3r(node.x, node.y, node.z), p, q, s);
    
    dir.normalize();
    
    p -= sgn(dir.x)*ceil(fabs(dir.x));
    q -= sgn(dir.y)*ceil(fabs(dir.y));
    s -= sgn(dir.z)*ceil(fabs(dir.z));
    
    queue<tuple<int, int, int>> fill_queue;

    fill_queue.push(make_tuple(p, q, s));

    tuple<int,int,int> neighbs[] = 
    {
        make_tuple(-1, 0, 0),
        make_tuple(1, 0, 0),
        make_tuple(0, -1, 0),
        make_tuple(0, 1, 0),
        make_tuple(0, 0, -1),
        make_tuple(0, 0, 1),
    };
    
    uint count = 0;
    
    while (!fill_queue.empty())
    {
        const auto& next = fill_queue.front();

        auto _p = get<0>(next);
        auto _q = get<1>(next);
        auto _s = get<2>(next);
        
        fill_queue.pop();
        
        auto& cell = getCellByLocalIndex(_p, _q, _s);

        if (cell.isUsed())
            continue;
        cell.setUsed(true);
        cell.setPlacement(true);
        count++;
        
        for (auto& neigh: neighbs)
        {

            int p = _p + get<0>(neigh);
            int q = _q + get<1>(neigh);
            int s = _s + get<2>(neigh);
            
            assert_gt(p, 0);
            assert_gt(q, 0);
            assert_gt(s, 0);
            
            assert_lt(p, meshElems-1);
            assert_lt(q, meshElems-1);
            assert_lt(s, meshElems-1);

            if (!getCellByLocalIndex(p, q, s).isBorder())
                fill_queue.push(make_tuple(p, q, s));
        }
    }
    
    LOG_DEBUG("Found " << count << " inner cells");
}

float MarkeredMesh::getRecommendedTimeStep()
{
    return 0.9*getMinH()/getMaxEigenvalue();
}

float MarkeredMesh::getMinH()
{
    return elemSize;
}

void MarkeredMesh::doNextPartStep(float tau, int stage)
{
    defaultNextPartStep(tau, stage);
}

void MarkeredMesh::checkTopology(float tau)
{
    preProcessGeometry();
}

void MarkeredMesh::findBorderNodeNormal(unsigned int border_node_index, float* x, float* y, float* z, bool debug)
{
    assert_ge(border_node_index, 0);
    assert_lt(border_node_index, nodes.size());
    assert_true(nodes[border_node_index].isBorder());
    
    const auto& face = surface.getMarkerFaces()[borderFacesMap[border_node_index]];
    const auto& _nodes = surface.getMarkerNodes();
    
    findTriangleFaceNormal(_nodes[face.verts[0]].coords, _nodes[face.verts[1]].coords, _nodes[face.verts[2]].coords, x, y, z);
}

bool MarkeredMesh::interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug,
                        CalcNode& targetNode, bool& isInnerPoint)
{
    assert_true(((dx == 0.0) && (dy == 0.0)) || ((dy == 0.0) && (dz == 0.0)) || ((dx == 0.0) && (dz == 0.0)));
    
    int i1, i2, i3;

    vector3r coords(origin.x+dx, origin.y+dy, origin.z+dz);
    
    getCellCoords(coords, i1, i2, i3);
    
    assert_ge(i1, 0);
    assert_ge(i2, 0);
    assert_ge(i3, 0);
    
    assert_lt(i1, meshElems);
    assert_lt(i2, meshElems);
    assert_lt(i3, meshElems);    
    
    const auto& cell = getCellByLocalIndex(i1, i2, i3);
    
    isInnerPoint = cell.isUsed() && cell.isLocal();
    if (isInnerPoint && cell.isBorder())
    {
        vector3r dir(coords.x-cell.x, coords.y-cell.y, coords.z-cell.z);
        vector3r norm;
        findBorderNodeNormal(cell.number, &norm.x, &norm.y, &norm.z, false);
        if (norm*dir > 0)
            isInnerPoint = false;
    }
    
    if (isInnerPoint) {
        auto idx = getNodeLocalIndex(origin.number);
        
        uint j1, j2, j3;

        getCellIndexes(idx, j1, j2, j3);
        
        assert_lt(j1, meshElems);
        assert_lt(j2, meshElems);
        assert_lt(j3, meshElems);            
    
        if (dx != 0.0)
            j1 += sgn(dx);
        else if (dy != 0.0)
            j2 += sgn(dy);
        else
            j3 += sgn(dz);

        targetNode.coords[0] = origin.coords[0]+dx;
        targetNode.coords[1] = origin.coords[1]+dy;
        targetNode.coords[2] = origin.coords[2]+dz;
        interpolator.interpolate(targetNode, origin, getCellByLocalIndex(j1, j2, j3));
        return true;

    } else {
        targetNode.coords[0] = origin.coords[0];
        targetNode.coords[1] = origin.coords[1];
        targetNode.coords[2] = origin.coords[2];
        return false;
    }
}

bool MarkeredMesh::interpolateNode(CalcNode& node)
{
    vector3r coords(node.x, node.y, node.z);
    
    int i, j, k;
    getCellCoords(coords, i, j, k);

    assert_ge(i, 0);
    assert_ge(j, 0);
    assert_ge(k, 0);

    assert_lt(i, meshElems);
    assert_lt(j, meshElems);
    assert_lt(k, meshElems);

    auto& cell = getCellByLocalIndex(i, j, k);

    auto dx = node.x-cell.x;
    auto dy = node.y-cell.y;
    auto dz = node.z-cell.z;

    if (fabs(dx) < EQUALITY_TOLERANCE)
        dx = 0.0;
    if (fabs(dy) < EQUALITY_TOLERANCE)
        dy = 0.0;
    if (fabs(dz) < EQUALITY_TOLERANCE)
        dz = 0.0;
    bool isInnerPoint;
    interpolateNode(cell, dx, dy, dz, false, node, isInnerPoint);

    return true;
}

bool MarkeredMesh::interpolateBorderNode(gcm::real x, gcm::real y, gcm::real z, 
                        gcm::real dx, gcm::real dy, gcm::real dz, CalcNode& node)
{
    vector3r coords(x+dx, y+dy, z+dz);
    int i, j, k;
    getCellCoords(coords, i, j, k);

    assert_ge(i, 0);
    assert_ge(j, 0);
    assert_ge(k, 0);

    assert_lt(i, meshElems);
    assert_lt(j, meshElems);
    assert_lt(k, meshElems);

    auto& cell = getCellByLocalIndex(i, j, k);
    
    if (cell.isBorder())
    {
//        memcpy(node.values, cell.values, sizeof(real)*VALUES_NUMBER);
//        memcpy(node.coords, cell.coords, sizeof(float)*3);
        node = cell;
        return true; 
    }

    return false;
}

const vector3r& MarkeredMesh::getPivot() const
{
    return pivot;
}

gcm::real MarkeredMesh::getElemSize() const
{
    return elemSize;
}

uint MarkeredMesh::getMeshElemes() const
{
    return meshElems;
}

const MarkeredSurface& MarkeredMesh::getMarkeredSurafce() const
{
    return surface;
}

void MarkeredMesh::logMeshStats()
{
    
}
void MarkeredMesh::calcMinH()
{
}

void MarkeredMesh::preProcessGeometry()
{
    findBorderCells();
    fillInterior();
}

void MarkeredMesh::setMeshElems(uint meshElems) {
    this->meshElems = meshElems;
}

void MarkeredMesh::setSurface(MarkeredSurface surface) {
    this->surface = surface;
}
            
void MarkeredMesh::transfer(float x, float y, float z)
{
    Mesh::transfer(x, y, z);
    pivot += vector3r(x, y, z);
}

const SnapshotWriter& MarkeredMesh::getSnaphotter() const
{
    return VTKMarkeredMeshSnapshotWriter::getInstance();
}

const SnapshotWriter& MarkeredMesh::getDumper() const
{
    return getSnaphotter();
}

void MarkeredMesh::moveCoords(float tau)
{
    const auto& markers = surface.getMarkerNodes();
    for (uint q = 0; q < markers.size(); q++)
    {
        const auto& marker = markers[q];
        int i, j, k;
        getCellCoords(vector3r(marker.x, marker.y, marker.z), i, j, k);

        assert_ge(i, 0);
        assert_ge(j, 0);
        assert_ge(k, 0);

        assert_lt(i, meshElems);
        assert_lt(j, meshElems);
        assert_lt(k, meshElems);

        const auto& cell = getCellByLocalIndex(i, j, k);
        assert_true(cell.isUsed());

        vector3r v = {cell.vx, cell.vy, cell.vz};

        surface.moveMarker(q, v*tau);
    }
}
