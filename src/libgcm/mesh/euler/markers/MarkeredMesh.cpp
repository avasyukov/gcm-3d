#include "libgcm/mesh/euler/markers/MarkeredMesh.hpp"
#include "libgcm/snapshot/MarkeredSurfaceSnapshotWriter.hpp"
#include "libgcm/util/Tribox.hpp"

#include <queue>
#include <tuple>

using namespace gcm;

using std::queue;
using std::vector;
using std::unordered_map;
using std::numeric_limits;

MarkeredMesh::MarkeredMesh()
{
    INIT_LOGGER("gcm.mesh.euler.markers.MarkeredMesh");
    movable = true;
}

MarkeredMesh::MarkeredMesh(MarkeredSurface surface, vector3u dimensions, vector3r cellSize, vector3r center): EulerMesh(dimensions, cellSize, center), surface(surface)
{
    INIT_LOGGER("gcm.mesh.euler.markers.MarkeredMesh");
    movable = true;
}

void MarkeredMesh::preProcessGeometry()
{
    generateMesh();
    reconstructBorder();
}

void MarkeredMesh::checkTopology(float tau) {
    reconstructBorder();
}

void MarkeredMesh::logMeshStats() {
}

const MarkeredSurface& MarkeredMesh::getSurface() {
    return surface;
}

void MarkeredMesh::setSurface(const MarkeredSurface& surface) {
    this->surface = surface;
    markersOffset.resize(surface.getNumberOfMarkerNodes());
}

