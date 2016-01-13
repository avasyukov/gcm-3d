#include "libgcm/snapshot/VTKCubicSnapshotWriter.hpp"

#ifdef CONFIG_VTK_5
#include <vtkstd/string>
#else
#include <vtkStdString.h>
#endif
#include <vtkStructuredGrid.h>
#include <vtkXMLStructuredGridWriter.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>

#include "libgcm/mesh/cube/BasicCubicMesh.hpp"

using namespace gcm;

VTKCubicSnapshotWriter::VTKCubicSnapshotWriter() {
    INIT_LOGGER("gcm.VTKCubicSnapshotWriter");
    extension = "vts";
}

void VTKCubicSnapshotWriter::dumpMeshSpecificData(BasicCubicMesh* mesh, 
	vtkSmartPointer<vtkStructuredGrid>& grid, vtkSmartPointer<vtkPoints>& points) const
{
    for(int i = 0; i < mesh->getNodesNumber(); i++)
    {
        CalcNode& node = mesh->getNodeByLocalIndex(i);
        points->InsertNextPoint( node.coords[0], node.coords[1], node.coords[2] );
    }

    // Specify the dimensions of the grid
    AABB outline = mesh->getOutline();
    // We suppose that mesh is uniform
    int meshSizeX = 1 + mesh->getNumX();
	int meshSizeY = 1 + mesh->getNumY();
	int meshSizeZ = 1 + mesh->getNumZ();
    grid->SetDimensions(meshSizeX, meshSizeY, meshSizeZ);
}
