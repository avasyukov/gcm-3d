#include "mesh/markers/MarkeredMesh.h"

#include <cmath>

#include "Math.h"

float gcm::MarkeredMesh::getRecommendedTimeStep() {
	return getMinH()/getMaxLambda();
}


float gcm::MarkeredMesh::getMinH() {
	return min({h[0], h[1], h[2]});
}


void gcm::MarkeredMesh::doNextPartStep(float tau, int stage) {
	defaultNextPartStep(tau, stage);
}


void gcm::MarkeredMesh::checkTopology(float tau) {
}

void gcm::MarkeredMesh::logMeshStats() {
}

void gcm::MarkeredMesh::preProcessGeometry() {
	for (int i = 0; i < 3; i++)
		h[i] = (meshOutline.max_coords[i]-meshOutline.min_coords[i]) / cells_num[i];

	generateMarkers();
	reconstructBorder();
	markUnusedNodes();
}

void gcm::MarkeredMesh::setNumberOfCells(unsigned int num1, unsigned int num2, unsigned int num3) {
	cells_num[0] = num1;
	cells_num[1] = num2;
	cells_num[2] = num3;

	points_num[0] = num1+1;
	points_num[1] = num2+1;
	points_num[2] = num3+1;

	cellsInnerFlags.clear();
	cellsInnerFlags.resize(num1*num2*num3, false);
}

void gcm::MarkeredMesh::getCellIndexes(unsigned int num, unsigned int &i1, unsigned int &i2, unsigned int &i3) {
	unsigned int t = num % (cells_num[1]*cells_num[0]);

	i1 = t % cells_num[0];
	i2 = t / cells_num[0];
	i3 = num / (cells_num[0]*cells_num[1]);
}

void gcm::MarkeredMesh::getPointIndexes(unsigned int num, unsigned int &i1, unsigned int &i2, unsigned int &i3) {
	unsigned int t = num % (points_num[1]*points_num[0]);

	i1 = t % points_num[0];
	i2 = t / points_num[0];
	i3 = num / (points_num[0]*points_num[1]);
}

void gcm::MarkeredMesh::getCellAABB(unsigned int num, AABB& aabb) {

	unsigned int i1, i2, i3;
	getCellIndexes(num, i1, i2, i3);


	aabb.minX = getNodeByLocalIndex(getPointNumber(i1, i2, i3)).x;
	aabb.maxX = getNodeByLocalIndex(getPointNumber(i1+1, i2, i3)).x;

	aabb.minY = getNodeByLocalIndex(getPointNumber(i1, i2, i3)).y;
	aabb.maxY = getNodeByLocalIndex(getPointNumber(i1, i2+1, i3)).y;

	aabb.minZ = getNodeByLocalIndex(getPointNumber(i1, i2, i3)).z;
	aabb.maxZ = getNodeByLocalIndex(getPointNumber(i1, i2, i3+1)).z;
}

void gcm::MarkeredMesh::getNumberOfCells(int &num1, int &num2, int &num3) {
	num1 = cells_num[0];
	num2 = cells_num[1];
	num3 = cells_num[2];
}

void gcm::MarkeredMesh::getNumberOfPoints(int &num1, int &num2, int &num3) {
	num1 = points_num[0];
	num2 = points_num[1];
	num3 = points_num[2];
}

unsigned int gcm::MarkeredMesh::getTotalNumberOfCells() {
	return cells_num[2]*cells_num[1]*cells_num[0];
}

int gcm::MarkeredMesh::getPointNumber(unsigned int i, unsigned int j, unsigned int k) {
	assert(i < points_num[0] && j < points_num[1] && k < points_num[2] && "Point indexes are out of range");
	return k*points_num[1]*points_num[0]+j*points_num[0]+i;
}

int gcm::MarkeredMesh::getCellNumber(unsigned int i, unsigned int j, unsigned int k) {
	assert(i < cells_num[0] && j < cells_num[1] && k < cells_num[2] && "Cell indexes are out of range");
	return k*cells_num[1]*cells_num[0]+j*cells_num[0]+i;
}

