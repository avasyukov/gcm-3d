#ifndef _EULER_MESH_HPP
#define _EULER_MESH_HPP

#include "libgcm/mesh/Mesh.hpp"
#include "libgcm/util/Types.hpp"
#include "libgcm/interpolator/LineFirstOrderInterpolator.hpp"

namespace gcm
{
/**
 * Base implementation of Euler mesh.
 */
class EulerMesh: public Mesh
{
protected:
	/**
	 * Mesh dimensions (cells).
	 */
	vector3u dimensions;
	/**
     * Mesh dimensions (nodes).
     */
	vector3u nodeDimensions;
	/**
	 * Mesh cell size.
	 */
	vector3r cellSize;
	/**
	 * Mesh center.
	 */
	vector3r center;
	/**
	 * Mesh min h.
	 */
	float minH = 0.0;
	/**
	 * Logger.
	 */
	USE_LOGGER;
	/**
	 * Cell status.
	 */
    uchar*** cellStatus = nullptr;
    /**
     * Map containing border node normals.
     */
    std::unordered_map<uint, vector3r> borderNormals;

    static const uchar VIRT_FLAG;
    static const uchar VIRT_CELL_AXIS_X_FLAG;
    static const uchar VIRT_CELL_AXIS_Y_FLAG;
    static const uchar VIRT_CELL_AXIS_Z_FLAG;
public:
	/**
	 * Constructor.
	 * @param dimensions Vector containing three positive numbers that specify number of cells in eas dimension
	 * @param cellSize Vector that  defines cell size
	 * @param center Mesh center
	 */
	EulerMesh(vector3u dimensions, vector3r cellSize, vector3r center = {0, 0, 0});
	/**
	 * Default constructor.
	 */
	EulerMesh();
	/**
	 * Destructor.
	 */
	virtual ~EulerMesh();
	/**
	 * Calculates minimal h for mesh.
	 */
    void calcMinH() override;
    /**
     * Returns minimal h for mesh.
     * @return Minimal h for mesh.
     */
    float getMinH() override;
    /**
     * @return minimal h of the mesh
     */
	float getAvgH() override;
	/**
     * Returns recommended time step.
     * @return Recommended time step.
     */
    float getRecommendedTimeStep() override;
    /**
     * Performs next partial step.
     * @param tau Time step.
     * @param stage Stage number.
     */
    void doNextPartStep(float tau, int stage) override;
    /**
     * Returns mesh snapshot writer instance.
     * @return Mesh snapshot writer instance.
     */
    const SnapshotWriter& getSnaphotter() const override;
    /**
     * Returns mesh dumper instance.
     * @return Mesh dumper instance.
     */
    const SnapshotWriter& getDumper() const override;
    /**
     * Interpolates node values at specified point.
     * @param origin Origin node that used to calculate specified point coords using offset.
     * @param dx X offset.
     * @param dy Y offset.
     * @param dz Z offset.
     * @param debug Debug flag.
     * @param targetNode Node to store interpolated values at.
     * @param isInnerPoint Variable to store boolean value that indicates if point is inner or not,
     * @return True if point is inner ant false otherwise.
     */
    bool interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug, CalcNode& targetNode, bool& isInnerPoint) override;
    /**
     * Interpolates border node.
     * @param x X coord of point outside of mesh.
     * @param y Y coord of point outside of mesh.
     * @param z Z coord of point outside of mesh.
     * @param dx X offset.
     * @param dy Y offset.
     * @param dz Z offset
     * @param node Node to store interpolated values at
     * @return True if interpolated and false otherwise.
     */
    bool interpolateBorderNode(real x, real y, real z, real dx, real dy, real dz, CalcNode& node) override;
    /**
     * Interpolates values at an arbitrary point.
     * @param node Point to interpolate at.
     * @return True if interpolated and false otherwise
     */
    bool interpolateNode(CalcNode& node) override;
    bool interpolateNode(CalcNode& node, const vector3u& index);
    /**
     * Returns normal for border node.
     * @param border_node_index Border node index.
     * @param x X normal component.
     * @param y Y normal component.
     * @param z Z normal component.
     * @param debug Debug flag.
     */
    void findBorderNodeNormal(const CalcNode& node, float* x, float* y, float* z, bool debug) override;
    /**
     * Returns node local indexes.
     *
     * @param node Node to get indexes for.
     * @param indexes Node indexes.
     * @returns True if node is in mesh and false otherwise.
     */
    bool getNodeEulerMeshIndex(const CalcNode& node, vector3u& indexes) const;
    /**
     * Returns local index for node specified by Euler mesh node index.
     * @param Euler mesh node index
     * @return
     */
    int getNodeLocalIndexByEulerMeshIndex(const vector3u& index) const;
    /**
     * Returns mesh node specified by Euler mesh node index
     * @param index Euler mesh node index
     * @return Node
     */
    CalcNode& getNodeByEulerMeshIndex(const vector3u& index);

    void setCellSize(const vector3r& cellSize);
    void setDimensions(const vector3u& dimensions);
    void setCenter(const vector3r& center);

    const vector3r& getCellSize() const;
    const vector3u& getDimensions() const;
    const vector3u& getNodeDimensions() const;

    void generateMesh();

    vector3u getCellEulerIndexByCoords(const vector3r& coords) const;
    vector3i getCellEulerIndexByCoordsUnsafe(const vector3r& coords) const;
    uint getCellLocalIndexByEulerIndex(const vector3u& index) const;
    vector3r getCellCenter(const vector3u& index) const;

    uchar getCellStatus(const vector3u& index) const;
};
}

#endif
