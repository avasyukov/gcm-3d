#include "libgcm/snapshot/VTKEulerMeshSnapshotWriter.hpp"

#include "libgcm/Math.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/mesh/euler/EulerMesh.hpp"

using namespace gcm;

VTKEulerMeshSnapshotWriter::VTKEulerMeshSnapshotWriter()
{
    INIT_LOGGER("gcm.snapshot.VTKEulerMeshSnapshotWriter");
    extension = "vts";
}

void VTKEulerMeshSnapshotWriter::dumpMeshSpecificData(EulerMesh* mesh, vtkSmartPointer<vtkStructuredGrid>& grid, vtkSmartPointer<vtkPoints>& points) const
{
    auto nodeDims = mesh->getNodeDimensions();
    auto cellDims = mesh->getDimensions();

    auto cellStatus = vtkSmartPointer<vtkIntArray>::New();
    cellStatus->SetName("cellStatus");

    for (uint i = 0; i < nodeDims.x; i++)
        for (uint j = 0; j < nodeDims.y; j++)
            for (uint k = 0; k < nodeDims.z; k++)
            {
                auto& node = mesh->getNodeByEulerMeshIndex(vector3u(i, j, k));
                points->InsertNextPoint(node.coords.x, node.coords.y, node.coords.z);
            }

    for (uint i = 0; i < cellDims.x; i++)
        for (uint j = 0; j < cellDims.y; j++)
            for (uint k = 0; k < cellDims.z; k++)
                cellStatus->InsertNextValue(mesh->getCellStatus(vector3u(i, j, k)) ? 1 : 0);

    grid->GetCellData()->AddArray(cellStatus);

    grid->SetDimensions(nodeDims.x, nodeDims.y, nodeDims.z);
}