void gcm::MarkeredMesh::setCellInnerFlag(unsigned int num, bool flag) {
	cellsInnerFlags[num] = flag;
}

bool gcm::MarkeredMesh::getCellInnerFlag(unsigned int num) {
	return cellsInnerFlags[num];
}

int gcm::MarkeredMesh::generateMarkers() {
	unsigned int nc = getTotalNumberOfCells();
	LOG_DEBUG("Generating markers for " << nc << " cells");
	markers.clear();
	for (unsigned int i = 0; i < nc; i++)
		if (!getCellInnerFlag(i)) {
			NeighbourCells cells;
			auto n = getCellNeighbours(i, cells);
			bool flag = false;
			for (unsigned int j = 0; j < n; j++)
				if (getCellInnerFlag(cells[j])) {
					flag = true;
					break;
				}
			if (!flag)
				continue;
			AABB aabb;
			getCellAABB(i, aabb);

			Node node;
			node.x = (aabb.minX + aabb.maxX) / 2;
			node.y = (aabb.minY + aabb.maxY) / 2;
			node.z = (aabb.minZ + aabb.maxZ) / 2;

			markers.push_back(node);
		}
	return markers.size();
}

void gcm::MarkeredMesh::reconstructInnerFlags() {
	LOG_DEBUG("Reconstructing cells inner flags");

	for (auto f: cellsInnerFlags)
		f = true;


	for (auto &m: markers) {
		long n = getCellForPoint(m.x, m.y, m.z);
		setCellInnerFlag(n, false);
	}
	// FIXME
	// we need to be sure that reconstructed border is closed

	vector<unsigned int> q;
	// FIXME
	// here we always start from 0 cell, but it may lead to incorrect
	// flags reconstruction object move far away from mesh center
	unsigned int nc = getTotalNumberOfCells();
	bool *queued = new bool[nc];
	memset(queued, 0, sizeof(bool)*nc);

	q.push_back(0);
	queued[0] = true;
	while (q.size()) {
		unsigned int n = q.back();
		q.pop_back();
		setCellInnerFlag(n, false);
		NeighbourCells cells;
		for (unsigned int i = 0; i < getCellNeighbours(n, cells); i++)
			if (!queued[cells[i]] && getCellInnerFlag(cells[i])) {
				q.push_back(cells[i]);
				queued[cells[i]] = true;
			}
	}

	delete[] queued;
}

long gcm::MarkeredMesh::getCellForPoint(float x, float y, float z) {
	if (!meshOutline.isInAABB(x, y, z))
		return -1;

	unsigned int i1 = floor((x-meshOutline.minX)/h[0]);
	unsigned int i2 = floor((y-meshOutline.minY)/h[1]);
	unsigned int i3 = floor((z-meshOutline.minZ)/h[2]);

	return getCellNumber(i1, i2, i3);
}

gcm::MarkeredMesh::MarkeredMesh() {
	INIT_LOGGER("gcm.MarkeredMesh");
	numericalMethodType = "InterpolationFixedAxis";
	snapshotWriterType = "VTKMarkeredMeshSnapshotWriter";
	dumpWriterType = "VTKMarkeredMeshSnapshotWriter";	
	interpolator = new LineFirstOrderInterpolator();
}

void gcm::MarkeredMesh::createOutline() {
	LOG_DEBUG("Creating outline");
	for (int i = 0; i < 3; i++) {
		outline.min_coords[i] = numeric_limits<float>::infinity();
		outline.max_coords[i] = -numeric_limits<float>::infinity();
	}
	for (unsigned int i = 0; i < getTotalNumberOfCells(); i++)
		if (getCellInnerFlag(i)) {
			AABB aabb;
			getCellAABB(i, aabb);
			for (int j = 0; j < 3; j++) {
				if (aabb.min_coords[j] < outline.min_coords[j])
					outline.min_coords[j] = aabb.min_coords[j];
				if (aabb.max_coords[j] > outline.max_coords[j])
					outline.max_coords[j] = aabb.max_coords[j];
			}
		}
	LOG_DEBUG("Creating mesh outline");
	AABB aabb;

	getCellAABB(0, aabb);
	memcpy(meshOutline.min_coords, aabb.min_coords, sizeof(aabb.min_coords));
	getCellAABB(getTotalNumberOfCells()-1, aabb);
	memcpy(meshOutline.max_coords, aabb.max_coords, sizeof(aabb.max_coords));
}

