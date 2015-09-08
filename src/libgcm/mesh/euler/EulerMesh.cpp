#include "libgcm/mesh/euler/EulerMesh.hpp"

#include "libgcm/util/Assertion.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/Math.hpp"
#include "libgcm/snapshot/VTKEulerMeshSnapshotWriter.hpp"

#include <algorithm>
#include <cmath>

using namespace gcm;
using std::min;
using std::floor;

const uchar EulerMesh::VIRT_FLAG = CalcNode::FLAG_1;
const uchar EulerMesh::VIRT_CELL_AXIS_X_FLAG = CalcNode::FLAG_2;
const uchar EulerMesh::VIRT_CELL_AXIS_Y_FLAG = CalcNode::FLAG_3;
const uchar EulerMesh::VIRT_CELL_AXIS_Z_FLAG = CalcNode::FLAG_4;

EulerMesh::EulerMesh()
{
    INIT_LOGGER("gcm.mesh.euler.EulerMesh");
    numericalMethodType = "InterpolationFixedAxis";
}

EulerMesh::~EulerMesh()
{
    if (cellStatus)
    {
        for (uint i = 0; i < dimensions.x; i++)
        {
            for (uint j = 0; j < dimensions.y; j++)
                delete[] cellStatus[i][j];
            delete[] cellStatus[i];
        }
        delete[] cellStatus;
    }
}

EulerMesh::EulerMesh(vector3u dimensions, vector3r cellSize, vector3r center): EulerMesh()
{
    setDimensions(dimensions);
    setCellSize(cellSize);
    setCenter(center);
    generateMesh();

    for (int i = 0; i < dimensions.x; i++)
        for (int j = 0; j < dimensions.y; j++)
            for (int k = 0; k < dimensions.z; k++)
                cellStatus[i][j][k] = true;

    auto& n1 = getNodeByEulerMeshIndex(vector3u(0, 0, 0));
    auto& n2 = getNodeByEulerMeshIndex(dimensions-vector3u(1, 1, 1));
    outline.minX = n1.coords.x;
    outline.minY = n1.coords.y;
    outline.minZ = n1.coords.z;
    outline.maxX = n2.coords.x;
	outline.maxY = n2.coords.y;
	outline.maxZ = n2.coords.z;
}

void EulerMesh::calcMinH()
{
    minH = min({cellSize.x, cellSize.y, cellSize.z});
}

float EulerMesh::getMinH()
{
    return minH;
}

float EulerMesh::getAvgH() {
	return minH;
}

float EulerMesh::getRecommendedTimeStep()
{
    return getMinH()/getMaxEigenvalue();
}

void EulerMesh::doNextPartStep(float tau, int stage)
{
    defaultNextPartStep(tau, stage);
}

const SnapshotWriter& EulerMesh::getSnaphotter() const
{
    return VTKEulerMeshSnapshotWriter::getInstance();
}

const SnapshotWriter& EulerMesh::getDumper() const
{
    return VTKEulerMeshSnapshotWriter::getInstance();
}

void EulerMesh::findBorderNodeNormal(const CalcNode& node, float* x, float* y, float* z, bool debug)
{
    assert_true(borderNormals.find(node.number) != borderNormals.end(),
        {
            LOG_DEBUG(node);
        }
    );

    auto norm = borderNormals[node.number];
    *x = norm.x;
    *y = norm.y;
    *z = norm.z;
}

