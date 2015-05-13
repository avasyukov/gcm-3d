#include "libgcm/mesh/euler/markers/MarkeredSurface.hpp"

#include <algorithm>

using namespace gcm;
using std::vector;
using std::pair;
using std::string;
using std::move;

MarkeredSurface::MarkeredSurface()
{
}
MarkeredSurface::MarkeredSurface(vector<CalcNode>& markers, vector<TriangleFirstOrder>& faces, vector<int>& regions, vector<pair<string, TetrMeshFirstOrder*>> meshes)
{
    this->markers = move(markers);
    this->faces = move(faces);
    this->regions = move(regions);
    this->meshes = meshes;

    updateAABB();
}

const vector<CalcNode>& MarkeredSurface::getMarkerNodes() const
{
    return markers;
}

unsigned int MarkeredSurface::getNumberOfMarkerNodes() const
{
    return markers.size();
}

const vector<TriangleFirstOrder>& MarkeredSurface::getMarkerFaces() const
{
    return faces;
}

unsigned int MarkeredSurface::getNumberOfMarkerFaces() const
{
    return faces.size();
}

const AABB& MarkeredSurface::getAABB() const
{
    return aabb;
}

void MarkeredSurface::updateAABB()
{
    AABB aabb;

    for (auto m: markers)
        for (int i = 0; i < 3; i++)
        {
            if (m.coords[i] < aabb.min_coords[i])
                aabb.min_coords[i] = m.coords[i];
            if (m.coords[i] > aabb.max_coords[i])
                aabb.max_coords[i] = m.coords[i];
        }

    this->aabb = aabb;
}

const std::vector<  int> MarkeredSurface::getRegions() const
{
    return regions;
}


void MarkeredSurface::moveMarker(uint index, const vector3r& ds)
{
    assert_lt(index, markers.size());
    auto& marker = markers[index];

    marker.coords += ds;
}