void MarkeredMesh::reconstructBorder()
{
//    // mark all mesh nodes as unused
//    for (uint i = 0; i < getNodesNumber(); i++)
//    {
//        auto& node = getNodeByLocalIndex(i);
//        node.setUsed(false);
//        node.setIsBorder(false);
//    }

    const auto& faces = surface.getMarkerFaces();
    const auto& nodes = surface.getMarkerNodes();

    for (uint i = 0; i < dimensions.x; i++)
        for (uint j = 0; j < dimensions.y; j++)
            for (uint k = 0; k < dimensions.z; k++)
                cellStatus[i][j][k] = false;

    LOG_DEBUG("Searching for border cells");

    unordered_map<uint, vector<uint>> borderFacesMap;

    for (auto f: faces)
    {
        const auto& v1 = nodes[f.verts[0]];
        const auto& v2 = nodes[f.verts[1]];
        const auto& v3 = nodes[f.verts[2]];

        auto minCoords = vmin(v1.coords, v2.coords, v3.coords);
        auto maxCoords = vmax(v1.coords, v2.coords, v3.coords);

        auto minEulerCoords = getCellEulerIndexByCoords(minCoords)-vector3u(1, 1, 1);
        auto maxEulerCoords = getCellEulerIndexByCoords(maxCoords)+vector3u(1, 1, 1);

        assert_lt(minEulerCoords.x, dimensions.x);
        assert_lt(minEulerCoords.y, dimensions.y);
        assert_lt(minEulerCoords.z, dimensions.z);

        assert_lt(maxEulerCoords.x, dimensions.x);
        assert_lt(maxEulerCoords.y, dimensions.y);
        assert_lt(maxEulerCoords.z, dimensions.z);

        for (uint i = minEulerCoords.x; i <= maxEulerCoords.x; i++)
            for (uint j = minEulerCoords.y; j <= maxEulerCoords.y; j++)
                for (uint k = minEulerCoords.z; k <= maxEulerCoords.z; k++)
                {
                    auto index = vector3u(i, j, k);
                    auto cellCenter = getCellCenter(index);

                    if (triangleIntersectsBox(cellCenter, cellSize/2, v1, v2, v3))
                    {
                        borderFacesMap[getCellLocalIndexByEulerIndex(index)].push_back(f.number);
                        cellStatus[i][j][k] = true;
                    }
                }

        auto index = getCellEulerIndexByCoords(v1.coords);
        if (!cellStatus[index.x][index.y][index.z])
        {
            cellStatus[index.x][index.y][index.z] = true;
            borderFacesMap[getCellLocalIndexByEulerIndex(index)].push_back(f.number);
        }
        index = getCellEulerIndexByCoords(v2.coords);
        if (!cellStatus[index.x][index.y][index.z])
        {
            cellStatus[index.x][index.y][index.z] = true;
            borderFacesMap[getCellLocalIndexByEulerIndex(index)].push_back(f.number);
        }
        index = getCellEulerIndexByCoords(v3.coords);
        if (!cellStatus[index.x][index.y][index.z])
        {
            cellStatus[index.x][index.y][index.z] = true;
            borderFacesMap[getCellLocalIndexByEulerIndex(index)].push_back(f.number);
        }
    }
    LOG_DEBUG("Found " << borderFacesMap.size() << " border cells");

    LOG_DEBUG("Filling mesh interior");

    //auto index = surface.getRegions()[0]/2;
    //const auto& face = surface.getMarkerFaces()[index];
    //const auto& node = nodes[face.verts[0]];
    //vector3r dir;
    //findTriangleFaceNormal(nodes[face.verts[0]].coords, nodes[face.verts[1]].coords, nodes[face.verts[2]].coords, &dir.x, &dir.y, &dir.z);

    //auto cellIndex = getCellEulerIndexByCoords(node.coords);
    auto cellIndex = getCellEulerIndexByCoords(surface.getAABB().getCenter());

    //dir.normalize();

    //cellIndex.x -= sgn(dir.x)*ceil(fabs(dir.x));
    //cellIndex.y -= sgn(dir.y)*ceil(fabs(dir.y));
    //cellIndex.z -= sgn(dir.z)*ceil(fabs(dir.z));

    queue<vector3u> fill_queue;

    fill_queue.push(cellIndex);

    vector3i neighbs[] =
    {
        {-1, 0, 0},
        {1, 0, 0},
        {0, -1, 0},
        {0, 1, 0},
        {0, 0, -1},
        {0, 0, 1},
    };

    uint innerCells = 0;

    while (!fill_queue.empty())
    {
        auto next = fill_queue.front();
        fill_queue.pop();

        if (cellStatus[next.x][next.y][next.z])
            continue;
        cellStatus[next.x][next.y][next.z] = true;
        innerCells++;

        for (auto& neigh: neighbs)
        {
            auto _next = next + neigh;

            assert_gt(_next.x, 0);
            assert_gt(_next.y, 0);
            assert_gt(_next.z, 0);

            assert_lt(_next.x, dimensions.x);
            assert_lt(_next.y, dimensions.y);
            assert_lt(_next.z, dimensions.z);

            if (!cellStatus[_next.x][_next.y][_next.z])
                fill_queue.push(_next);
        }
    }

    LOG_DEBUG("Found " << innerCells << " inner cells");

    outline.minX = numeric_limits<float>::infinity();
    outline.minY = numeric_limits<float>::infinity();
    outline.minZ = numeric_limits<float>::infinity();

    outline.maxX = -outline.minX;
    outline.maxY = -outline.minY;
    outline.maxZ = -outline.minZ;

    vector<int> nodesToFix;

    unordered_map<int, bool> wasUsed;

    for (int i = 1; i < nodeDimensions.x-1; i++)
        for (int j = 1; j < nodeDimensions.y-1; j++)
            for (int k = 1; k < nodeDimensions.z-1; k++)
            {
                auto& node = getNodeByEulerMeshIndex(vector3u(i, j, k));
                auto _used = node.isUsed();
                wasUsed[node.number] = _used;

                node.setUsed(false);
                node.setIsBorder(false);

                uint usedCells = 0;
                for (uint p = 0; p <= 1; p++)
                    for (uint q = 0; q <= 1; q++)
                        for (uint s = 0; s <= 1; s++)
                            if (cellStatus[i-p][j-q][k-s])
                                usedCells++;
                if (usedCells)
                {
                    node.setUsed(true);
                    if (!_used && initialized)
                        nodesToFix.push_back(node.number);
                }
                if (usedCells != 0 && usedCells != 8)
                {
                    node.setIsBorder(true);

                    if (node.coords.x < outline.minX)
                        outline.minX = node.coords.x;
                    if (node.coords.y < outline.minY)
                        outline.minY = node.coords.y;
                    if (node.coords.z < outline.minZ)
                        outline.minZ = node.coords.z;
                    if (node.coords.x > outline.maxX)
                        outline.maxX = node.coords.x;
                    if (node.coords.y > outline.maxY)
                        outline.maxY = node.coords.y;
                    if (node.coords.z > outline.maxZ)
                        outline.maxZ = node.coords.z;

                    vector3r norm;
                    uint cnt = 0;
                    for (uint p = 0; p <= 1; p++)
                        for (uint q = 0; q <= 1; q++)
                            for (uint s = 0; s <= 1; s++)
                            {
                                auto index = getCellLocalIndexByEulerIndex(vector3u(i-p, j-q, k-s));
                                for (auto fnum: borderFacesMap[index])
                                {
                                    vector3r _norm;
                                    findTriangleFaceNormal((const real *)nodes[faces[fnum].verts[0]].coords, (const real *)nodes[faces[fnum].verts[1]].coords, (const real *)nodes[faces[fnum].verts[2]].coords, &_norm.x, &_norm.y, &_norm.z);
                                    norm += _norm;
                                    cnt++;
                                }
                            }
                    assert_gt(cnt, 0);
                    norm /= cnt;
                    norm.normalize();
                    borderNormals[node.number] = norm;
                }

            }
    LOG_DEBUG("Fixing values at " << nodesToFix.size() << " nodes");

    auto findNeighb = [this, &wasUsed](const vector3u& index) -> const CalcNode&
    {
        int x = index.x;
        int y = index.y;
        int z = index.z;

        for (int i = -1; i <= 1; i+=2)
            for (int j = -1; j <= 1; j+=2)
                for (int k = -1; k <= 1; k+=2)
                {
                    int _x = x + i;
                    int _y = y + j;
                    int _z = z + k;

                    assert_ge(_x, 0);
                    assert_ge(_y, 0);
                    assert_ge(_z, 0);

                    assert_lt(_x, nodeDimensions.x);
                    assert_lt(_y, nodeDimensions.y);
                    assert_lt(_z, nodeDimensions.z);

                    const auto& node  = this->getNodeByEulerMeshIndex(vector3u(_x, _y, _z));

                    if (wasUsed[node.number])
                        return node;
                }

        THROW_BAD_MESH("Can't find used neighbour node ");
    };

    for (auto idx: nodesToFix)
    {
        auto& node = getNode(idx);
        vector3u index;

        auto result = getNodeEulerMeshIndex(node, index);
        assert_true(result);

        const auto& neighb = findNeighb(index);

        node.sxx = neighb.sxx;
        node.sxy = neighb.sxy;
        node.sxz = neighb.sxz;
        node.syy = neighb.syy;
        node.syz = neighb.syz;
        node.szz = neighb.szz;

        node.vx = neighb.vx;
        node.vy = neighb.vy;
        node.vz = neighb.vz;
    }
    initialized = true;
}