unsigned int gcm::MarkeredMesh::getCellNeighbours(unsigned int n, NeighbourCells &cells) {
	unsigned int idx[3], ci = 0;

	getCellIndexes(n, idx[0], idx[1], idx[2]);

	for (int i3 = -1; i3 <= 1; i3++)
		for (int i2 = -1; i2 <= 1; i2++)
			for (int i1 = -1; i1 <= 1; i1++)
				if (i1 || i2 || i3) {
					int _i1 = idx[0] + i1;
					int _i2 = idx[1] + i2;
					int _i3 = idx[2] + i3;
					// FIXME
					// is the any way to avoid comparsion between signed and unsigned values?
					// it's  safe comparsion, but it should be fixed
					if (_i1 >= 0 && _i1 < static_cast<int>(cells_num[0]) &&
						_i2 >= 0 && _i2 < static_cast<int>(cells_num[1]) &&
						_i3 >= 0 && _i3 < static_cast<int>(cells_num[2]))
						cells[ci++] = getCellNumber(_i1, _i2, _i3);
				}

	return ci;
}

const vector<Node>& gcm::MarkeredMesh::getMarkers() {
	return markers;
}

const AABB& gcm::MarkeredMesh::getMeshOutline() {
	return meshOutline;
}

void gcm::MarkeredMesh::reconstructBorder() {
	LOG_DEBUG("Reconstructing border");
	NeighbourCells cells;
	auto nc = getTotalNumberOfCells();

	for (int i = 0; i < getNodesNumber(); i++)
		getNodeByLocalIndex(i).setIsBorder(false);

	for (unsigned int i = 0; i < nc; i++)
		if (!getCellInnerFlag(i))
			for (unsigned int j = 0; j < getCellNeighbours(i, cells); j++) {
				auto c2 = cells[j];
				if (getCellInnerFlag(c2)) {
					CommonPoints pts;
					for (unsigned int k = 0; k < getCellsCommonPoints(i, c2, pts); k++)
						getNodeByLocalIndex(pts[k]).setIsBorder(true);
				}
			}
}

unsigned int gcm::MarkeredMesh::getCellsCommonPoints(unsigned int c1,
		unsigned int c2, CommonPoints& pts) {

	CellPoints pts1, pts2;

	getCellPoints(c1, pts1);
	getCellPoints(c2, pts2);

	unsigned int ci = 0;

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			if (pts1[i] == pts2[j]) {
				pts[ci++] = pts1[i];
				break;
			}

	return ci;
}

void gcm::MarkeredMesh::getCellPoints(unsigned int n, CellPoints& pts) {
	unsigned int idx[3];
	unsigned int ci = 0;

	getCellIndexes(n, idx[0], idx[1], idx[2]);

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
				pts[ci++] = getPointNumber(idx[0]+i, idx[1]+j, idx[2]+k);
}

void gcm::MarkeredMesh::calcMinH() {
}

void gcm::MarkeredMesh::markUnusedNodes() {
	LOG_DEBUG("Setting unused flags for nodes");
	for (unsigned int i = 0; i < getTotalNumberOfCells(); i++) {
		CellPoints pts;
		getCellPoints(i, pts);
		for (unsigned int j = 0; j < 8; j++)
			if (getCellInnerFlag(i))
				getNodeByLocalIndex(pts[j]).setUsed(true);
			else
				if (!getNodeByLocalIndex(pts[j]).isBorder())
					getNodeByLocalIndex(pts[j]).setUsed(false);
				else
					getNodeByLocalIndex(pts[j]).setUsed(true);
	}
}