bool EulerMesh::interpolateBorderNode(real x, real y, real z, real dx, real dy, real dz, CalcNode& node)
{
    assert_true(
        ((dx == 0.0) && (dy == 0.0)) ||
        ((dy == 0.0) && (dz == 0.0)) ||
        ((dx == 0.0) && (dz == 0.0))
    );

    vector3r coords(x, y, z);

    auto index0 = getCellEulerIndexByCoordsUnsafe(coords);

    coords += vector3r(dx, dy, dz);
    auto index = getCellEulerIndexByCoordsUnsafe(coords);


    auto dindex = index - index0;
    int l = dindex.normalize();


    bool found = false;
    vector3i cmin(0, 0, 0);
    vector3i cmax(dimensions.x-1, dimensions.y-1, dimensions.z-1);
    for (int i = 0; i <= l; i++)
    {
    	if ((index0 >>= cmin) && (index0 <<= cmax))
			if (cellStatus[index0.x][index0.y][index0.z])
			{
				found = true;
				break;
			}
    	index0 += dindex;
    }

    if (!found)
    	return false;

    index = index0;

    real x1, x2, y1, y2, _x, _y;
    real* q11;
    real* q21;
    real* q22;
    real* q12;

    uint d;

    if (dx != 0.0)
    {
        d = dx > 0.0 ? 0 : 1;
        auto& _node = getNodeByEulerMeshIndex(vector3u(index.x+d, index.y, index.z));
        assert_true(_node.isBorder());

        x1 = _node.coords.y;
        y1 = _node.coords.z;
        x2 = x1 + cellSize.y;
        y2 = y1 + cellSize.z;

        q11 = _node.values;
        q21 = getNodeByEulerMeshIndex(vector3u(index.x+d, index.y+1, index.z)).values;
        q22 = getNodeByEulerMeshIndex(vector3u(index.x+d, index.y+1, index.z+1)).values;
        q12 = getNodeByEulerMeshIndex(vector3u(index.x+d, index.y, index.z+1)).values;

        node.coords.x = _node.coords.x;
        _x = node.coords.y = coords.y;
        _y = node.coords.z = coords.z;

        node.setMaterialId(_node.getMaterialId());
        node.setRheologyMatrix(_node.getRheologyMatrix());
        node.number = _node.number;

        node.setCustomFlag(VIRT_CELL_AXIS_X_FLAG, d);
        node.setCustomFlag(VIRT_CELL_AXIS_Y_FLAG, 0);
        node.setCustomFlag(VIRT_CELL_AXIS_Z_FLAG, 0);
    } else if (dy != 0.0)
    {
        d = dy > 0.0 ? 0 : 1;
        auto& _node = getNodeByEulerMeshIndex(vector3u(index.x, index.y+d, index.z));
        assert_true(_node.isBorder());

        x1 = _node.coords.x;
        y1 = _node.coords.z;
        x2 = x1 + cellSize.x;
        y2 = y1 + cellSize.z;

        q11 = _node.values;
        q21 = getNodeByEulerMeshIndex(vector3u(index.x+1, index.y+d, index.z)).values;
        q22 = getNodeByEulerMeshIndex(vector3u(index.x+1, index.y+d, index.z+1)).values;
        q12 = getNodeByEulerMeshIndex(vector3u(index.x, index.y+d, index.z+1)).values;

        _x = node.coords.x = coords.x;
        node.coords.y = _node.coords.y;
        _y = node.coords.z = coords.z;

        node.setMaterialId(_node.getMaterialId());
        node.setRheologyMatrix(_node.getRheologyMatrix());
        node.number = _node.number;

        node.setCustomFlag(VIRT_CELL_AXIS_X_FLAG, 0);
        node.setCustomFlag(VIRT_CELL_AXIS_Y_FLAG, d);
        node.setCustomFlag(VIRT_CELL_AXIS_Z_FLAG, 0);
    } else
    {
        d = dz > 0.0 ? 0 : 1;
        auto& _node = getNodeByEulerMeshIndex(vector3u(index.x, index.y, index.z+d));
        assert_true(_node.isBorder());

        x1 = _node.coords.x;
        y1 = _node.coords.y;
        x2 = x1 + cellSize.x;
        y2 = y1 + cellSize.y;

        q11 = _node.values;
        q21 = getNodeByEulerMeshIndex(vector3u(index.x+1, index.y, index.z+d)).values;
        q22 = getNodeByEulerMeshIndex(vector3u(index.x+1, index.y+1, index.z+d)).values;
        q12 = getNodeByEulerMeshIndex(vector3u(index.x, index.y+1, index.z+d)).values;

        _x = node.coords.x = coords.x;
        _y = node.coords.y = coords.y;
        node.coords.z = _node.coords.z;

        node.setMaterialId(_node.getMaterialId());
        node.setRheologyMatrix(_node.getRheologyMatrix());
        node.number = _node.number;

        node.setCustomFlag(VIRT_CELL_AXIS_X_FLAG, 0);
        node.setCustomFlag(VIRT_CELL_AXIS_Y_FLAG, 0);
        node.setCustomFlag(VIRT_CELL_AXIS_Z_FLAG, d);
    }

    interpolateRectangle(x1, y1, x2, y2, _x, _y, q11, q12, q22, q21, node.values, VALUES_NUMBER);
    node.setIsBorder(true);

    return true;
}