void MarkeredMesh::transfer(float x, float y, float z)
{
    EulerMesh::transfer(x, y, z);

    for (uint q = 0; q < surface.getNumberOfMarkerNodes(); q++)
        surface.moveMarker(q, vector3r(x, y, z));

    surface.updateAABB();

}

void MarkeredMesh::moveCoords(float tau) {
    const auto& markers = surface.getMarkerNodes();
    for (uint q = 0; q < markers.size(); q++)
    {
        const auto& marker = markers[q];
        auto index = getCellEulerIndexByCoords(marker.coords);

        assert_true(cellStatus[index.x][index.y][index.z]);

        auto& n000 = getNodeByEulerMeshIndex(index);
        auto& n010 = getNodeByEulerMeshIndex(index+vector3u(0, 1, 0));
        auto& n110 = getNodeByEulerMeshIndex(index+vector3u(1, 1, 0));
        auto& n100 = getNodeByEulerMeshIndex(index+vector3u(1, 0, 0));
        auto& n001 = getNodeByEulerMeshIndex(index+vector3u(0, 0, 1));
        auto& n011 = getNodeByEulerMeshIndex(index+vector3u(0, 1, 1));
        auto& n111 = getNodeByEulerMeshIndex(index+vector3u(1, 1, 1));
        auto& n101 = getNodeByEulerMeshIndex(index+vector3u(1, 0, 1));

        vector3r v;

        interpolateBox(n000.coords.x, n000.coords.y, n000.coords.z, n111.coords.x, n111.coords.y, n111.coords.z, marker.coords.x, marker.coords.y, marker.coords.z, n000.velocity, n001.velocity, n010.velocity, n011.velocity, n100.velocity, n101.velocity, n110.velocity, n111.velocity, v, 3);

        auto& offset = markersOffset[q];
        offset += v*tau;
        if (offset.length() > /*cellSize.length()/2*/ 0.0)
        {
        	surface.moveMarker(q, offset);
        	offset = vector3r(0, 0, 0);
//        	assert_true(false);
        }
    }
    surface.updateAABB();

}

const SnapshotWriter& MarkeredMesh::getSnapshotter2() const {
    return MarkeredSurfaceSnapshotWriter::getInstance();
}