void gcm::MarkeredMesh::findBorderNodeNormal(int border_node_index, float* x,
		float* y, float* z, bool debug) {
	*x = *y = *z = 0.0;

	CalcNode& node = getNode(border_node_index);
	assert(node.isBorder());

	auto idx = getNodeLocalIndex(border_node_index);

	unsigned int i1, i2, i3;
	getPointIndexes(idx, i1, i2, i3);

	assert(i1 > 0);
	assert(i2 > 0);
	assert(i3 > 0);
	assert(i1 < points_num[0]-1);
	assert(i2 < points_num[1]-1);
	assert(i3 < points_num[2]-1);

	for (int k = 1; k >= 0; k--)
		for (int j = 1; j >= 0; j-- )
			for (int i = 1; i >=0; i--)
				if (getCellInnerFlag(getCellNumber(i1-i, i2-j, i3-k))) {
					float dnx = (i-0.5)*h[0];
					float dny = (j-0.5)*h[1];
					float dnz = (k-0.5)*h[2];
					float l = vectorNorm(dnx, dny, dnz);
					dnx /= l;
					dny /= l;
					dnz /= l;

					*x += dnx;
					*y += dny;
					*z += dnz;
				}
}

void gcm::MarkeredMesh::interpolateNode(CalcNode& origin, float dx, float dy,
		float dz, bool debug, CalcNode& targetNode, bool& isInnerPoint) {
	assert(((dx == 0.0) && (dy == 0.0)) || ((dy == 0.0) && (dz == 0.0)) || ((dx == 0.0) && (dz == 0.0)));

	auto idx = getNodeLocalIndex(origin.number);
	unsigned int i1, i2, i3;

	getPointIndexes(idx, i1, i2, i3);

	int d;
	unsigned int *k, l;

	if (dx != 0.0) {
		d = sgn(dx);
		k = &i1;
		l = points_num[0];
	} else if (dy != 0.0) {
		d = sgn(dy);
		k = &i2;
		l = points_num[1];
	} else {
		d = sgn(dz);
		k = &i3;
		l = points_num[2];
	}

	isInnerPoint = true;

	if (d > 0) {
		if (*k >= l-1)
			isInnerPoint = false;
		else {
			*k += 1;
			if (!getNodeByLocalIndex(getPointNumber(i1, i2, i3)).isUsed())
				isInnerPoint = false;
		}
	} else {
		if (*k == 0)
			isInnerPoint = false;
		else {
			*k -= 1;
			if (!getNodeByLocalIndex(getPointNumber(i1, i2, i3)).isUsed())
				isInnerPoint = false;
		}
	}

	if (isInnerPoint) {
		targetNode.coords[0] = origin.coords[0]+dx;
		targetNode.coords[1] = origin.coords[1]+dy;
		targetNode.coords[2] = origin.coords[2]+dz;
		interpolator->interpolate(targetNode, origin, getNodeByLocalIndex(getPointNumber(i1, i2, i3)));

	} else {
		targetNode.coords[0] = origin.coords[0];
		targetNode.coords[1] = origin.coords[1];
		targetNode.coords[2] = origin.coords[2];
	}


}

void gcm::MarkeredMesh::moveMarkers(float dt) {
	for (auto& m: markers) {
		CellPoints pts;
		getCellPoints(getCellForPoint(m.x, m.y, m.z), pts);
		float v[3] = {0.0, 0.0, 0.0};
		int n = 0;
		for (int i = 0; i < 8; i++) {
			auto node = getNodeByLocalIndex(pts[i]);
			if (node.isUsed()) {
				for (int j = 0; j < 3; j++) 
					v[j] += node.velocity[j];
				n++;
			}
		}
		for (int i = 0; i < 3; i++)
			v[i] /= n;
		m.x += v[0]*dt;
		m.y += v[1]*dt;
		m.z += v[2]*dt;
	}
}