bool EulerMesh::interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug, CalcNode& targetNode, bool& isInnerPoint)
{
    targetNode.coords = origin.coords+vector3r(dx, dy, dz);

    if (!origin.getCustomFlag(VIRT_FLAG))
    {
        isInnerPoint = interpolateNode(targetNode);
        return isInnerPoint;
    } else
    {
        assert_true(
            (dx == 0.0 && dy == 0.0) ||
            (dz == 0.0 && dy == 0.0) ||
            (dx == 0.0 && dz == 0.0)
        );

        vector3u index;
        bool inner = getNodeEulerMeshIndex(origin, index);
        assert_true(inner);
        auto d1 = origin.getCustomFlag(VIRT_CELL_AXIS_X_FLAG);
        auto d2 = origin.getCustomFlag(VIRT_CELL_AXIS_Y_FLAG);
        auto d3 = origin.getCustomFlag(VIRT_CELL_AXIS_Z_FLAG);
        index.x -= d1;
        index.y -= d2;
        index.z -= d3;
        assert_true(cellStatus[index.x][index.y][index.z]);
        if (dx != 0.0)
        {
            isInnerPoint = ((d1 == 1 && dx < 0.0) || (d1 == 0 && dx > 0.0));
        } else if (dy != 0.0)
        {
            isInnerPoint = ((d2 == 1 && dy < 0.0) || (d2 == 0 && dy > 0.0));
        } else
        {
            isInnerPoint = ((d3 == 1 && dz < 0.0) || (d3 == 0 && dz > 0.0));
        }
        if (isInnerPoint)
        {
            auto res = interpolateNode(targetNode, index);
            assert_true(res);
            return true;
        }
        else
            return false;
    }
}

bool EulerMesh::getNodeEulerMeshIndex(const CalcNode& node, vector3u& indexes) const
{
    const auto& c1 = const_cast<EulerMesh*>(this)->getNodeByEulerMeshIndex(vector3u(0, 0, 0)).coords;
    const auto& c2 = const_cast<EulerMesh*>(this)->getNodeByEulerMeshIndex(nodeDimensions-vector3u(1, 1, 1)).coords;

    if (node.coords < c1 || node.coords > c2)
        return false;

    auto localNumber = Mesh::getNodeLocalIndex(node.number);

    uint t = localNumber % (nodeDimensions.y*nodeDimensions.z);

    indexes.x = localNumber / (nodeDimensions.y*nodeDimensions.z);
    indexes.y = t / nodeDimensions.z;
    indexes.z = t % nodeDimensions.z;

    return true;
}

int EulerMesh::getNodeLocalIndexByEulerMeshIndex(const vector3u& index) const
{
    return index.z + index.y*nodeDimensions.z + index.x*nodeDimensions.y*nodeDimensions.z;
}

CalcNode& EulerMesh::getNodeByEulerMeshIndex(const vector3u& index) {
    return Mesh::getNodeByLocalIndex(getNodeLocalIndexByEulerMeshIndex(index));
}

void EulerMesh::setCellSize(const vector3r& cellSize)
{
    assert_gt(cellSize.x, 0);
    assert_gt(cellSize.y, 0);
    assert_gt(cellSize.z, 0);

    this->cellSize = cellSize;
}

void EulerMesh::setDimensions(const vector3u& dimensions)
{
    assert_gt(dimensions.x, 0);
    assert_gt(dimensions.y, 0);
    assert_gt(dimensions.z, 0);

    this->dimensions = dimensions;
    nodeDimensions = dimensions + vector3u(1, 1, 1);
}

void EulerMesh::setCenter(const vector3r& center)
{
    this->center = center;
}

void EulerMesh::generateMesh() {
    // generate mesh
    auto halfSize = cellSize%dimensions/2;
    auto point = center-halfSize;

    LOG_DEBUG("Generating Euler mesh with AABB: [" << point << ", " << point + 2*halfSize << "]");
    LOG_DEBUG("Mesh center: " << center);
    LOG_DEBUG("Mesh dimensions: " << dimensions);
    LOG_DEBUG("Mesh element size: " << cellSize);

    uint nodesNumber = (dimensions.x+1)*(dimensions.y+1)*(dimensions.z+1);
    LOG_DEBUG("Total number of node: " << nodesNumber);
    nodes.reserve(nodesNumber);

    CalcNode node;
    node.setPlacement(true);
    // FIXME do we actually need this?
    node.setUsed(false);
    int index = 0;
    for (uint i = 0; i <= dimensions.x; i++)
        for (uint j = 0; j <= dimensions.y; j++)
            for (uint k = 0; k <= dimensions.z; k++)
            {
                node.coords = point + cellSize%vector3u(i, j, k);
                node.number = index++;
                addNode(node);
            }

    cellStatus = new uchar**[dimensions.x];
    for (uint i = 0; i < dimensions.x; i++)
    {
        cellStatus[i] = new uchar*[dimensions.y];
        for (uint j = 0; j < dimensions.y; j++)
            cellStatus[i][j] = new uchar[dimensions.z];
    }

}

vector3u EulerMesh::getCellEulerIndexByCoords(const vector3r& coords) const
{
    vector3i index = getCellEulerIndexByCoordsUnsafe(coords);

    assert_ge(index.x, 0);
    assert_ge(index.y, 0);
    assert_ge(index.z, 0);

    assert_lt(index.x, dimensions.x);
    assert_lt(index.y, dimensions.y);
    assert_lt(index.z, dimensions.z);

    return vector3u(index.x, index.y, index.z);
}

vector3r EulerMesh::getCellCenter(const vector3u& index) const {
    assert_lt(index.x, dimensions.x);
    assert_lt(index.y, dimensions.y);
    assert_lt(index.z, dimensions.z);

    return const_cast<EulerMesh*>(this)->getNodeByEulerMeshIndex(index).coords+cellSize/2;
}

uint EulerMesh::getCellLocalIndexByEulerIndex(const vector3u& index) const
{
    return index.z + index.y*dimensions.z + index.x*dimensions.y*dimensions.z;
}


const vector3r& EulerMesh::getCellSize() const
{
    return cellSize;
}

const vector3u& EulerMesh::getDimensions() const
{
    return dimensions;
}

const vector3u& EulerMesh::getNodeDimensions() const
{
    return nodeDimensions;
}

vector3i EulerMesh::getCellEulerIndexByCoordsUnsafe(const vector3r& coords) const
{
    const vector3r& p = const_cast<EulerMesh*>(this)->getNodeByEulerMeshIndex(vector3u(0, 0, 0)).coords;

    vector3r index = (coords-p)/cellSize;

    return vector3i(floor(index.x), floor(index.y), floor(index.z));
}

uchar EulerMesh::getCellStatus(const vector3u& index) const
{
    assert_lt(index.x, dimensions.x);
    assert_lt(index.y, dimensions.y);
    assert_lt(index.z, dimensions.z);

    return cellStatus[index.x][index.y][index.z];
}

bool EulerMesh::interpolateNode(CalcNode& node) {
	if (!outline.isInAABB(node))
		return false;
    auto index = getCellEulerIndexByCoords(node.coords);

    return interpolateNode(node, index);
}

bool EulerMesh::interpolateNode(CalcNode& node, const vector3u &index) {
    if (!cellStatus[index.x][index.y][index.z])
        return false;

    auto& n000 = getNodeByEulerMeshIndex(index);
    auto& n010 = getNodeByEulerMeshIndex(index+vector3u(0, 1, 0));
    auto& n110 = getNodeByEulerMeshIndex(index+vector3u(1, 1, 0));
    auto& n100 = getNodeByEulerMeshIndex(index+vector3u(1, 0, 0));
    auto& n001 = getNodeByEulerMeshIndex(index+vector3u(0, 0, 1));
    auto& n011 = getNodeByEulerMeshIndex(index+vector3u(0, 1, 1));
    auto& n111 = getNodeByEulerMeshIndex(index+vector3u(1, 1, 1));
    auto& n101 = getNodeByEulerMeshIndex(index+vector3u(1, 0, 1));

    interpolateBox(n000.coords.x, n000.coords.y, n000.coords.z, n111.coords.x, n111.coords.y, n111.coords.z, node.coords.x, node.coords.y, node.coords.z, n000.values, n001.values, n010.values, n011.values, n100.values, n101.values, n110.values, n111.values, node.values, VALUES_NUMBER);
    node.setMaterialId(n000.getMaterialId());
    node.setRheologyMatrix(n000.getRheologyMatrix());

    return true;
}
